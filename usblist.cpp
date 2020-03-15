#include <libusb.h>
#include <iostream>

using namespace std;

struct plv::usb::UsbList::Opaque
{
    libusb_device **list = nullptr;
};

auto plv::usb::UsbList::~UsbList() -> void
{
    libusb_free_device_lisit(o_->list, this->size());
}
