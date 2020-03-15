#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_
#include <memory>
#include <vector>
#include "usbexcept.h"
#include "usbdevice.h"

namespace plv::usb
{

class Usb;

class UsbList : std::vector<UsbDevice>
{
private:
    struct Opaque;
    std::unique_ptr<struct Opaque> o_;
    friend Usb;
};

}
#endif // USB_DEVICE_H_
