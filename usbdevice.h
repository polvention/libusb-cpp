#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_
#include <memory>
#include <vector>
#include "usbexcept"

namespace plv::usb
{

class Usb;

class UsbDevice
{
public:
    UsbDevice(uint16_t vendorId, uint16_t productId, uint8_t deviceClass) : vendorId_(vendorId), productId_(productId), deviceClass_(deviceClass) {}
private:
    uint16_t vendorId_;
    uint16_t productId_;
    uint8_t deviceClass_;

    struct Opaque;
    std::unique_ptr<struct Opaque> o_;
    friend Usb;
};

}
#endif // USB_DEVICE_H_
