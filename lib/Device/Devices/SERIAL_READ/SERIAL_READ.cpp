#include "./SERIAL_READ.h"

void SERIAL_READ_Device::loop()
{
  while (Serial.available() > 0) {
    _lastByteTime = millis();
    _handle_byte(Serial.read());
  }

  if (_state != WAIT_TYPE && (millis() - _lastByteTime) > 1000) {
    Serial.println("Frame timeout — resyncing");
    if (_isBufferMode) {
      _flush_stream_chunk();
      on_buffer_end(_typeId, _streamReceived);
    }
    _reset_parser();
  }
}

void SERIAL_READ_Device::_reset_parser() {
  _state = WAIT_TYPE;
  _lenBytesSeen = 0;
  _payloadLen = 0;
  _payloadIndex = 0;
  _streamChunkIndex = 0;
  _streamReceived = 0;
  _isBufferMode = false;
}

/**------------------------------------------------------------------------
 *?                           PARSING
 *------------------------------------------------------------------------**/
void SERIAL_READ_Device::_handle_byte(uint8_t b) {
  switch (_state) {

    case WAIT_TYPE:
      _type = b;
      _typeId = b & SR_TYPE_MASK;
      _isBufferMode = (b & SR_TYPE_BUFFER_FLAG) != 0;
      _lenBytesSeen = 0;
      _payloadLen = 0;
      _state = WAIT_LEN;
      break;

    case WAIT_LEN:
      _lenBuffer[_lenBytesSeen++] = b;
      if (_lenBytesSeen == 2) {
        _payloadLen = ((uint16_t)_lenBuffer[0] << 8)
                    |  (uint16_t)_lenBuffer[1];

        _payloadIndex = 0;
        _streamChunkIndex = 0;
        _streamReceived = 0;

        if (_payloadLen == 0) {
          if (_isBufferMode) {
            on_buffer_start(_typeId, 0);
            on_buffer_end(_typeId, 0);
          } else {
            _execute_command(_typeId, _payload, 0);
          }
          _reset_parser();
        } else if (!_isBufferMode && _payloadLen > SR_MAX_PAYLOAD) {
          Serial.print("Payload too large: ");
          Serial.println(_payloadLen);
          _reset_parser();
        } else {
          if (_isBufferMode) {
            on_buffer_start(_typeId, _payloadLen);
          }
          _state = WAIT_PAYLOAD;
        }
      }
      break;

    case WAIT_PAYLOAD:
      if (_isBufferMode) {
        _streamChunk[_streamChunkIndex++] = b;
        _streamReceived++;

        if (_streamChunkIndex >= SR_STREAM_CHUNK) {
          _flush_stream_chunk();
        }

        if (_streamReceived >= _payloadLen) {
          _flush_stream_chunk();
          on_buffer_end(_typeId, _streamReceived);
          _reset_parser();
        }
      } else {
        _payload[_payloadIndex++] = b;
        if (_payloadIndex >= _payloadLen) {
          _execute_command(_typeId, _payload, _payloadLen);
          _reset_parser();
        }
      }
      break;
  }
}

void SERIAL_READ_Device::_flush_stream_chunk() {
  if (_streamChunkIndex == 0) return;
  on_buffer_chunk(_typeId, _streamChunk, _streamChunkIndex);
  _streamChunkIndex = 0;
}

/**------------------------------------------------------------------------
 *?                           EXECUTE
 *------------------------------------------------------------------------**/
void SERIAL_READ_Device::_execute_command(uint8_t type, uint8_t *data, uint32_t len) {
  switch (type)
  {
  case 0x00:
    /* code */
    break;
  
  default:
    break;
  }
}