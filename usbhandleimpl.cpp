#include "usbhandleimpl.h"

namespace
{
using namespace plv::usb;
}

UsbHandleImpl::UsbHandleImpl(libusb_device *dev)
{
}

auto UsbHandleImpl::open() -> void
{
    libusb_open(dev, handle);
}

auto UsbHandleImpl::close() -> void
{
}
