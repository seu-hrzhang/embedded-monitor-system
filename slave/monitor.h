#ifndef MONITOR_H
#define MONITOR_H

#include "camera.h"
#include "util.h"

#define ADC_ADDR "/dev/adc"
#define CAM_ADDR "/dev/video0"

class Monitor : public QObject {
    Q_OBJECT
  public:
    int fd_adc;
    char buf[512];

    unsigned char *buf_rgb, *buf_yuv;

    Camera *cam; // Camera object

    int resist;  // Sampled resistance data
    QPixmap img; // Filmed image

    Monitor();

    bool init_adc(); // Initialize ADC

    int get_resist();    // Get resistance value
    QPixmap get_image(); // Film image from camera

    // Customized signals for Comm Class
  signals:
    void adc_init_succeed(); // Successfully initialized ADC
    void adc_init_error();   // Error initializing ADC
    void sampled();          // Data sampling finished
    void sample_error();     // Error sampling data
    void filmed();           // Filming finished
    void film_error();       // Error filming

  private slots:
    void sample(); // Read data from ADC
    void film();   // Film image from camera
};

#endif // MONITOR_H
