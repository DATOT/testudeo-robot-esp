#include "WEBSOCKET.h"
#include <WiFi.h>
#include <WiFiProv.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <vector>
#include <cstring>
#include "Global.hpp"
#include "AUDIO_OUT/AUDIO_OUT.h"

namespace {
constexpr char ROBOT_NAME[] = "Testudeo Robot";
constexpr char APP_HOST[] = "testudeo.local";
constexpr uint16_t APP_PORT = 8386;
constexpr char APP_PATH[] = "/ws/stt";

int base64_value(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}

std::vector<uint8_t> decode_base64(const char *input) {
  std::vector<uint8_t> out;
  if (!input) return out;
  uint32_t accumulator = 0;
  int bits = -8;
  for (const char *p = input; *p && *p != '='; ++p) {
    const int value = base64_value(*p);
    if (value < 0) continue;
    accumulator = (accumulator << 6) | static_cast<uint32_t>(value);
    bits += 6;
    if (bits >= 0) {
      out.push_back(static_cast<uint8_t>((accumulator >> bits) & 0xff));
      bits -= 8;
    }
  }
  return out;
}
}

void WEBSOCKET_Device::setup() {
  WiFi.mode(WIFI_STA);
  Serial.println("[WiFiProv] Starting BLE provisioning (security 0)");
  WiFi.beginProvision(provSchemeBLE,
                      WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM,
                      WIFI_PROV_SECURITY_0,
                      nullptr,
                      const_cast<char *>(ROBOT_NAME),
                      nullptr,
                      nullptr);
}

void WEBSOCKET_Device::loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!_mdnsStarted) {
      _mdnsStarted = MDNS.begin("robot");
      if (_mdnsStarted) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("[mDNS] Robot available at http://robot.local (IP %s)\n",
                      WiFi.localIP().toString().c_str());
      }
    }

    if (!_socketStarted) _connect_to_app();
    if (_socketStarted) _socket.loop();
  } else {
    if (_socketStarted) {
      _socket.disconnect();
      _socketStarted = false;
    }
    Global::state = Global::State::Disconnected;
    Global::mic_enabled = false;
  }
}

void WEBSOCKET_Device::_connect_to_app() {
  Serial.printf("[WebSocket] Looking up %s\n", APP_HOST);
  const IPAddress appIp = MDNS.queryHost(APP_HOST, 2500);
  if (appIp == INADDR_NONE) {
    Serial.println("[WebSocket] testudeo.local not found; retrying");
    delay(250);
    return;
  }

  Global::state = Global::State::Connecting;
  _socket.onEvent([this](WStype_t type, uint8_t *payload, size_t length) {
    _on_event(type, payload, length);
  });
  _socket.begin(appIp, APP_PORT, APP_PATH);
  _socket.setReconnectInterval(5000);
  _socket.enableHeartbeat(15000, 3000, 2);
  _socketStarted = true;
}

void WEBSOCKET_Device::_on_event(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Global::state = Global::State::Connected;
      Serial.println("[WebSocket] Connected to app /ws/stt");
      break;
    case WStype_DISCONNECTED:
      Global::state = Global::State::Disconnected;
      Global::mic_enabled = false;
      Serial.println("[WebSocket] Disconnected from app");
      break;
    case WStype_TEXT:
      _handle_server_message(payload, length);
      break;
    default:
      break;
  }
}

void WEBSOCKET_Device::_handle_server_message(const uint8_t *payload, size_t length) {
  JsonDocument message;
  if (deserializeJson(message, payload, length)) return;

  const char *type = message["type"] | "";
  if (strcmp(type, "mic") == 0) {
    Global::mic_enabled = message["enabled"] | false;
    return;
  }

  if (strcmp(type, "audio") == 0) {
    const char *encoded = message["samples"] | "";
    std::vector<uint8_t> bytes = decode_base64(encoded);
    const size_t sampleCount = bytes.size() / sizeof(float);
    std::vector<int16_t> pcm(sampleCount);
    for (size_t i = 0; i < sampleCount; ++i) {
      float sample = 0.0f;
      memcpy(&sample, bytes.data() + i * sizeof(float), sizeof(float));
      if (sample > 1.0f) sample = 1.0f;
      if (sample < -1.0f) sample = -1.0f;
      pcm[i] = static_cast<int16_t>(sample * 32767.0f);
    }
    if (manager && !pcm.empty()) {
      if (auto output = manager->get_first_device<AUDIO_OUT_Device>()) {
        (*output)->play_pcm16(pcm.data(), pcm.size());
      }
    }
    return;
  }

  if (strcmp(type, "done") == 0) {
    // The app sends done after all TTS chunks. Its server re-enables the mic
    // after playback; the mic command remains the authoritative state.
    return;
  }

  if (strcmp(type, "error") == 0) {
    Serial.printf("[WebSocket] Server error: %s\n", message["message"] | "unknown");
  }
}

bool WEBSOCKET_Device::connected() const {
  return _socket.isConnected();
}

bool WEBSOCKET_Device::send_audio(const uint8_t *data, size_t length) {
  return connected() && _socket.sendBIN(const_cast<uint8_t *>(data), length);
}

bool WEBSOCKET_Device::send_text(const char *text) {
  return connected() && _socket.sendTXT(text);
}
