#ifndef __CONNECTED_PACK_HPP__
#define __CONNECTED_PACK_HPP__

#include "BasePackage.h"

class ConnectedPackage : public BasePackage {
public:
    ConnectedPackage() : BasePackage((uint8_t)0x00, 0, nullptr) {}
};

#endif // !__CONNECTED_PACK_HPP__