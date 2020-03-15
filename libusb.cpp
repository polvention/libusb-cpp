#include <iostream>
#include <libusb.h>
#include "usb.h"

using namespace std;

struct plv::usb::Usb::Opaque
{
	libusb_context *ctx_ = nullptr;
};

struct plv::usb::UsbDevice::Opaque
{
	libusb_context *ctx_ = nullptr;
    libusb_device *dev = nullptr;
};

plv::usb::UsbDevice::~UsbDevice()
{
    libusb_free_device_list(o_->devs,1);
}

struct plv::usb::UsbHandle::Opaque
{
    libusb_device_handle *handle = nullptr;
};


plv::usb::Usb::Usb()
{
	int ret = libusb_init(&o_->ctx_);
	if (ret < 0) {
		throw plv::usb::UsbException("Usb init failed:" + std::to_string(ret));
	}
	libusb_set_debug(o_->ctx_, 3);
}

auto plv::usb::Usb::getDeviceList() -> std::unique_ptr<std::vector<UsbDevice>>
{
    libusb_device **devs = nullptr;
	ssize_t cnt = libusb_get_device_list(o_->ctx_, &devs);
	if (cnt < 0) {
		throw plv::usb::UsbException("Getting devices list failed:" + std::to_string(cnt));
	}
    auto list = std::make_unique<std::vector<UsbDevice>, std::function<void()>>();
    list->reserve(cnt);
    list->ctx_ = this;
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

auto plv::usb::Usb::openDevice(const UsbDevice& device) -> std::unique_ptr<UsbHandle>
{
    auto handle_ptr = std::make_unique<UsbHandle>();
    ssize_t ret = libusb_open(device.o_->dev, &handle_ptr->o_->handle);
    if (ret < 0)
    {
        throw plv::usb::UsbException("Opening device failed:" + std::to_string(ret));
    }
    return handle_ptr;
}

/*
int main() {
	for(ssize_t i = 0; i < cnt; i++) {
		printdev(devs[i]);
	}
	libusb_free_device_list(devs, 1);
	libusb_exit(ctx);
	return 0;
}

void printdev(libusb_device *dev) {
	libusb_device_descriptor desc{};
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		cout << "failed to get device descriptor" << endl;
		return;
	}
	cout << "Number of possible configurations: " << desc.bNumConfigurations;
	cout << " Device Class: " << desc.bDeviceClass;
	cout << " VendorID: " << desc.idVendor;
	cout << " ProductID: "<< desc.idProduct << endl;
	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	cout << "Interfaces: " << config->bNumInterfaces << " ||| ";
	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;
	for(int i = 0; i < config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		cout << "Number of alternate settings: " << inter->num_altsetting <<" | ";
		for(int j = 0; j < inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			cout << "Interface Number: " << interdesc->bInterfaceNumber << " | ";
			cout << "Number of endpoints: " << interdesc->bNumEndpoints << " | ";
			for(int k = 0; k < interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				cout << "Descriptor Type: " << epdesc->bDescriptorType << " | ";
				cout << "EP Address: " << epdesc->bEndpointAddress << " | ";
			}
		}
	}
	cout << endl << endl << endl;
	libusb_free_config_descriptor(config);
}
*/
