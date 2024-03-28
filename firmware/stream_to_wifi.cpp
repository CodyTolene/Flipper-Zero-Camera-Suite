#include "stream_to_wifi.h"

AsyncWebServer server(80);
DNSServer dnsServer;

char ssid[30] = "ESP"; // Default SSID
char password[30] = "12345678"; // Default Password

bool is_wifi_streaming_initialized = false;

// Called from the main loop of the Firmware: `~firmware.ino`.
void stream_to_wifi() {
  if (!is_wifi_streaming_initialized) {
    is_wifi_streaming_initialized = true;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    
    Serial.print("AP SSID: ");
    Serial.println(ssid);

    dnsServer.start(53, "*", WiFi.softAPIP());
    server.begin();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      camera_fb_t * fb = NULL;
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        request->send(500);
        return;
      }
      AsyncWebServerResponse *response = 
        request->beginResponse(
          "image/jpeg", 
          fb->len, 
          [fb](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
            if(fb->len > alreadySent) {
              size_t toCopy = fb->len - alreadySent;
              if(toCopy > maxLen) {
                toCopy = maxLen;
              }
              memcpy(buffer, fb->buf + alreadySent, toCopy);
              return toCopy;
            }
            // Nothing is left to send, return 0.
            return 0;
          });
        response->addHeader("Connection", "close");
        request->send(response);
        esp_camera_fb_return(fb);
      });
  } else {
    dnsServer.processNextRequest();
  }
}

void start_wifi_stream() {
  // Make sure the camera is not streaming to serial.
  camera_model.isStreamToSerialEnabled = false;

  set_camera_config_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_model_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_defaults(CAMERA_FUNCTION_WIFI);

  // @todo - Dynamically set ssid and password via prompts.

  // Enable WiFi streaming.
  camera_model.isStreamToWiFiEnabled = true;

  // Turn the flash on momentarily to ensure the wifi started.
  // @todo - Remove after testing.
  turn_flash_on();
}

void stop_wifi_stream() {
  WiFi.softAPdisconnect(true);
  camera_model.isStreamToWiFiEnabled = false;
  is_wifi_streaming_initialized = false;

  // Turn the flash off after the wifi has started.
  // @todo - Remove after testing.
  turn_flash_off();
}
