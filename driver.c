#include <stdio.h>
#include <stdlib.h>

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#include "hidapi.h"

#define VENDOR_ID 0x046d // logitech
#define PRODUCT_ID 0xc52b

static volatile int run = 1;

void sig_int_handle(int a) {
	run = 0;
}

void workspace_left() {
	system("osascript -e 'tell application \"System Events\" to key code 123 using control down'");
}

void workspace_right() {
	system("osascript -e 'tell application \"System Events\" to key code 124 using control down'");
}

int main(void) {
	unsigned char buf[8];

	const char *path_to_open = NULL;

	struct hid_device_info *devs;
	struct hid_device_info *cur_dev;

	signal(SIGINT, sig_int_handle);

	hid_init();

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;

	while (cur_dev) {
		if (cur_dev->vendor_id == VENDOR_ID && cur_dev->product_id == PRODUCT_ID && cur_dev->interface_number == 2) {
			path_to_open = cur_dev->path;
			break;
		}

		cur_dev = cur_dev->next;
	}

	hid_device *handle = hid_open_path(path_to_open);

	hid_free_enumeration(devs);

	while (run) {
		hid_read_timeout(handle, buf, 8, 200);

		if (buf[5] == 174) {
			workspace_left();
		} else if (buf[5] == 176) {
			workspace_right();
		}
	}

	hid_exit();

	return 0;
}
