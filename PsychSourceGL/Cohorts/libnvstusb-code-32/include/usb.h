#include <stdbool.h>
#include <stdint.h>

struct nvstusb_usb_device;

bool nvstusb_usb_init();
void nvstusb_usb_deinit();

struct nvstusb_usb_device *nvstusb_usb_open_device(const char *firmware);
void nvstusb_usb_close_device(struct nvstusb_usb_device *dev);

int nvstusb_usb_write_bulk(struct nvstusb_usb_device *dev, int endpoint, const void *data, int size);
int nvstusb_usb_read_bulk(struct nvstusb_usb_device *dev, int endpoint, void *data, int size);

