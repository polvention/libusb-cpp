
set(LIBUSB_SRC_DIR ${LIBUSB_DIR}/libusb)
set(LIBUSB_OS_SRC_DIR ${LIBUSB_SRC_DIR}/os)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include(CTest)

set(src
    ${LIBUSB_SRC_DIR}/core.c
    ${LIBUSB_SRC_DIR}/descriptor.c
    ${LIBUSB_SRC_DIR}/hotplug.c
    ${LIBUSB_SRC_DIR}/io.c
    ${LIBUSB_SRC_DIR}/sync.c
    ${LIBUSB_SRC_DIR}/strerror.c
    )

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(src ${src}
    ${LIBUSB_OS_SRC_DIR}/linux_usbfs.c
    ${LIBUSB_OS_SRC_DIR}/poll_posix.c
    ${LIBUSB_OS_SRC_DIR}/threads_posix.c
    ${LIBUSB_OS_SRC_DIR}/linux_netlink.c
    )

set(LIBUSB_SYSTEM_INC_DIR ${LIBUSB_DIR}/linux)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(src ${src}
    ${LIBUSB_OS_SRC_DIR}/darwin_usb.c
    ${LIBUSB_OS_SRC_DIR}/poll_posix.c
    ${LIBUSB_OS_SRC_DIR}/threads_posix.c
    )

set(LIBUSB_SYSTEM_INC_DIR ${LIBUSB_DIR}/darwin)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(src ${src}
    ${LIBUSB_OS_SRC_DIR}/poll_windows.c
    ${LIBUSB_OS_SRC_DIR}/threads_windows.c
    ${LIBUSB_OS_SRC_DIR}/windows_nt_common.c
    ${LIBUSB_OS_SRC_DIR}/windows_winusb.c
    )

set(LIBUSB_SYSTEM_INC_DIR ${LIBUSB_DIR}/windows)

endif()

add_library(libusb usb.cpp ${src})

target_include_directories(libusb
    PUBLIC
    ${LIBUSB_DIR}
    ${LIBUSB_SRC_DIR}
    ${LIBUSB_OS_SRC_DIR}
    ${LIBUSB_SYSTEM_INC_DIR}
    )

if (BUILD_TESTING)
add_subdirectory(tests)
endif ()
