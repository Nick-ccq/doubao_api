#ifndef DOUBAO_API_H
#define DOUBAO_API_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJsonK10.h>
#include <WiFiClientSecure.h>

// Error codes
extern const String ERROR_NETWORK;
extern const String ERROR_CAMERA;
extern const String ERROR_IMAGE_TOO_LARGE;
extern const String ERROR_INVALID_INPUT;
extern const String ERROR_JSON_PARSE;
extern const String ERROR_TIMEOUT;

// Global answer variable
extern String answer;

// Function declarations

/**
 * Validate configuration parameters
 * @param apiKey API key to validate
 * @param modelId Model ID to validate
 * @param temp Temperature parameter to validate
 * @return true if configuration is valid, false otherwise
 */
bool validateConfig(const char* apiKey, String modelId, float temp);

/**
 * Send HTTP request to Doubao API
 * @param payload JSON payload to send
 * @param apiKey API key for authentication
 * @return Response text or error code
 */
String sendHttpRequest(String payload, const char* apiKey);

/**
 * Send HTTP request with retry mechanism
 * @param payload JSON payload to send
 * @param apiKey API key for authentication
 * @param maxRetries Maximum number of retry attempts (default: 3)
 * @return Response text or error code
 */
String sendHttpRequestWithRetry(String payload, const char* apiKey, int maxRetries = 3);

/**
 * Build JSON payload for API request
 * @param inputText Text message to send
 * @param modelId Model ID to use
 * @param systemPrompt System prompt/role
 * @param temp Temperature parameter
 * @param base64Image Base64 encoded image (optional, default: "")
 * @param imageFormat Image format (e.g., "jpg", "png") (optional, default: "")
 * @return JSON payload string
 */
String buildPayload(String inputText, String modelId, String systemPrompt, float temp, String base64Image = "", String imageFormat = "");

/**
 * Get GPT answer for text message
 * @param inputText Text message to send
 * @param apiKey API key for authentication
 * @param modelId Model ID to use
 * @param systemPrompt System prompt/role
 * @param temp Temperature parameter
 * @return AI response or error code
 */
String getGPTAnswer(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp);

/**
 * Get GPT answer for text message with image URL
 * @param inputText Text message to send
 * @param imageUrl URL of the image
 * @param apiKey API key for authentication
 * @param modelId Model ID to use
 * @param systemPrompt System prompt/role
 * @param temp Temperature parameter
 * @return AI response or error code
 */
String getGPTAnswer_urlimg(String inputText, String imageUrl, const char* apiKey, String modelId, String systemPrompt, float temp);

/**
 * Get GPT answer for text message with camera photo
 * @param inputText Text message to send
 * @param apiKey API key for authentication
 * @param modelId Model ID to use
 * @param systemPrompt System prompt/role
 * @param temp Temperature parameter
 * @return AI response or error code
 */
String getGPTAnswer_camera(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp);

/**
 * Parse JSON and get value by key
 * @param msg_json JSON string to parse
 * @param msg_key Key to extract from JSON
 * @return Value of the key or error message
 */
String getChoice(String msg_json, String msg_key);

#endif // DOUBAO_API_H
