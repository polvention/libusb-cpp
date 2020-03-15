#ifndef USB_EXCEPT_H_
#define USB_EXCEPT_H_
#include <stdexcept>
#include <string>

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

}
#endif // USB_EXCEPT_H_
