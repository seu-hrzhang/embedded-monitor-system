// This file is part of the experiment demo
// Its contents remain unchanged

#ifndef CAMERA_H
#define CAMERA_H

#include <asm/types.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define CAM_WIDTH 640
#define CAM_HEIGHT 480

#define CLEAR(x) memset(&(x), 0, sizeof(x))
typedef enum {
  IO_METHOD_READ,
  IO_METHOD_MMAP,
  IO_METHOD_USERPTR,
} io_method;
struct buffer {
  void *start;
  size_t length;
};

int convert_yuv_to_rgb_pixel(int y, int u, int v);

class Camera {
public:
  Camera(const char *DEV_NAME, int w, int h, int camer_type);
  ~Camera();
  bool OpenDevice();
  bool isOpen;
  void CloseDevice();
  bool GetBuffer(unsigned char *image);
  unsigned int getImageSize();
  bool process_image(unsigned char *yuv, unsigned char *rgb);
  char dev_name[50];
  io_method io;
  int fd;
  int width;
  int height;
  struct buffer *buffers;
  unsigned int n_buffers;
  unsigned int cap_image_size;

private:
  bool init_device(void);
  // void init_read(unsigned int buffer_size);
  bool init_mmap(void);
  // void init_userp(unsigned int buffer_size);
  void uninit_device(void);
  bool start_capturing(void);
  void stop_capturing(void);
  void mainloop(unsigned char *image);
  int read_frame(unsigned char *image);
  void close_device(void);
  bool open_device(void);

  void errno_exit(const char *s);
  int xioctl(int fd, int request, void *arg);
};

void InitTable();
void YUV2RGB422(unsigned char *pRGB, unsigned char *pYUV, int size);
void RGB2YUV422(unsigned char *pRGB, unsigned char *pYUV, int size);
void inline Yuv2RgbPixel(unsigned char y, unsigned char u, unsigned char v,
                         unsigned char *rgbPixel);
void YUV2RGB(unsigned char *pRGB, unsigned char *pYUV, int size);
void inline Rgb2YuvPiexl(unsigned char r, unsigned char g, unsigned char b,
                         unsigned char *pYUV);
void RGB2YUV(unsigned char *pRGB, unsigned char *pYUV, int size);

#endif // CAMERA_H
