#include "monitor.h"

Monitor::Monitor() {
  resist = 0;
  cam = new Camera(CAM_ADDR, CAM_WIDTH, CAM_HEIGHT, 0);
  buf_rgb = new unsigned char[CAM_WIDTH * CAM_HEIGHT * 3];
  buf_yuv = new unsigned char[CAM_WIDTH * CAM_HEIGHT * 2];
}

bool Monitor::init_adc() {
  if ((fd_adc = open(ADC_ADDR, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
    emit adc_init_error();
    return false;
  }
  emit adc_init_succeed();
  return true;
}

int Monitor::get_resist() {
  memset(buf, 0, sizeof(buf)); // Set buffer empty

  if (fd_adc < 0) {
    emit sample_error();
    return -1;
  }

  // Read 10 bytes from ADC, saved to buffer
  // 'len': Number of bytes actually read in
  int len = read(fd_adc, buf, 10);
  return atoi(buf) * 2.442; // Convert 'char *' to 'int'
}

QPixmap Monitor::get_image() {
  QPixmap *img = new QPixmap;
  if (cam->isOpen && cam->GetBuffer(buf_yuv)) {
    cam->process_image(buf_yuv, buf_rgb);
    QImage *photo =
        new QImage(buf_rgb, CAM_WIDTH, CAM_HEIGHT, QImage::Format_RGB888);
    *img = QPixmap::fromImage(*photo);
  }
  return *img;
}

void Monitor::sample() {
  resist = get_resist();
  emit sampled();
}

void Monitor::film() {
  if (cam->isOpen) {
    img = get_image();
    emit filmed();
  }
}
