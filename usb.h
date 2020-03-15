#ifndef USB_H_
#define USB_H_
#include <memory>
#include <stdexcept>
#include <vector>
#include <functional>

namespace plv::usb
{

class UsbException: public std::exception
{
public:
    UsbException(std::string_view message)
    {
    }
private:
    std::string_view message_;
};

class Usb;

class UsbCallback
{
    struct Opaque;
    std::unique_ptr<struct Opaque> o_;
    friend Usb;
};

class UsbDevice
{
public:
    UsbDevice(uint16_t vendorId, uint16_t productId, uint8_t deviceClass) : vendorId_(vendorId), productId_(productId), deviceClass_(deviceClass), o_() {};
private:
    uint16_t vendorId_;
    uint16_t productId_;
    uint8_t deviceClass_;

    struct Opaque;
    std::unique_ptr<struct Opaque> o_;
    friend Usb;
};

class UsbHandle
{
    enum Event
    {
        Open,
        Close,
    };
    struct Opaque;
    std::unique_ptr<struct Opaque> o_;

    friend Usb;
};

class Usb
{
    Usb();

    auto dumpDeviceList() -> void;

    auto registerListener(int32_t vendorId, int32_t productId, int32_t deviceClass,
                         std::function<void(const UsbHandle&)>) -> UsbCallback;

    auto unregisterListener(UsbCallback) -> void;

    auto openDevice(const UsbDevice&) -> std::unique_ptr<UsbHandle>;
private:
    struct Opaque;
    std::unique_ptr<struct Opaque> o_;
};
}
#endif // USB_H_
