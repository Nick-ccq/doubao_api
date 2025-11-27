# Doubao API Library

An Arduino library for integrating with Doubao (ByteDance) AI API, providing text-based conversation and vision capabilities for ESP32 and other WiFi-enabled Arduino platforms.

## Features

- 🤖 **Text-based AI Chat**: Send text messages and receive AI-generated responses
- 🖼️ **Image URL Analysis**: Analyze images from URLs with AI vision capabilities
- 📷 **Camera Integration**: Capture and analyze images directly from K10 camera module
- 🔄 **Automatic Retry**: Built-in retry mechanism for network reliability
- ⚡ **Chunked Transfer**: Efficient handling of large payloads
- 🛡️ **Error Handling**: Comprehensive error codes and validation
- 📦 **JSON Processing**: Automatic JSON parsing and response extraction

## Hardware Requirements

- ESP32 or other Arduino-compatible board with WiFi
- K10 camera module (for camera-based features)
- Stable WiFi connection

## Software Dependencies

- `ArduinoJson` library
- `WiFiClientSecure` library
- `HTTPClient` library
- `k10_base64` library (for camera features)

## Installation

### Arduino IDE

1. Download this repository as a ZIP file
2. Open Arduino IDE
3. Go to **Sketch** → **Include Library** → **Add .ZIP Library**
4. Select the downloaded ZIP file
5. Restart Arduino IDE

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps =
    doubao_api
    ArduinoJson
```

## Quick Start

### Basic Text Chat Example

```cpp
#include <WiFi.h>
#include <doubao_api.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
const char* apiKey = "your_doubao_api_key";

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    
    // Send a message to AI
    String response = getGPTAnswer(
        "Hello, how are you?",           // Input text
        apiKey,                          // API key
        "your-model-id",                 // Model ID
        "You are a helpful assistant",   // System prompt
        0.7                              // Temperature (0.0-1.0)
    );
    
    if (response.startsWith("<")) {
        Serial.println("Error: " + response);
    } else {
        Serial.println("AI Response: " + response);
    }
}

void loop() {
    // Your code here
}
```

### Image URL Analysis Example

```cpp
#include <WiFi.h>
#include <doubao_api.h>

const char* apiKey = "your_doubao_api_key";

void setup() {
    Serial.begin(115200);
    // WiFi setup...
    
    String response = getGPTAnswer_urlimg(
        "What's in this image?",                    // Question about image
        "https://example.com/image.jpg",            // Image URL
        apiKey,                                     // API key
        "your-model-id",                            // Model ID
        "You are an image analysis assistant",      // System prompt
        0.7                                         // Temperature
    );
    
    Serial.println("AI Analysis: " + response);
}

void loop() {}
```

### Camera Image Analysis Example

```cpp
#include <WiFi.h>
#include <doubao_api.h>

const char* apiKey = "your_doubao_api_key";

void setup() {
    Serial.begin(115200);
    // WiFi setup...
    // Camera initialization...
    
    String response = getGPTAnswer_camera(
        "Describe what you see in this image",      // Question
        apiKey,                                     // API key
        "your-model-id",                            // Model ID
        "You are a vision AI assistant",            // System prompt
        0.7                                         // Temperature
    );
    
    Serial.println("AI Vision Response: " + response);
}

void loop() {}
```

## API Reference

### Functions

#### `bool validateConfig(const char* apiKey, String modelId, float temp)`

Validates configuration parameters before making API requests.

**Parameters:**
- `apiKey`: API key to validate (must not be NULL or empty)
- `modelId`: Model ID to validate (must not be empty)
- `temp`: Temperature parameter (must be between 0.0 and 1.0)

**Returns:**
- `true` if all parameters are valid
- `false` if any parameter is invalid

**Example:**
```cpp
if (validateConfig(apiKey, "model-id", 0.7)) {
    Serial.println("Configuration is valid");
}
```

---

#### `String getGPTAnswer(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp)`

Send a text message to Doubao AI and get a response.

**Parameters:**
- `inputText`: The text message to send
- `apiKey`: Your Doubao API key
- `modelId`: The model ID to use
- `systemPrompt`: System role/instructions for the AI
- `temp`: Temperature parameter (0.0-1.0) - higher values = more creative

**Returns:**
- AI response text on success
- Error code string on failure (see Error Codes section)

**Example:**
```cpp
String answer = getGPTAnswer(
    "Explain quantum computing",
    apiKey,
    "doubao-model-v1",
    "You are a physics teacher",
    0.5
);
```

---

#### `String getGPTAnswer_urlimg(String inputText, String imageUrl, const char* apiKey, String modelId, String systemPrompt, float temp)`

Analyze an image from a URL with AI vision capabilities.

**Parameters:**
- `inputText`: Question or instruction about the image
- `imageUrl`: URL of the image to analyze
- `apiKey`: Your Doubao API key
- `modelId`: The model ID (must support vision)
- `systemPrompt`: System role/instructions
- `temp`: Temperature parameter (0.0-1.0)

**Returns:**
- AI analysis response on success
- Error code string on failure

**Example:**
```cpp
String analysis = getGPTAnswer_urlimg(
    "What objects are in this image?",
    "https://example.com/photo.jpg",
    apiKey,
    "doubao-vision-model",
    "You are an image analyst",
    0.7
);
```

---

#### `String getGPTAnswer_camera(String inputText, const char* apiKey, String modelId, String systemPrompt, float temp)`

Capture an image from K10 camera and analyze it with AI.

**Parameters:**
- `inputText`: Question or instruction about the captured image
- `apiKey`: Your Doubao API key
- `modelId`: The model ID (must support vision)
- `systemPrompt`: System role/instructions
- `temp`: Temperature parameter (0.0-1.0)

**Returns:**
- AI analysis response on success
- Error code string on failure

**Example:**
```cpp
String vision = getGPTAnswer_camera(
    "Identify all objects in view",
    apiKey,
    "doubao-vision-model",
    "You are a smart camera assistant",
    0.8
);
```

---

#### `String sendHttpRequest(String payload, const char* apiKey)`

Send an HTTP request to Doubao API (low-level function).

**Parameters:**
- `payload`: JSON payload string
- `apiKey`: API key for authentication

**Returns:**
- Response text from API
- Error code on failure

---

#### `String sendHttpRequestWithRetry(String payload, const char* apiKey, int maxRetries = 3)`

Send HTTP request with automatic retry mechanism.

**Parameters:**
- `payload`: JSON payload string
- `apiKey`: API key for authentication
- `maxRetries`: Maximum number of retry attempts (default: 3)

**Returns:**
- Response text from API
- Error code on failure

---

#### `String buildPayload(String inputText, String modelId, String systemPrompt, float temp, String base64Image = "", String imageFormat = "")`

Build JSON payload for API request.

**Parameters:**
- `inputText`: Text message
- `modelId`: Model ID
- `systemPrompt`: System prompt
- `temp`: Temperature parameter
- `base64Image`: Base64 encoded image (optional)
- `imageFormat`: Image format like "jpg" or "png" (optional)

**Returns:**
- JSON payload string

---

#### `String getChoice(String msg_json, String msg_key)`

Parse JSON and extract value by key.

**Parameters:**
- `msg_json`: JSON string to parse
- `msg_key`: Key to extract

**Returns:**
- Value of the key
- Error message on parse failure

**Example:**
```cpp
String json = "{\"status\":\"success\",\"code\":200}";
String status = getChoice(json, "status");  // Returns "success"
```

---

### Error Codes

The library uses the following error codes:

| Error Code | Constant | Description |
|------------|----------|-------------|
| `<network_error>` | `ERROR_NETWORK` | Network connection failed |
| `<camera_error>` | `ERROR_CAMERA` | Failed to capture camera image |
| `<image_too_large>` | `ERROR_IMAGE_TOO_LARGE` | Image exceeds size limit |
| `<invalid_input>` | `ERROR_INVALID_INPUT` | Invalid input parameters |
| `<json_parse_error>` | `ERROR_JSON_PARSE` | JSON parsing failed |
| `<timeout_error>` | `ERROR_TIMEOUT` | Request timeout |

**Example Error Handling:**
```cpp
String response = getGPTAnswer("Hello", apiKey, "model", "prompt", 0.7);

if (response == ERROR_NETWORK) {
    Serial.println("Network error - check WiFi connection");
} else if (response == ERROR_TIMEOUT) {
    Serial.println("Request timeout - try again");
} else if (response.startsWith("<")) {
    Serial.println("Error occurred: " + response);
} else {
    Serial.println("Success: " + response);
}
```

---

### Global Variables

#### `String answer`

Global variable that stores the last AI response. Can be used as an alternative to function return values.

**Example:**
```cpp
getGPTAnswer("Hello", apiKey, "model", "prompt", 0.7);
Serial.println("Last answer: " + answer);
```

## Advanced Usage

### Custom Temperature Settings

Temperature controls randomness in responses (0.0 to 1.0):

- **0.0 - 0.3**: Focused and deterministic (good for factual queries)
- **0.4 - 0.7**: Balanced creativity and coherence (general use)
- **0.8 - 1.0**: Highly creative and varied (creative writing)

```cpp
// Factual query - use low temperature
String fact = getGPTAnswer("What is 2+2?", apiKey, model, prompt, 0.1);

// Creative task - use high temperature
String story = getGPTAnswer("Write a short story", apiKey, model, prompt, 0.9);
```

### System Prompts

System prompts define the AI's behavior and role:

```cpp
// Technical assistant
String techPrompt = "You are a senior software engineer. Provide detailed technical explanations.";

// Friendly chatbot
String chatPrompt = "You are a friendly and helpful assistant. Keep responses concise.";

// Educational tutor
String tutorPrompt = "You are a patient teacher. Explain concepts step by step.";
```

### Error Recovery

Implement robust error handling with retries:

```cpp
String sendMessageWithRetry(String message, int maxAttempts = 3) {
    for (int i = 0; i < maxAttempts; i++) {
        String response = getGPTAnswer(message, apiKey, modelId, prompt, 0.7);
        
        if (!response.startsWith("<")) {
            return response;  // Success
        }
        
        Serial.printf("Attempt %d failed: %s\n", i + 1, response.c_str());
        delay(1000 * (i + 1));  // Exponential backoff
    }
    
    return ERROR_TIMEOUT;
}
```

## Best Practices

1. **WiFi Connection**: Always verify WiFi is connected before making API calls
2. **API Key Security**: Store API keys securely, avoid hardcoding in production
3. **Error Handling**: Always check response for error codes
4. **Rate Limiting**: Implement delays between consecutive API calls
5. **Memory Management**: Monitor heap usage when processing large responses
6. **Temperature Tuning**: Adjust temperature based on your use case
7. **Timeout Handling**: Implement appropriate timeouts for your application

## Troubleshooting

### Issue: Network Error

**Symptoms**: Returns `<network_error>`

**Solutions:**
- Verify WiFi connection is stable
- Check internet connectivity
- Ensure correct API endpoint access
- Verify firewall settings

### Issue: JSON Parse Error

**Symptoms**: Returns `<json_parse_error>`

**Solutions:**
- Increase JSON buffer size in code
- Check API response format
- Verify model ID is correct
- Review API documentation for changes

### Issue: Timeout Error

**Symptoms**: Returns `<timeout_error>`

**Solutions:**
- Increase timeout duration (default: 300 seconds)
- Check network speed and stability
- Reduce payload size
- Use retry mechanism

### Issue: Camera Error

**Symptoms**: Returns `<camera_error>`

**Solutions:**
- Verify K10 camera is properly initialized
- Check camera connections
- Ensure sufficient power supply
- Verify camera queue is working

## Examples

Complete examples are available in the `examples/` folder:

1. **BasicChat**: Simple text-based conversation
2. **ImageAnalysis**: Analyze images from URLs
3. **CameraVision**: Real-time camera image analysis
4. **ErrorHandling**: Comprehensive error handling example
5. **AdvancedConfig**: Advanced configuration options

## API Endpoint

This library connects to:
```
https://ark.cn-beijing.volces.com/api/v3/chat/completions
```

## License

This library is released under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Support

For questions and support:
- Open an issue on GitHub
- Check Doubao API documentation
- Review example code

## Changelog

### Version 1.0.0
- Initial release
- Text-based chat support
- Image URL analysis
- Camera integration
- Retry mechanism
- Comprehensive error handling

## Credits

Developed for Arduino/ESP32 platforms with Doubao AI integration.
