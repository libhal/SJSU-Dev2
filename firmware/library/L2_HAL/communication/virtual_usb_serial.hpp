#pragma once

#include <cstdint>

// https://stratifylabs.co/embedded%20design%20tips/2013/10/22/Tips-USB-Virtual-Serial-Port-Firmware/

// This is the standard device descriptor
[[gnu::packed]] struct UsbDeviceDescription_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcdUSB;
  uint8_t device_class;
  uint8_t device_subclass;
  uint8_t device_protocol;
  uint8_t max_packet_size;
  uint16_t id_vendor;
  uint16_t id_product;
  uint16_t bcdDevice;
  uint8_t manufacturer;
  uint8_t product;
  uint8_t serial_number;
  uint8_t num_configurations;
};

// definition to aid with declaring USB strings
#define usb_declare_string(len) \
  struct HWPL_PACK              \
  {                             \
    uint8_t length;             \
    uint8_t descriptor_type;    \
    uint16_t string[len];       \
  }

// definition to aid with assigning values to a USB string
#define usb_assign_string(len, ...)                                       \
  {                                                                       \
    .length = len * 2 + 2, .descriptor_type = USB_STRING_DESCRIPTOR_TYPE, \
    .string = {                                                           \
      __VA_ARGS__                                                         \
    }                                                                     \
  }

// The standard configuration descriptor
[[gnu::packed]] struct UsbConfigDescription_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t total_length;
  uint8_t num_interfaces;
  uint8_t configuration_value;
  uint8_t configuration;
  uint8_t attributes;
  uint8_t max_power;
};

// The standard interface descriptor
[[gnu::packed]] struct UsbInterfaceDescription_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint8_t interface_number;
  uint8_t alternate_setting;
  uint8_t num_endpoints;
  uint8_t interface_class;
  uint8_t interface_subclass;
  uint8_t interface_protocol;
  uint8_t interface;
};

// The standard endpoint descriptor
[[gnu::packed]] struct UsbEndpointDescriptor_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint8_t endpoint_address;
  uint8_t attributes;
  uint16_t max_packet_size;
  uint8_t interval;
};

// The CDC functional header
[[gnu::packed]] struct CdcFunctionalHeaderDescriptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint16_t bcdCDC;
};

// The CDC call management descriptor
[[gnu::packed]] struct CdcCallManagementDesciptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint8_t capabilities;
  uint8_t data_interface;
};

// The CDC ACM descriptor
[[gnu::packed]] struct CdcAbstractControlModelFunctionDescriptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint8_t capabilities;
};

// The CDC union descriptor
[[gnu::packed]] struct CdcUnionFunctionDescriptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint8_t master_interface;
  uint8_t slave_interface[];
};

// CDC interface descriptor with CDC header, ACM, union, and call management
typedef struct HWPL_PACK
{
  usb_dev_cdc_header_t header;
  usb_dev_cdc_acm_t acm;
  usb_dev_cdc_uniondescriptor_t union_descriptor;
  CdcCallManagementDesciptor_t call_management;
} cdc_acm_interface_t;

// Application level configuration descriptor with interfaces and endpoints
typedef struct HWPL_PACK
{
  UsbConfigDescription_t cfg /* The configuration descriptor */;
  UsbInterfaceDescription_t ifcontrol /* The interface descriptor */;
  link_cdc_acm_interface_t acm /*! The CDC ACM Class descriptor */;
  UsbEndpointDescriptor_t control /* Endpoint:  Interrupt out for control packets */;
  UsbInterfaceDescription_t ifdata /* The interface descriptor */;
  UsbEndpointDescriptor_t data_out /* Endpoint:  Bulk out */;
  UsbEndpointDescriptor_t data_in /* Endpoint:  Bulk in */;
  uint8_t terminator /* A null terminator used by the driver (required) */;
} app_cfg_desc_t;

#define USB_DESC_MANUFACTURER_SIZE 15
#define USB_DESC_PRODUCT_SIZE 10
#define USB_DESC_SERIAL_SIZE 16
#define USB_DESC_MANUFACTURER_STRING \
  'C', 'o', 'A', 'c', 't', 'i', 'o', 'n', 'O', 'S', ',', ' ', 'I', 'n', 'c'
#define USB_DESC_PRODUCT_STRING 'C', 'o', 'A', 'c', 't', 'i', 'o', 'n', 'O', 'S'
#define USB_DESC_SERIAL_STRING \
  '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'

// The application structure used for storing the strings
struct HWPL_PACK app_usb_string_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t wLANGID;
  usb_declare_string(USB_DESC_MANUFACTURER_SIZE) manufacturer;
  usb_declare_string(USB_DESC_PRODUCT_SIZE) product;
  usb_declare_string(USB_DESC_SERIAL_SIZE) serial;
};

const struct link_usb_string_t app_string_desc = {
  .length          = 4,
  .descriptor_type = USB_STRING_DESCRIPTOR_TYPE,
  .wLANGID         = 0x0409,  // English
  .manufacturer    = usb_assign_string(USB_DESC_MANUFACTURER_SIZE,
                                    USB_DESC_MANUFACTURER_STRING),
  .product = usb_assign_string(USB_DESC_PRODUCT_SIZE, USB_DESC_PRODUCT_STRING),
  .serial  = usb_assign_string(USB_DESC_SERIAL_SIZE, USB_DESC_SERIAL_STRING)
};

// The application device descriptor assignment
const UsbDeviceDescription_t app_dev_desc = {
  .length             = sizeof(UsbDeviceDescription_t),
  .descriptor_type    = USB_DEVICE_DESCRIPTOR_TYPE,
  .bcdUSB             = 0x0200,
  .device_class       = USB_DEVICE_CLASS_COMMUNICATIONS,
  .device_subclass    = 0,
  .device_protocol    = 0,
  .max_packet_size    = USB_MAX_PACKET0,
  .idVendor           = _vINK_USB_VID,
  .idProduct          = _pLINK_USB_PID + 10,
  .bcdDevice          = 0x0010,
  .manufacturer       = 1,
  .product            = 2,
  .serial_number      = 3,
  .num_configurations = 1
};

// The assignment of the application's USB configuration descriptor
const link_cfg_desc_t app_cfg_desc = {
  .cfg = { .length          = sizeof(UsbConfigDescription_t),
           .descriptor_type = USB_CONFIGURATION_DESCRIPTOR_TYPE,
           .total_length =
               sizeof(app_cfg_desc_t) - 1,  // exclude the zero terminator
           .num_interfaces      = 0x02,
           .configuration_value = 0x01,
           .configuration      = 0x03,
           .attributes        = USB_CONFIG_BUS_POWERED,
           .max_power           = USB_CONFIG_POWER_MA(LINK_REQD_CURRENT) },

  .ifcontrol = { .length             = sizeof(UsbInterfaceDescription_t),
                 .descriptor_type    = USB_INTERFACE_DESCRIPTOR_TYPE,
                 .interface_number   = 0x00,
                 .alternate_setting  = 0x00,
                 .num_endpoints      = 0x01,
                 .interface_class    = USB_INTERFACE_CLASS_COMMUNICATIONS,
                 .interface_subclass = USB_INTERFACE_SUBCLASS_ACM,
                 .interface_protocol = USB_INTERFACE_PROTOCOL_V25TER,
                 .interface         = 0x00 },

  .acm = { .header.length             = sizeof(usb_dev_cdc_header_t),
           .header.descriptor_type    = 0x24,
           .header.descriptor_subtype = 0x00,
           .header.bcdCDC             = 0x0110,
           .acm.function_length       = sizeof(usb_dev_cdc_acm_t),
           .acm.descriptor_type       = 0x24,
           .acm.descriptor_subtype    = 0x02,
           .acm.capabilities        = 0x02,
           .union_descriptor.function_length =
               sizeof(usb_dev_cdc_uniondescriptor_t),
           .union_descriptor.descriptor_type    = 0x24,
           .union_descriptor.descriptor_subtype = 0x06,
           .union_descriptor.master_interface   = 0x00,
           .union_descriptor.slave_interface    = 0x01,
           .call_management.function_length =
               sizeof(CdcCallManagementDesciptor_t),
           .call_management.descriptor_type    = 0x24,
           .call_management.descriptor_subtype = 0x01,
           .call_management.capabilities     = 0x00,
           .call_management.data_interface     = 0x01 },

  .control = { .length           = sizeof(UsbEndpointDescriptor_t),
               .descriptor_type  = USB_ENDPOINT_DESCRIPTOR_TYPE,
               .endpoint_address = USB_INTIN,
               .attributes     = USB_ENDPOINT_TYPE_INTERRUPT,
               .max_packet_size   = LINK_INTERRUPT_ENDPOINT_SIZE,
               .interval        = 1 },

  .ifdata = { .length             = sizeof(UsbInterfaceDescription_t),
              .descriptor_type    = USB_INTERFACE_DESCRIPTOR_TYPE,
              .interface_number   = 0x01,
              .alternate_setting  = 0x00,
              .num_endpoints      = 0x02,
              .interface_class    = USB_INTERFACE_CLASS_COMMUNICATIONS_DATA,
              .interface_subclass = 0x00,
              .interface_protocol = 0x00,
              .interface         = 0x00 },

  .data_out = { .length           = sizeof(UsbEndpointDescriptor_t),
                .descriptor_type  = USB_ENDPOINT_DESCRIPTOR_TYPE,
                .endpoint_address = USB_BULKOUT,
                .attributes     = USB_ENDPOINT_TYPE_BULK,
                .max_packet_size   = LINK_BULK_ENDPOINT_SIZE,
                .interval        = 1 },

  .data_in = { .length           = sizeof(UsbEndpointDescriptor_t),
               .descriptor_type  = USB_ENDPOINT_DESCRIPTOR_TYPE,
               .endpoint_address = USB_BULKIN,
               .attributes     = USB_ENDPOINT_TYPE_BULK,
               .max_packet_size   = LINK_BULK_ENDPOINT_SIZE,
               .interval        = 1 },

  .terminator = 0
};