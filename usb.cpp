#include <iostream>
#include <libusb.h>
#include <unordered_map>
#include "usb.h"

using plv::usb::Usb;
using plv::usb::UsbHandle;

struct plv::usb::Usb::Opaque
{
	libusb_context *ctx_ = nullptr;
    std::unordered_map<libusb_device*, std::unique_ptr<UsbHandle>> handlers_{};
};

struct plv::usb::UsbCallback::Opaque
{
    libusb_hotplug_callback_handle handle;
};

struct plv::usb::UsbHandle::Opaque
{
    libusb_device_handle *handle = nullptr;
};

struct plv::usb::UsbDevice::Opaque
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

auto plv::usb::Usb::dumpDeviceList() -> void
{
    libusb_device **devs = nullptr;
	ssize_t cnt = libusb_get_device_list(o_->ctx_, &devs);
	if (cnt < 0) {
		throw plv::usb::UsbException("Getting devices list failed:" + std::to_string(cnt));
	}
	libusb_device_descriptor desc{};
	for(ssize_t i = 0; i < cnt; i++) {
    	int r = libusb_get_device_descriptor(devs[i], &desc);
    	if (r < 0) {
		    throw plv::usb::UsbException("Getting descriptor failed:" + std::to_string(r));
        }
        std::cout << "IdVendor: " << desc.idVendor;
        std::cout << "IdProduct: " << desc.idProduct;
        std::cout << "DeviceClass: " << desc.bDeviceClass;
    }
    libusb_free_device_list(devs, 1);
}

auto plv::usb::Usb::registerListener(int32_t vendorId, int32_t productId,
                                    int32_t deviceClass,
                                    std::function<void(const std::unique_ptr<UsbHandle>&, UsbHandle::Event)> callback) -> UsbCallback
{
    UsbCallback callbackHndl{};

    auto libusbCallback = [this, callback](libusb_device *dev,
                                            UsbHandle::Event event) -> int
    {
        if (UsbHandle::Event::Open == event) {
            auto pair = o_->handlers_.emplace(dev, std::make_unique<UsbHandle>());
            if (libusb_open(dev, &pair.first->second->o_->handle) != LIBUSB_SUCCESS) {
                std::cout << "Could not open USB device\n";
            }
            callback(pair.first->second, event);
        } else if (UsbHandle::Event::Close == event) {
            if (o_->handlers_.find(dev) == o_->handlers_.end()) {
    	        libusb_device_descriptor desc{};
                int r = libusb_get_device_descriptor(dev, &desc);
                if (r < 0) {
                    std::cout << "Failed to get device descriptor\n";
	                return 1;
                }
                std::cout << "Try to close device that wasn't opened. (";
                std::cout << "Device Class: " << desc.bDeviceClass;
                std::cout << ",VendorID: " << desc.idVendor;
                std::cout << ", ProductID: "<< desc.idProduct << std::endl;
                return 1;
            }
            libusb_close(o_->handlers_[dev]->o_->handle);
            callback(o_->handlers_[dev], event);
            o_->handlers_.erase(dev);
        } else {
            std::cout << "Unhandled event: " << event << std::endl;
        }
        return 0;
    };
    int ret = libusb_hotplug_register_callback(o_->ctx_,
            static_cast<libusb_hotplug_event>(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
            LIBUSB_HOTPLUG_NO_FLAGS, vendorId, productId, deviceClass,
            [](libusb_context *ctx, libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) -> int {
            if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
                return (*static_cast<decltype(libusbCallback)*>(user_data))(dev, UsbHandle::Event::Open);
            } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
                return (*static_cast<decltype(libusbCallback)*>(user_data))(dev, UsbHandle::Event::Open);
            }
            return 1;
            }, &libusbCallback, &callbackHndl.o_->handle);
	if (ret < 0) {
		throw plv::usb::UsbException("Registering callback has failed: " + std::to_string(ret));
	}
    return callbackHndl;
}

auto plv::usb::Usb::unregisterListener(UsbCallback callback) -> void
{
    libusb_hotplug_deregister_callback(o_->ctx_, callback.o_->handle);
}

auto plv::usb::Usb::openDevice(const UsbDevice& device) -> std::unique_ptr<UsbHandle>
{
    auto handle_ptr = std::make_unique<UsbHandle>();
    /*
    ssize_t ret = libusb_open(device.o_->dev, &handle_ptr->o_->handle);
    if (ret < 0)
    {
        throw plv::usb::UsbException("Opening device failed:" + std::to_string(ret));
    }
    */
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
