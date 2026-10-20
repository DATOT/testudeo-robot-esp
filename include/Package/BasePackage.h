#ifndef __BASE_PACK_H__
#define __BASE_PACK_H__

#define SEND_ENABLED false
#include <Arduino.h>

/**------------------------------------------------------------------------
 * *                                INFO
 * * Package: Gói dữ liệu được gửi qua Serial để đến backend
 * * Gồm 2 phần: header và buffer
 * ! header gói dữ liệu: 00000000 00000000 (u16)
 * !                     (header) [length]
 * ? Giải thích: 8 bit đầu là để xác định gói dữ liệu gửi gì
 * ?             8 bit còn lại là cho độ dài buffer(theo bytes)
 * ! buffer: [________(dữ liệu dạng u8, 1 byte), ...]
 *
 *------------------------------------------------------------------------**/

/**-----------------------
 * !      WARNING
 * ! Không được thay đổi code ở đây
 *------------------------**/

class BasePackage
{
private:
	uint8_t _package_raw_header_bytes[2] = {0, 0};
	uint8_t *_buffer = nullptr;
	uint8_t _buffer_size = 0;

protected:
	uint8_t _init_header = 0;

public:
	// Khởi tạo gói dữ liệu
	explicit BasePackage(
			uint8_t header,
			uint8_t buffer_size = 0,
			uint8_t *buffer = nullptr) : _init_header(header)
	{
		update_data(buffer_size, buffer);
	}

	~BasePackage() = default;

	// Cập nhật thông tin của gói dữ liệu
	bool update_data(
			uint8_t buffer_size,
			uint8_t *buffer)
	{
		_buffer = buffer;
		_buffer_size = buffer_size;

		_package_raw_header_bytes[0] = _init_header;
		_package_raw_header_bytes[1] = _buffer_size;

		return true;
	}

	// Gửi gói dữ liệu qua Serial
	bool send()
	{
		#if SEND_ENABLED
		if (Serial.write(_package_raw_header_bytes, 2) != 2)
		{
			return false;
		}

		if (_buffer != nullptr && _buffer_size > 0)
		{
			if (Serial.write(_buffer, _buffer_size) != _buffer_size)
			{
				return false;
			}
		}

	#endif
		return true;
	}
};

#endif // !__BASE_PACK_H__