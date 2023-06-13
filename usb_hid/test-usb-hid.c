/*

  Copyright (c) 2014 Guillaume Duc <guillaume@guiduc.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


typedef struct{
    uint8_t            frameId;  
    uint8_t            volume;
    uint8_t            buttons;
    uint8_t            buzzer;
}usb_hid_in_report_s ;

typedef struct{
    uint8_t            frameId;
    uint8_t            volume;
    uint8_t            buttons;
    uint8_t            buzzer;
} usb_hid_out_report_s;

#define USB_HID_IN_REPORT_SIZE  sizeof(usb_hid_in_report_s)
#define USB_HID_OUT_REPORT_SIZE sizeof(usb_hid_out_report_s)


static uint8_t usb_hid_in_report_buf[USB_HID_IN_REPORT_SIZE];
/* +1 for the report index */
static uint8_t usb_hid_out_report_buf[USB_HID_OUT_REPORT_SIZE + 1];

static usb_hid_in_report_s* usb_hid_in_report = (usb_hid_in_report_s*)usb_hid_in_report_buf;

static usb_hid_out_report_s* usb_hid_out_report = (usb_hid_out_report_s*)(&usb_hid_out_report_buf[1]);

static int     usb_hid_fd;
static uint8_t wkup_pb_old_value = 0;

static void read_in_report()
{
  int res, i;

  printf("read()=%d\n",sizeof(usb_hid_in_report_s));
  res = read(usb_hid_fd, usb_hid_in_report_buf, USB_HID_IN_REPORT_SIZE);
  if (res < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  } else {
    printf("Volume,Buttons,buzzer:%d\t%d\t%x\t%d\n", usb_hid_in_report->frameId, usb_hid_in_report->volume,
                                                     usb_hid_in_report->buttons,usb_hid_in_report->buzzer);
    printf("\n");
  }
}

static void send_out_report()
{
  int res;

  usb_hid_out_report_buf[0] = 0;
  printf("Set buzzer=%d\n", usb_hid_out_report->buzzer);

  res = write(usb_hid_fd, usb_hid_out_report_buf, USB_HID_OUT_REPORT_SIZE + 1);
  if (res < 0) {
    perror("*write*");
    //exit(EXIT_FAILURE);
  }
}

static void usb_hid_init(const char* dev_name)
{
  int  i, res;
  int  desc_size = 0;
  char buf[256];

  struct hidraw_report_descriptor rpt_desc;
  struct hidraw_devinfo           info;

  usb_hid_fd = open(dev_name, O_RDWR);

  if (usb_hid_fd < 0) {
    perror("Unable to open device");
    exit(EXIT_FAILURE);
  }

  memset(&rpt_desc, 0x0, sizeof(rpt_desc));
  memset(&info, 0x0, sizeof(info));
  memset(buf, 0x0, sizeof(buf));

  /* Get Report Descriptor Size */
  res = ioctl(usb_hid_fd, HIDIOCGRDESCSIZE, &desc_size);
  if (res < 0)
    perror("HIDIOCGRDESCSIZE");
  else
    printf("Report Descriptor Size: %d\n", desc_size);

  /* Get Report Descriptor */
  rpt_desc.size = desc_size;
  res           = ioctl(usb_hid_fd, HIDIOCGRDESC, &rpt_desc);
  if (res < 0) {
    perror("HIDIOCGRDESC");
  } else {
    printf("Report Descriptor:\n");
    for (i = 0; i < rpt_desc.size; i++)
      printf("%02hhx ", rpt_desc.value[i]);
    puts("\n");
  }
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "Usage: %s /dev/hidrawX\n", argv[0]);
    return EXIT_FAILURE;
  }

  memset(usb_hid_out_report_buf, 0, sizeof(usb_hid_out_report_buf));
 
  usb_hid_init(argv[1]);
  usb_hid_out_report->volume = 4;
  usb_hid_out_report->frameId = 20;
  usb_hid_out_report->buzzer = 1;

  uint8_t            buzzer = usb_hid_out_report->buzzer;
  send_out_report();

  while (1) {
      read_in_report();
      usb_hid_out_report->buzzer = usb_hid_in_report->volume >= 40?1:0 ;
      if ( usb_hid_in_report->frameId >=30 ){
          usb_hid_out_report->volume = 2;
          usb_hid_out_report->frameId = 2;
          usb_hid_out_report->buzzer = 1;
          send_out_report();
      }
      buzzer = usb_hid_out_report->buzzer;
  }

  close(usb_hid_fd);

  return EXIT_SUCCESS;
}