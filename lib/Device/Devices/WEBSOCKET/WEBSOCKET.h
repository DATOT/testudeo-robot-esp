#ifndef __WEBSOCKET_DEVICE_H__
#define __WEBSOCKET_DEVICE_H__

#include <Arduino.h>
#include <WebSocketsClient.h>
#include "Device/BaseDevice.h"

class WEBSOCKET_Device : public BaseDevice {
private:
  WebSocketsClient _socket;
  bool _mdnsStarted = false;
  bool _socketStarted = false;

  void _on_event(WStype_t type, uint8_t *payload, size_t length);
  void _connect_to_app();
  void _handle_server_message(const uint8_t *payload, size_t length);

public:
  void setup() override;
  void loop() override;
  bool has_loop() const override { return true; }

  bool connected() const;
  bool send_audio(const uint8_t *data, size_t length);
  bool send_text(const char *text);
};

#endif
