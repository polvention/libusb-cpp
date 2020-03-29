#include <gtest/gtest.h>
#include <usb.h>

class UsbTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        usb_ = std::make_unique<plv::usb::Usb>();
    }

    void TearDown() override
    {
    }
    std::unique_ptr<plv::usb::Usb> usb_;
};

TEST_F(UsbTest, Dump)
{
    usb_->dumpDeviceList();
}
