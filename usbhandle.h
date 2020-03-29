#ifndef PLV_USBHANDLE_H_
#define PLV_USBHANDLE_H_

namespace plv::usb
{

class UsbHandle
{
    enum Event
    {
        Open,
        Close,
    };
    auto open() -> void = 0;
    auto close() -> void = 0;
};
} // plv::usb

#endif // PLV_USBHANDLE_H
