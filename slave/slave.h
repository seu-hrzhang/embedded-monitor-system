#ifndef SLAVE_H
#define SLAVE_H

#include "comm.h"
#include "monitor.h"
#include "util.h"

#define VISITOR 0
#define USER 1

QT_BEGIN_NAMESPACE
namespace Ui {
class Slave;
}
QT_END_NAMESPACE

class CurvePlot : public QwtPlot {
  Q_OBJECT

private:
  QwtPlotCanvas *canvas;
  QwtPlotCurve *curve;
  QVector<double> coord_x, coord_y;

public:
  explicit CurvePlot(QWidget *parent = NULL);
  void refresh(QVector<ResistData>);
};

class Slave : public QMainWindow {
  Q_OBJECT

public:
  Slave(QWidget *parent = NULL);
  ~Slave();

private:
  Ui::Slave *ui;

  QWidget *widget;     // Central widget
  QGridLayout *layout; // Layout of central widget

  // Tabs
  QTabWidget *tabs;

  // System info panel elems
  QWidget *panel;
  QGridLayout *layout_panel;
  QLabel *lb_buf_0;
  QLabel *lb_info, *lb_buf_1;
  QLabel *lb_entry, *lb_value, *lb_state, *lb_warn;
  QLabel *lb_host, *lb_addr, *lb_net_state, *lb_cnct;
  QLabel *lb_cycle;
  QComboBox *cb_cycle;
  int current_cycle;
  QPushButton *btn_apply;
  QLabel *lb_buf_2;
  QTableWidget *table;

  // Login bar elems
  QWidget *login;
  QGridLayout *layout_login;
  QLabel *lb_buf_3, *lb_buf_4, *lb_buf_5;
  QLabel *lb_id, *lb_pwd;
  QLineEdit *le_id, *le_pwd;
  QPushButton *btn_login;

  // Gallery bar elems
  QWidget *gallery;
  QGridLayout *layout_gallery;
  QLabel *lb_image;
  QPushButton *btn_prev, *btn_next, *btn_folder;
  QLabel *lb_buf_6;

  // Control panel elems
  QWidget *ctrl;
  QGridLayout *layout_ctrl;
  QLabel *lb_logout, *lb_net, *lb_cam, *lb_sys;
  QPushButton *btn_logout, *btn_net, *btn_cam, *btn_sys;
  QLabel *lb_buf_7;

  // Line chart elems
  QWidget *visual;
  CurvePlot *cv_plot;

  // System user
  User *usr;

  // Data sequence
  QVector<ResistData> data;

  // Image sequence
  int img_id;
  QVector<QPixmap> imgs;
  QPixmap img_rsv; // Reserver image for comparison

  // Communication module
  Comm *comm;
  CommPack *pack;

  // Monitor module
  Monitor *monitor;

  // Timers
  QTimer *tm_sample; // Timer used to sample data from ADC

  // Initialize components
  void init_widgets();
  void init_connection();
  void init_table();
  void init_image();

  void add_image(QPixmap img); // Add new image to image sequence

  bool sys_state; // true = running, false = stopped
  bool run();     // Run system
  void stop();    // Stop system

  // Change UI when user sign in or out
  // opt: USER / VISITOR
  void change_ui(int opt);

  // Reset components to empty
  void reset();

private slots:
  void folder();              // Open image folder
  void show_prev_image();     // Show previous image
  void show_next_image();     // Show next image
  void usr_login();           // Check login info & create user
  void usr_logout();          // Delete user info & change ui
  void alter_state();         // Alter system state
  void alter_cam();           // Alter state of cam
  void alter_net();           // Change status of network connection
  void apply_cycle();         // Apply newly changed cycle
  void send_cycle();          // Send cycle info to host computer
  void proc_msg();            // Process new message
  void proc_data();           // Process new data
  void proc_image();          // Process new image
  void client_connected();    // Client connected to server
  void client_disconnected(); // Client disconnected from server
  void client_disabled();     // Client disabled
};
#endif // SLAVE_H
