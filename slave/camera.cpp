// This file is part of the experiment demo
// Its contents remain unchanged

#include "camera.h"

int c_camera_type = 0;

Camera::Camera(const char *DEV_NAME, int w, int h, int camera_type) {
  memcpy(dev_name, DEV_NAME, strlen(DEV_NAME));
  io = IO_METHOD_MMAP; // IO_METHOD_READ;//IO_METHOD_MMAP;
  cap_image_size = 0;
  width = w;
  height = h;
  isOpen = false;

  if (1 == camera_type) {
    c_camera_type = 1;
  }
}

Camera::~Camera() {}

int convert_yuv_to_rgb_pixel(int y, int u, int v) {
  unsigned int pixel32 = 0;
  unsigned char *pixel = (unsigned char *)&pixel32;
  int r, g, b;
  r = y + (1.370705 * (v - 128));
  g = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
  b = y + (1.732446 * (u - 128));
  if (r > 255)
    r = 255;
  if (g > 255)
    g = 255;
  if (b > 255)
    b = 255;
  if (r < 0)
    r = 0;
  if (g < 0)
    g = 0;
  if (b < 0)
    b = 0;
  pixel[0] = r * 220 / 256;
  pixel[1] = g * 220 / 256;
  pixel[2] = b * 220 / 256;
  return pixel32;
}

bool Camera::process_image(unsigned char *yuv, unsigned char *rgb) {
  unsigned int in, out = 0;
  unsigned int pixel_16;
  unsigned char pixel_24[3];
  unsigned int pixel32;
  int y0, u, y1, v;
  for (in = 0; in < width * height * 2; in += 4) {
    pixel_16 =
        yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] << 8 | yuv[in + 0];
    y0 = (pixel_16 & 0x000000ff);
    u = (pixel_16 & 0x0000ff00) >> 8;
    y1 = (pixel_16 & 0x00ff0000) >> 16;
    v = (pixel_16 & 0xff000000) >> 24;
    pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
    pixel_24[0] = (pixel32 & 0x000000ff);
    pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
    pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
    rgb[out++] = pixel_24[0];
    rgb[out++] = pixel_24[1];
    rgb[out++] = pixel_24[2];
    pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
    pixel_24[0] = (pixel32 & 0x000000ff);
    pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
    pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
    rgb[out++] = pixel_24[0];
    rgb[out++] = pixel_24[1];
    rgb[out++] = pixel_24[2];
  }
  return true;
}

unsigned int Camera::getImageSize() { return cap_image_size; }

void Camera::CloseDevice() {
  stop_capturing();
  uninit_device();
  close_device();
  isOpen = false;
}

void Camera::errno_exit(const char *s) {
  // fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
  exit(EXIT_FAILURE);
}
int Camera::xioctl(int fd, int request, void *arg) {
  int r;
  do
    r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);
  return r;
}

int Camera::read_frame(unsigned char *image) {
  struct v4l2_buffer buf;

  CLEAR(buf);
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
    switch (errno) {
    case EAGAIN:
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      return 0;
    case EIO:
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      /* Could ignore EIO, see spec. */
      /* fall through */
    default:
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      errno_exit("VIDIOC_DQBUF");
    }
  }
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  assert(buf.index < n_buffers);
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  memcpy(image, buffers[0].start, cap_image_size);
  if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    errno_exit("VIDIOC_QBUF");

  return 1;
}
void Camera::stop_capturing(void) {
  enum v4l2_buf_type type;
  switch (io) {
  case IO_METHOD_READ:
    /* Nothing to do. */
    break;
  case IO_METHOD_MMAP:
  case IO_METHOD_USERPTR:
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
      errno_exit("VIDIOC_STREAMOFF");
    break;
  }
}
bool Camera::start_capturing(void) {
  unsigned int i;
  enum v4l2_buf_type type;
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  for (i = 0; i < n_buffers; ++i) {
    struct v4l2_buffer buf;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      return false;
    }
  }
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    return false;
  }
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  return true;
}
void Camera::uninit_device(void) {
  unsigned int i;
  switch (io) {
  case IO_METHOD_READ:
    free(buffers[0].start);
    break;
  case IO_METHOD_MMAP:
    for (i = 0; i < n_buffers; ++i)
      if (-1 == munmap(buffers[i].start, buffers[i].length))
        errno_exit("munmap");
    break;
  case IO_METHOD_USERPTR:
    for (i = 0; i < n_buffers; ++i)
      free(buffers[i].start);
    break;
  }
  free(buffers);
}

bool Camera::init_mmap(void) {
  struct v4l2_requestbuffers req;
  CLEAR(req);
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      // fprintf(stderr, "%s does not support memory mapping\n", dev_name);
      return false;
    } else {
      return false;
    }
  }
  if (req.count < 2) {
    // fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
    return false;
  }
  buffers = (buffer *)calloc(req.count, sizeof(*buffers));
  if (!buffers) {
    // fprintf(stderr, "Out of memory\n");
    return false;
  }
  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
    struct v4l2_buffer buf;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n_buffers;
    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
      errno_exit("VIDIOC_QUERYBUF");
    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
        mmap(NULL /* start anywhere */, buf.length,
             PROT_READ | PROT_WRITE /* required */,
             MAP_SHARED /* recommended */, fd, buf.m.offset);
    if (MAP_FAILED == buffers[n_buffers].start)
      return false;
  }
  return true;
}

bool Camera::init_device(void) {
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;
  if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    if (EINVAL == errno) {
      // fprintf(stderr, "%s is no V4L2 device\n", dev_name);
      return false;
    } else {
      return false;
    }
  }
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    // fprintf(stderr, "%s is no video capture device\n", dev_name);
    return false;
  }
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  switch (io) {
  case IO_METHOD_READ:
    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      // fprintf(stderr, "%s does not support read i/o\n", dev_name);
      return false;
    }
    break;
  case IO_METHOD_MMAP:
  case IO_METHOD_USERPTR:
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      // fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
      return false;
    }
    break;
  }

  v4l2_input input;
  memset(&input, 0, sizeof(struct v4l2_input));
  input.index = 0;
  int rtn = ioctl(fd, VIDIOC_S_INPUT, &input);
  if (rtn < 0) {
    // printf("VIDIOC_S_INPUT:rtn(%d)\n", rtn);
    return false;
  }
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  // printf("-#-#-#-#-#-#-#-#-#-#-#-#-#-\n");
  // printf("\n");
  ////////////crop finished!
  //////////set the format
  CLEAR(fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  // V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_YUV420 �� Planar formats with 1/2
  // horizontal and vertical chroma resolution, also known as YUV 4:2:0
  // V4L2_PIX_FMT_YUYV �� Packed format with 1/2 horizontal chroma resolution,
  // also known as YUV 4:2:2
  fmt.fmt.pix.pixelformat =
      V4L2_PIX_FMT_YUYV; // V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YUV420;//V4L2_PIX_FMT_YUYV;
                         // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  // fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
  {
    // printf("-#-#-#-#-#-#-#-#-#-#-#-#-#-\n");
    // printf("=====will set fmt to (%d, %d)--", fmt.fmt.pix.width,
    // fmt.fmt.pix.height);
    if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
      // printf("V4L2_PIX_FMT_YUYV\n");
    } else if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420) {
      // printf("V4L2_PIX_FMT_YUV420\n");
    } else if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_NV12) {
      // printf("V4L2_PIX_FMT_NV12\n");
    }
  }
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);

  // printf("************** %s, line = %d(camera_type = %d)\n", __FUNCTION__,
  // __LINE__, c_camera_type);

  if (c_camera_type == 0) {
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
      // printf("************** %s, line = %d\n", __FUNCTION__,
      // __LINE__);
      return false;
    }
  }

#if 1
  if (c_camera_type == 0) {
    fmt.type = V4L2_BUF_TYPE_PRIVATE;
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
      // printf("************** %s, line = %d\n", __FUNCTION__,
      // __LINE__);
      return false;
    }
  }
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#endif
  // CLEAR (fmt);
#if 1
  if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt)) {
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    return false;
  }
#endif
  // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
  // printf("=====after set fmt\n");
  // printf("    fmt.fmt.pix.width = %d\n", fmt.fmt.pix.width);
  // printf("    fmt.fmt.pix.height = %d\n", fmt.fmt.pix.height);
  // printf("    fmt.fmt.pix.sizeimage = %d\n", fmt.fmt.pix.sizeimage);
  cap_image_size = fmt.fmt.pix.sizeimage;
  // printf("    fmt.fmt.pix.bytesperline = %d\n",
  // fmt.fmt.pix.bytesperline); printf("-#-#-#-#-#-#-#-#-#-#-#-#-#-\n");
  // printf("\n");

  /* Note VIDIOC_S_FMT may change width and height. */
  // printf("-#-#-#-#-#-#-#-#-#-#-#-#-#-\n");
  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
  min = (unsigned int)width * height * 3 / 2;
  // printf("min:%d\n", min);
  if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;
  cap_image_size = fmt.fmt.pix.sizeimage;
  // printf("After Buggy driver paranoia\n");
  // printf("    >>fmt.fmt.pix.sizeimage = %d\n", fmt.fmt.pix.sizeimage);
  // printf("    >>fmt.fmt.pix.bytesperline = %d\n",
  // fmt.fmt.pix.bytesperline); printf("-#-#-#-#-#-#-#-#-#-#-#-#-#-\n");
  // printf("\n");

  init_mmap();

  return true;
}
void Camera::close_device(void) {
  if (-1 == close(fd))
    errno_exit("close");
  fd = -1;
}
bool Camera::open_device(void) {
  struct stat st;
  if (-1 == stat(dev_name, &st)) {
    // fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno,
    // strerror(errno));
    return false;
  }
  if (!S_ISCHR(st.st_mode)) {
    // fprintf(stderr, "%s is no device\n", dev_name);
    return false;
  }
  fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
  if (-1 == fd) {
    // fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno,
    // strerror(errno));
    return false;
  }
  return true;
}

bool Camera::OpenDevice() {
  if (open_device()) {
    // printf("open success\n");
    if (init_device()) {
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
      if (start_capturing())
        // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
        isOpen = true;
      return true;
    } else {
      // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    }
  } else
    // printf("open failed\n");

    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    return false;
}

bool Camera::GetBuffer(unsigned char *image) {
  fd_set fds;
  struct timeval tv;
  int r;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  /* Timeout. */
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  r = select(fd + 1, &fds, NULL, NULL, &tv);
  if (-1 == r) {
    errno_exit("select");
  }
  if (0 == r) {
    // printf("************** %s, line = %d\n", __FUNCTION__, __LINE__);
    // fprintf(stderr, "select timeout\n");
    exit(EXIT_FAILURE);
  }
  read_frame(image);
}

#define TUNE(r) (r < 0 ? 0 : (r > 255 ? 255 : r))

static int RGB_Y[256];
static int RGBR_V[256];
static int RGBG_U[256];
static int RGBG_V[256];
static int RGBB_U[256];

static int YUVY_R[256];
static int YUVY_G[256];
static int YUVY_B[256];

static int YUVU_R[256];
static int YUVU_G[256];
static int YUVU_B[256];

static int YUVV_R[256];
static int YUVV_G[256];
static int YUVV_B[256];

static int coff_rv[256];
static int coff_gu[256];
static int coff_gv[256];
static int coff_bu[256];

void InitTable() {
  int i;
  for (i = 0; i < 256; i++) {
    RGB_Y[i] = 298 * (i - 16);
    RGBR_V[i] = 408 * (i - 128);
    RGBG_U[i] = 100 * (128 - i);
    RGBG_V[i] = 208 * (128 - i);
    RGBB_U[i] = 517 * (i - 128);

    YUVY_R[i] = 66 * i;
    YUVY_G[i] = 129 * i;
    YUVY_B[i] = 25 * i;
    YUVU_R[i] = -38 * i;
    YUVU_G[i] = -74 * i;
    YUVU_B[i] = 112 * i;
    YUVV_R[i] = 112 * i;
    YUVV_G[i] = -94 * i;
    YUVV_B[i] = -18 * i;

    coff_rv[i] = (8 + 1.402 * (i - 128)) * 256;
    coff_gu[i] = -0.34413 * (i - 128) * 256;
    coff_gv[i] = -0.71414 * (i - 128) * 256;
    coff_bu[i] = 1.772 * (i - 128) * 256;
  }
}

void YUV2RGB422(unsigned char *pRGB, unsigned char *pYUV, int size) {
  unsigned char y, u, v, y1;
  int r, g, b;
  unsigned int i = 0;
  unsigned int loop = size >> 1;
  while (loop-- > 0) {

    y = *pYUV;
    pYUV++;
    u = *pYUV;
    pYUV++;
    y1 = *pYUV;
    pYUV++;
    v = *pYUV;
    pYUV++;

    r = 1.164 * (y - 16) + 1.596 * (v - 128);
    g = 1.164 * (y - 16) - 0.813 * (v - 128) - 0.392 * (u - 128);
    b = 1.164 * (y - 16) + 2.017 * (u - 128);

    *pRGB = TUNE(r);
    pRGB++;
    *pRGB = TUNE(g);
    pRGB++;
    *pRGB = TUNE(b);
    pRGB++;

    r = 1.164 * (y1 - 16) + 1.596 * (v - 128);
    g = 1.164 * (y1 - 16) - 0.813 * (v - 128) - 0.392 * (u - 128);
    b = 1.164 * (y1 - 16) + 2.017 * (u - 128);
    *pRGB = TUNE(r);
    pRGB++;
    *pRGB = TUNE(g);
    pRGB++;
    *pRGB = TUNE(b);
    pRGB++;
  }
}

void RGB2YUV422(unsigned char *pRGB, unsigned char *pYUV, int size) {
  unsigned char r, g, b, u, v, u1, v1, r1, g1, b1;
  // unsigned char *YUVBuff;
  // unsigned char *p;
  // p = YUVBuff;
  int loop = size / 2;
  int i;
  for (i = 0; i < loop; i++) {
    r = *pRGB;
    pRGB++;
    g = *pRGB;
    pRGB++;
    b = *pRGB;
    pRGB++;
    r1 = *pRGB;
    pRGB++;
    g1 = *pRGB;
    pRGB++;
    b1 = *pRGB;
    pRGB++;

    int y = ((YUVY_R[r] + YUVY_G[g] + YUVY_B[b] + 128) >> 8) + 16;
    u = ((YUVU_R[r] + YUVU_G[g] + YUVU_B[b] + 128) >> 8) + 128;
    v = ((YUVV_R[r] + YUVV_G[g] + YUVV_B[b] + 128) >> 8) + 128;
    int y1 = ((YUVY_R[r1] + YUVY_G[g1] + YUVY_B[b1] + 128) >> 8) + 16;
    u1 = ((YUVU_R[r1] + YUVU_G[g1] + YUVU_B[b1] + 128) >> 8) + 128;
    v1 = ((YUVV_R[r1] + YUVV_G[g1] + YUVV_B[b1] + 128) >> 8) + 128;

    *pYUV++ = TUNE(y);
    *pYUV++ = (TUNE(u) + TUNE(u1)) >> 1;
    *pYUV++ = TUNE(y1);
    *pYUV++ = TUNE(v);
  }
}

void inline Yuv2RgbPixel(unsigned char y, unsigned char u, unsigned char v,
                         unsigned char *rgbPixel) {
  int r = (RGB_Y[y] + RGBR_V[v] + 128) >> 8;
  int g = ((RGB_Y[y] + RGBG_V[v] + RGBG_U[u] + 128) >> 8);
  int b = ((RGB_Y[y] + RGBB_U[u] + 128) >> 8);
  *rgbPixel = TUNE(r);
  rgbPixel++;
  *rgbPixel = TUNE(g);
  rgbPixel++;
  *rgbPixel = TUNE(b);
}

void YUV2RGB(unsigned char *pRGB, unsigned char *pYUV, int size) // 444
{
  unsigned char y, u, v;

  for (int i = 0; i < size; i++) {
    y = *pYUV;
    pYUV++;
    u = *pYUV;
    pYUV++;
    v = *pYUV;
    pYUV++;

    Yuv2RgbPixel(y, u, v, pRGB);
    pRGB += 3;
  }
}

void inline Rgb2YuvPiexl(unsigned char r, unsigned char g, unsigned char b,
                         unsigned char *pYUV) {
  int y = ((YUVY_R[r] + YUVY_G[g] + YUVY_B[b] + 128) >> 8) + 16;
  int u = ((YUVU_R[r] + YUVU_G[g] + YUVU_B[b] + 128) >> 8) + 128;
  int v = ((YUVV_R[r] + YUVV_G[g] + YUVV_B[b] + 128) >> 8) + 128;
  *pYUV = TUNE(y);
  pYUV++;
  // *pYUV = u < 0 ? 0 : (u > 255 ? 255 : u);
  // pYUV++;
  *pYUV = TUNE(u);
  pYUV++;
  *pYUV = TUNE(v);
}

void RGB2YUV(unsigned char *pRGB, unsigned char *pYUV, int size) {
  unsigned char r, g, b, y, u, v;
  for (int i = 0; i < size; i++) {
    r = *pRGB;
    pRGB++;
    g = *pRGB;
    pRGB++;
    b = *pRGB;
    pRGB++;
    Rgb2YuvPiexl(r, g, b, pYUV);
    pYUV += 3;
  }
}
