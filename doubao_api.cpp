#include "doubao_api.h"
#include "k10_base64.h"

// Error codes
const String ERROR_NETWORK = "<network_error>";
const String ERROR_CAMERA = "<camera_error>";
const String ERROR_IMAGE_TOO_LARGE = "<image_too_large>";
const String ERROR_INVALID_INPUT = "<invalid_input>";
const String ERROR_JSON_PARSE = "<json_parse_error>";
const String ERROR_TIMEOUT = "<timeout_error>";

// Global answer variable
String answer;

bool validateConfig(const char* apiKey, String modelId, float temp) {
  if (apiKey == nullptr || strlen(apiKey) == 0) {
    Serial.println("Error: API key not set");
    return false;
  }
  if (modelId.length() == 0) {
    Serial.println("Error: Model ID not set");
    return false;
  }
  if (temp < 0.0 || temp > 1.0) {
    Serial.println("Error: Temperature parameter out of range (0–1)");
    return false;
  }
  return true;
}

String sendHttpRequest(String payload, const char* apiKey) {
  WiFiClientSecure client;
  String outputText = ERROR_NETWORK;
  client.setInsecure();
  if (!client.connect("ark.cn-beijing.volces.com", 443)) {
    Serial.println("Failed to connect to server");
    return ERROR_NETWORK;
  }
  String request = "POST /api/v3/chat/completions HTTP/1.1\r\n";
  request += "Host: ark.cn-beijing.volces.com\r\n";
  request += "Content-Type: application/json\r\n";
  request += "Authorization: Bearer " + String(apiKey) + "\r\n";
  request += "Transfer-Encoding: chunked\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  client.print(request);
  int chunkSize = 4096;
  int totalLength = payload.length();
  int sent = 0;
  while (sent < totalLength) {
    int currentChunkSize = min(chunkSize, totalLength - sent);
    String chunk = payload.substring(sent, sent + currentChunkSize);
    String chunkHeader = String(currentChunkSize, HEX) + "\r\n";
    client.print(chunkHeader);
    client.print(chunk);
    client.print("\r\n");
    sent += currentChunkSize;
    delay(5);
  }
  client.print("0\r\n\r\n");
  String response = "";
  unsigned long timeout = millis() + 300000;
  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      response += client.readString();
    }
    delay(10);
  }
  client.stop();
  if (response.length() > 0) {
    int jsonStart = response.indexOf("\r\n\r\n");
    if (jsonStart > 0) {
      response = response.substring(jsonStart + 4);
    }
    DynamicJsonDocument jsonDoc(32768);
    DeserializationError error = deserializeJson(jsonDoc, response);
    if (!error) {
      outputText = jsonDoc["choices"][0]["message"]["content"].as<String>();
      Serial.println("JSON Parse Successful");
    } else {
      Serial.print("JSON Parse Error: ");
      Serial.println(error.c_str());
      outputText = ERROR_JSON_PARSE;
    }
  } else {
    Serial.println("No response received");
    outputText = ERROR_TIMEOUT;
  }
  return outputText;
}

String sendHttpRequestWithRetry(String payload, const char* apiKey, int maxRetries) {
  String result;
  for (int i = 0; i < maxRetries; i++) {
    Serial.printf("Requesting %d/%d\n", i + 1, maxRetries);
    result = sendHttpRequest(payload, apiKey);
    if (result != ERROR_NETWORK && result != ERROR_TIMEOUT) {
      return result;
    }
    if (i < maxRetries - 1) {
      int delayTime = 1000 * (i + 1);
      Serial.printf("Request failed, retrying after %d ms\n", delayTime);
      delay(delayTime);
    }
  }
  return result;
}

String buildPayload(String inputText, String modelId, String systemPrompt, float temp, String base64Image, String imageFormat) {
  String payload;
  payload.reserve(2000);
  payload = "{";
  payload += "\"model\":\"" + modelId + "\",";
  payload += "\"messages\":[";
  payload += "{\"role\":\"system\",\"content\":\"" + systemPrompt + "\"},";
  payload += "{";
  payload += "\"role\":\"user\",";
  payload += "\"content\":[";
  if (base64Image != "" && base64Image != "NULL") {
    payload += "{\"type\":\"image_url\",";
    payload += "\"image_url\":{\"url\":\"data:image/" + imageFormat + ";base64," + base64Image + "\"}},";
  }
  payload += "{\"type\":\"text\",\"text\":\"" + inputText + "\"}";
  payload += "]";
  payload += "}";
  payload += "],";
  payload += "\"temperature\":" + String(temp);
  payload += "}";
  return payload;
}

String getGPTAnswer(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp) {
  if (!validateConfig(apiKey, modelId, temp)) {
    return ERROR_INVALID_INPUT;
  }
  if (inputText.length() == 0) {
    Serial.println("Error: Input text is empty");
    return ERROR_INVALID_INPUT;
  }
  String payload = buildPayload(inputText, modelId, systemPrompt, temp);
  Serial.printf("Send text request, payload length:: %d\n", payload.length());
  return sendHttpRequestWithRetry(payload, apiKey);
}

String getGPTAnswer_urlimg(String inputText, String imageUrl, const char* apiKey, String modelId, String systemPrompt, float temp) {
  if (inputText.length() == 0) {
    Serial.println("Error: Input text is empty");
    return ERROR_INVALID_INPUT;
  }
  String payload;
  payload.reserve(2000);
  payload = "{";
  payload += "\"model\":\"" + modelId + "\",";
  payload += "\"messages\":[";
  payload += "{\"role\":\"system\",\"content\":\"" + systemPrompt + "\"},";
  if (imageUrl != "") {
    payload += "{\"role\":\"user\",\"content\":[";
    payload += "{\"type\":\"image_url\",\"image_url\":{\"url\":\"" + imageUrl + "\"}},";
    payload += "{\"type\":\"text\",\"text\":\"" + inputText + "\"}";
    payload += "]}";
  } else {
    payload += "{\"role\":\"user\",\"content\":\"" + inputText + "\"}";
  }
  payload += "],";
  payload += "\"temperature\":" + String(temp);
  payload += "}";
  Serial.printf("Send URL image request, payload length: %d\n", payload.length());
  return sendHttpRequestWithRetry(payload, apiKey);
}

String getGPTAnswer_camera(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp) {
  if (inputText.length() == 0) {
    Serial.println("Error: Input text is empty");
    return ERROR_INVALID_INPUT;
  }
  K10_base64 k10base64;
  String base64Image = k10base64.K10tobase64();
  if (base64Image == "NULL" || base64Image.length() == 0) {
    Serial.println("Failed to capture image");
    return ERROR_CAMERA;
  }
  String payload = buildPayload(inputText, modelId, systemPrompt, temp, base64Image, "jpg");
  Serial.printf("Send camera image request, payload length: %d\n", payload.length());
  return sendHttpRequestWithRetry(payload, apiKey);
}

String getChoice(String msg_json, String msg_key) {
  const char* jsonStr = msg_json.c_str();
  ArduinoJson::DynamicJsonDocument doc(512);
  ArduinoJson::DeserializationError error = ArduinoJson::deserializeJson(doc, jsonStr);
  if (!error) {
    if (doc.containsKey(msg_key)) {
      String choice = doc[msg_key].as<String>();
      return choice;
    } else {
      return "Parsing failed";
    }
  } else {
    return "JSON syntax error";
  }
}
