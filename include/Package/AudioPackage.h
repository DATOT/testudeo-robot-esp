#ifndef __AUDIO_PACK_HPP__
#define __AUDIO_PACK_HPP__

#include "BasePackage.h"

class AudioPackage : public BasePackage
{
public:
	explicit AudioPackage(
			uint16_t *buffer = nullptr,
			uint8_t buffer_size = 0) : BasePackage((uint8_t)0xa0, buffer_size, (uint8_t *)buffer) {}
};

#endif // !__AUDIO_PACK_HPP__