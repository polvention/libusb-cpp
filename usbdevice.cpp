#include <iostream>
#include <functional>
#include <libusb.h>
#include "usb.h"

using namespace std;

struct plv::usb::UsbDevice::Opaque
{
    libusb_device *dev = nullptr;
};

auto plv::usb::Usb::getDeviceList() -> std::unique_ptr<std::vector<UsbDevice>>
{
    libusb_device **devs = nullptr;
	ssize_t cnt = libusb_get_device_list(o_->ctx_, &devs);
	if (cnt < 0) {
		throw plv::usb::UsbException("Getting devices list failed:" + std::to_string(cnt));
	}
    auto deleter = [devs, cnt](libusb_device** ptr) {
        libusb_free_device_list(ptr, cnt);
    };
    std::unique_ptr<std::vector<UsbDevice>> list(new std::vector<UsbDevice>(), deleter);
    list->reserve(cnt);
	libusb_device_descriptor desc{};
	for(ssize_t i = 0; i < cnt; i++) {
    	int r = libusb_get_device_descriptor(devs[i], &desc);
    	if (r < 0) {
		    throw plv::usb::UsbException("Getting descriptor failed:" + std::to_string(r));
        }
        auto& item = list->emplace_back(desc.idVendor, desc.idProduct, desc.bDeviceClass);
        item.o_->dev = devs[i];
    }
    return list;
}
