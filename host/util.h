#ifndef UTIL_H
#define UTIL_H

#include <QBuffer>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <fstream>
#include <string>

#define VISITOR 0
#define USER 1

#define IMG_PATH "/Users/starrynight/Developer/Qt/host/src/image/"
#define DATA_PATH "/Users/starrynight/Developer/Qt/host/src/data/"

#define TIME_FULL 0
#define TIME_DATE 1
#define TIME_CONCISE 2

#define CHART_LEN 10
#define MAX_IMG_NUM 20

#define DEFAULT_CYCLE 1000

// Class of system user
class User {
public:
  bool login;
  QString name;
  QString id;
  QString pwd;

  User(QString name = "User", QString id = "ID", QString pwd = "Password'");
};

// Class of data pack
class ResistData {
public:
  QString date;
  QString time;
  QString resistance;
  QString status;

  ResistData(QString resistance, QString status, QString date = "date",
             QString time = "time");
  ResistData(QString resistance);
  ResistData() {}

  void get_status();
  void get_time();
  QString merge(); // Merge all attribution strings to one
};

// Print content on terminal screen
void print_line(QTextEdit *term, QString str);

// Open specified folder
// e.g. "/usr/local/bin"
void open_folder(QString path);

// Add new item to table
void add_table_item(QTableWidget *table, ResistData *item);

// Get current time in QString format
QString get_current_time(int opt);

// Verify login info
// Returns status code
// 0: success
// 1: user not exist
// 2: wrong password
int verify_user(QString id, QString pwd, QString &name);

// Clear all components in layout
void clear_layout(QLayout *layout);

// Write single pack data to file
void write_data(ResistData *data);

// Write image to folder
void write_image(QPixmap image, QString filename = get_current_time(TIME_FULL));

// Reorder data sequence, adding new data
void reorder_data(QVector<ResistData> &data, ResistData new_data);

// Split a complex string to multiple strings
QVector<QString> split_str(QString str);

#endif // UTIL_H
