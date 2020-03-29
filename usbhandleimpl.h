#ifndef PLV_USB_USBHANDLEIMPL_H_
#define PLV_USB_USBHANDLEIMPL_H_

#include "usbhandle.h"

namespace plv::usb
{

class UsbHandleImpl: UsbHandle
{
    auto open() -> void;
    auto close() -> void;
};
} // plv::usb

#endif // PLV_USB_USBHANDLEIMPL_H
