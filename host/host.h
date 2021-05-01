#ifndef HOST_H
#define HOST_H

#include "comm.h"
#include "util.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Host;
}
QT_END_NAMESPACE

class Host : public QMainWindow {
  Q_OBJECT

public:
  Host(QWidget *parent = nullptr);
  ~Host();

private:
  Ui::Host *ui;
  QWidget *widget;     // Central widget
  QGridLayout *layout; // Layout of central widget

  // Menu bar
  QMenuBar *mbar;
  QMenu *menu_usr, *menu_view, *menu_net;
  QAction *act_usr, *act_logout;
  QAction *act_line, *act_image;
  QAction *act_net, *act_status;

  // Fonts
  QFont *ft_info;  // Info bar
  QFont *ft_text;  // Text
  QFont *ft_btext; // Text (bold)
  QFont *ft_code;  // Terminal code

  QLabel *lb_buf_0;

  // Info bar elems
  QLabel *lb_info; // User info bar label
  QLabel *lb_buf_1;

  // Panel bar elems
  QWidget *panel;
  QGridLayout *layout_panel;
  QLabel *lb_entry, *lb_value, *lb_state, *lb_warn;
  QLabel *lb_cycle;
  QLineEdit *le_cycle;
  int current_cycle;
  QPushButton *btn_apply;
  QLabel *lb_image;
  QPushButton *btn_prev, *btn_next, *btn_folder;
  QLabel *lb_buf_2;

  // Line chart elems
  QChartView *view;
  QChart *chart;
  QLineSeries *series;
  QValueAxis *axis_x, *axis_y;
  QPen *pen;

  // Data inspector elems
  QWidget *inspect;
  QGridLayout *layout_inspect;
  QTableWidget *table;
  QTextEdit *term;

  // Login bar elems
  QWidget *login;
  QGridLayout *layout_login;
  QLabel *lb_buf_3, *lb_buf_4, *lb_buf_5, *lb_buf_6;
  QLabel *lb_id, *lb_pwd;
  QLineEdit *le_id, *le_pwd;
  QPushButton *btn_login;

  // System user
  User *usr;

  // Data sequence
  QVector<ResistData> data;

  // Image sequence
  int img_id;
  QVector<QPixmap> imgs;

  // Communication module
  Comm *comm;
  CommPack *pack;

  // Initialize components
  void init_menu();
  void init_widgets();
  void init_font();

  void init_connection();

  void init_table();
  void init_image();

  // Change UI when user sign in or out
  // opt: USER / VISITOR
  void change_ui(int opt);

  // Refresh line chart display with latest data
  void refresh_chart();

  // Add new image to image sequence
  void add_image(QPixmap img);

  // Run system after connection established
  void run();

  // Stop system when disconnected
  void stop();

  // Reset components to empty
  void reset();

private slots:
  void folder();           // Open image folder
  void show_prev_image();  // Show previous image
  void show_next_image();  // Show next image
  void apply_cycle();      // Apply newly changed cycle
  void usr_login();        // Check login info & create user
  void usr_logout();       // Delete user info & change ui
  void show_line_chart();  // Switch panel to line chart
  void show_image();       // Switch panel to image
  void alter_net();        // Change status of network connection
  void send_cycle();       // Send cycle info to slave computer
  void proc_msg();         // Process new message
  void server_connected(); // Connected to network
  void server_disabled();  // Disabled network
  void server_error();     // Error in connection
  void socket_succeed();   // Succeed in socket
};

#endif // HOST_H
