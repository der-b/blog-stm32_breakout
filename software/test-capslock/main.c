#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/f0/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "HID_Report.h"

#define AHB_FREQUENCY 14000000

#define VENDOR_ID                 0x1209    /* see: http://pid.codes/ */
#define PRODUCT_ID                0x0001    /* test product id: http://pid.codes/1209/0001/ */
#define DEVICE_VER                0x0100    /* Program version */

/* Reference [1]: USB 3.2 Revision 1.0 */
/* Reference [2]: Device Class Definitions for Human Interface Devices (HID) Version 1.11 */
/* Reference [3]: https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/ */

/* See [1] Table 9-11 */
#define USB_VERSION_2_0 (0x0200)

/* See [1] Table 9-25 "bEndpointAddress" */
#define KEYBOARD_ADDRESS (0x81)

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];
usbd_device *g_usbd_dev = 0;

/*
 * Example from [2] Section E.6
 */
static const uint8_t hid_report[] = {
USAGE_PAGE (GENERIC_DESKTOP_CONTROLS),
USAGE (KEYBOARD),
COLLECTION (APPLICATION),
	USAGE_PAGE (KEYBOARD_KEYPAD),
	USAGE_MINIMUM (224),
	USAGE_MAXIMUM (231),
	LOGICAL_MINIMUM (0),
	LOGICAL_MAXIMUM (1),
	REPORT_SIZE (1),
	REPORT_COUNT (8),
	INPUT (DATA | VARIABLE | ABSOLUTE),

	REPORT_COUNT (1),
	REPORT_SIZE (8),
	INPUT (CONSTANT),

	REPORT_COUNT (6),
	REPORT_SIZE (8),
	LOGICAL_MINIMUM (0),
	LOGICAL_MAXIMUM (101),
	USAGE_PAGE (KEYBOARD_KEYPAD),
	USAGE_MINIMUM (0),
	USAGE_MAXIMUM (101),
	INPUT (DATA | ARRAY),

	REPORT_COUNT (5),
	REPORT_SIZE (1),
	USAGE_PAGE (LEDS),
	USAGE_MINIMUM (1),
	USAGE_MAXIMUM (7),
	OUTPUT (DATA | VARIABLE | ABSOLUTE),

	REPORT_COUNT (1),
	REPORT_SIZE (3),
	OUTPUT (CONSTANT),

END_COLLECTION, // END_COLLECTION
};


/*
 * See [2] Section 6.2.1
 */
struct tomu_hid_report {
	struct usb_hid_descriptor hid_d;
	uint8_t bDescriptorType;
	uint16_t wDescriptorLenght;
} __attribute__((packed));

/*
 * Device descriptor
 * See [1] Table 9-11
 */
static const struct usb_device_descriptor dev_desc = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = USB_VERSION_2_0,
	.bDeviceClass = 0, // we have to set it to 0 to be compliant with [2] Section 5.1 
	.bDeviceSubClass = 0, // compliant to [1] Table 9-11
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 8,
	.idVendor = VENDOR_ID,
	.idProduct = PRODUCT_ID,
	.bcdDevice = DEVICE_VER,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};


/*
 * endpoint descriptor
 * See [1] Table 9-25
 */
const struct usb_endpoint_descriptor hid_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = KEYBOARD_ADDRESS, // address 1, direction IN
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT, // interrupt, periodic poll 
#warning "You should check this"
	.wMaxPacketSize = 8,
	.bInterval = 0x20, // poll every 2 ms
};


/*
 *
 */
static const struct tomu_hid_report hid_desc ={
	.hid_d.bLength = sizeof(hid_desc),
	.hid_d.bDescriptorType = USB_DT_HID, // see [2] Section 7.1
	.hid_d.bcdHID = 0x0100, // hid revision (see [2] Section 6.2.1 and for an example Section E.4 or E.8)
	.hid_d.bCountryCode = 0,
	.hid_d.bNumDescriptors = 1,
	.bDescriptorType = USB_DT_REPORT, // see [2] Section 7.1
	.wDescriptorLenght = sizeof(hid_report),
};


/*
 * interface descriptor
 * See [1] Table 9-24
 */
const struct usb_interface_descriptor hid_interface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID, // USB_CLASS_HID=3 (see [2] section 4.1)
	.bInterfaceSubClass = 0, // no boot interface (see [2] section 4.2)
	.bInterfaceProtocol = 1, // it is a keyboard (see [2] section 4.3)
	.iInterface = 0,

	.endpoint = &hid_endpoint,
	.extra = &hid_desc,
	.extralen = sizeof(hid_desc),
};

const struct usb_interface interfaces[] = {{
	.num_altsetting = 1,
	.altsetting = &hid_interface,
}};

/*
 * USB Configuration Descriptor
 * See [1] Table 9-22
 */
const struct usb_config_descriptor config_desc = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
#warning "check this"
	.wTotalLength = sizeof(hid_interface) + sizeof(hid_desc) + sizeof(hid_report) + sizeof(hid_endpoint), 
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = USB_CONFIG_ATTR_DEFAULT | USB_CONFIG_ATTR_SELF_POWERED, // self powered
	.bMaxPower = 50, // we will draw 100 mA at max
	.interface = interfaces,
};

static const char *usb_strings[] = {
	"STM32 Breakout Board Capslock Indicator",
	"HID keyboard Demo",
	"http://der-b.com",
};



static enum usbd_request_return_codes hid_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)complete;
	(void)dev;

	if((req->bmRequestType != KEYBOARD_ADDRESS) ||
	   (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
	   (req->wValue != 0x2200))
		return 0;

	/* Handle the HID report descriptor. */
	*buf = (uint8_t *)hid_report;
	*len = sizeof(hid_report);

	return 1;
}

void usb_isr(void)
{
}


static enum usbd_request_return_codes usb_rx(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)dev;
	(void)buf;
	(void)len;
	(void)complete;

	if (   USB_REQ_SET_CONFIGURATION != req->bRequest
	    || 1 != req->wLength
	    || 0 != req->wIndex)
		return 0;

	if (0x1 << 1 & (*buf)[0]) {
		gpio_set(GPIOA, GPIO4);
	} else {
		gpio_clear(GPIOA, GPIO4);
	}
	if (0x1 << 0 & (*buf)[0]) {
		gpio_set(GPIOA, GPIO3);
	} else {
		gpio_clear(GPIOA, GPIO3);
	}
	return 1;
}

static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;
	(void)dev;

	usbd_ep_setup(dev, KEYBOARD_ADDRESS, USB_ENDPOINT_ATTR_INTERRUPT, 8, NULL);

	usbd_register_control_callback(
				dev,
				USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				hid_control_request);

	usbd_register_control_callback(
				dev,
				USB_REQ_TYPE_OUT       | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_DIRECTION | USB_REQ_TYPE_TYPE  | USB_REQ_TYPE_RECIPIENT,
				usb_rx);
}

void hard_fault_handler(void)
{
    while(1);
}

int main(void)
{
    rcc_clock_setup_in_hsi_out_48mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);

    rcc_set_usbclk_source(RCC_PLL);
    rcc_periph_clock_enable(RCC_USB);
    rcc_wait_for_osc_ready(rcc_usb_clock_source());

    /* USB initialisation */
    g_usbd_dev = usbd_init(&st_usbfs_v2_usb_driver, &dev_desc, &config_desc, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
    usbd_register_set_config_callback(g_usbd_dev, hid_set_config);

    while(1) {
	    usbd_poll(g_usbd_dev);
    }
}
