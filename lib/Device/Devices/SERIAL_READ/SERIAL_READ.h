#ifndef __SERIAL_READ_H__
#define __SERIAL_READ_H__

#include <Arduino.h>
#include "Device/BaseDevice.h"

#define SR_MAX_PAYLOAD 4096

//* Bit 7 cho biết là đang streaming hay không
#define SR_TYPE_BUFFER_FLAG  0x80
#define SR_TYPE_MASK         0x7F

class SERIAL_READ_Device : public BaseDevice
{
//*-------------------------------- PUBLIC CALLBACKS ------------------------------*//
public:
  virtual void on_buffer_start(uint8_t type, uint32_t totalLen) {}
  virtual void on_buffer_chunk(uint8_t type, const uint8_t *data, uint32_t len) {}
  virtual void on_buffer_end(uint8_t type, uint32_t received) {}

//*-------------------------------- PRIVATE ------------------------------*//
private:
  //*-------------------------------- PRIVATE VARIABLES ------------------------------*//
  enum RxState {
    WAIT_TYPE,
    WAIT_LEN,
    WAIT_PAYLOAD
  };

  RxState  _state = WAIT_TYPE;
  uint8_t  _type = 0;             // full type byte (with bit7)
  uint8_t  _typeId = 0;           // masked type (bits 0-6)
  bool     _isBufferMode = false;

  uint16_t _payloadLen = 0;
  uint8_t  _lenBytesSeen = 0;
  uint8_t  _lenBuffer[2];

  // Normal-mode buffer
  uint8_t  _payload[SR_MAX_PAYLOAD];
  uint16_t _payloadIndex = 0;

  // Buffer-mode streaming chunk staging
  static const uint16_t SR_STREAM_CHUNK = 128;
  uint8_t  _streamChunk[SR_STREAM_CHUNK];
  uint16_t _streamChunkIndex = 0;
  uint32_t _streamReceived = 0;

  unsigned long _lastByteTime = 0;

  //*-------------------------------- PRIVATE FUNCTIONS ------------------------------*//
  
  void _reset_parser();
  void _handle_byte(uint8_t b);
  /**==============================================
     **              _execute_command
    *?  Xử lý lệnh được decode
    *@param type uint8_t   Xử lý lệnh gì
    *@param data uint8_t*  Dữ liệu để xử lý
    *@param len uint32_t*  Độ dài của data
    *@return void
    *=============================================**/
  void _execute_command(uint8_t type, uint8_t *data, uint32_t len);
  void _flush_stream_chunk();

//*-------------------------------- PUBLIC ------------------------------*//
public:
  void loop() override;
  inline bool has_loop() const override { return true; }
};

#endif // !__SERIAL_READ_H__