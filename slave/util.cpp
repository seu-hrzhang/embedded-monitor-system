#include "util.h"

using namespace std;

User::User(QString name, QString id, QString pwd) {
  this->name = name;
  this->id = id;
  this->pwd = pwd;
}

ResistData::ResistData(QString resistance, QString status, QString date,
                       QString time) {
  this->date = date;
  this->time = time;
  this->resistance = resistance;
  this->status = status;
}

ResistData::ResistData(QString resistance) {
  this->resistance = resistance;
  get_status();
  get_time();
}

void ResistData::get_status() {
  if (resistance.toInt() > 9000)
    status = "high";
  else if (resistance.toInt() < 1000)
    status = "low";
  else
    status = "normal";
}

void ResistData::get_time() {
  date = get_current_time(TIME_DATE);
  time = get_current_time(TIME_CONCISE);
}

QString ResistData::merge() {
  return date + "\t" + time + "\t" + resistance + "\t" + status;
}

void print_line(QTextEdit *term, QString str) {
  QTextCursor cursor = term->textCursor();
  // cursor.movePosition(QTextCursor::Start); // Print reversely
  cursor.insertText("[" + get_current_time(TIME_CONCISE) + "] ");
  cursor.insertText(str);
  cursor.insertText("\n");
}

// void open_folder(QString path) {
//  QDesktopServices::openUrl(QUrl("file://" + path, QUrl::TolerantMode));
//}

void add_table_item(QTableWidget *table, ResistData *item) {
  int row = table->rowCount();
  table->insertRow(row);

  QTableWidgetItem *time = new QTableWidgetItem(item->date + " " + item->time);
  QTableWidgetItem *resistance = new QTableWidgetItem(item->resistance);
  QTableWidgetItem *status = new QTableWidgetItem(item->status);

  time->setTextAlignment(Qt::AlignCenter);
  resistance->setTextAlignment(Qt::AlignCenter);
  status->setTextAlignment(Qt::AlignCenter);

  table->setItem(row, 0, time);
  table->setItem(row, 1, resistance);
  table->setItem(row, 2, status);
}

QString get_current_time(int opt) {
  QDateTime time = QDateTime::currentDateTime();
  if (opt == TIME_FULL)
    return time.toString("yyyy-MM-dd hh:mm:ss");
  else if (opt == TIME_DATE)
    return time.toString("yyyy-MM-dd");
  else if (opt == TIME_CONCISE)
    return time.toString("hh:mm:ss");
  else
    return *(new QString);
}

int verify_user(QString id, QString pwd, QString &name) {
  string filename = "user.txt";
  ifstream user_file((DATA_PATH + filename).data());

  string usr_id, usr_name, usr_pwd;

  while (user_file >> usr_id >> usr_name >> usr_pwd) {
    if (usr_id == id.toStdString()) {
      if (usr_pwd == pwd.toStdString()) {
        name = QString::fromStdString(usr_name);
        return 0;
      } else
        return 1;
    }
  }
  return 2;
}

void clear_layout(QLayout *layout) {
  QLayoutItem *child;
  while ((child = layout->takeAt(0)) != 0) {
    if (child->widget()) {
      child->widget()->setParent(NULL);
    }
    delete child;
  }
}

void write_data(ResistData *data) {
  string filename = "data.txt";
  ofstream data_file((DATA_PATH + filename).data(), ios::app);
  data_file << data->date.toStdString() << "\t" << data->time.toStdString()
            << "\t" << data->resistance.toStdString() << "\t"
            << data->status.toStdString() << endl;
}

void write_image(QPixmap image, QString filename) {
  image.save(IMG_PATH + filename + ".JPG", "JPG", -1);
}

void reorder_data(QVector<ResistData> &data, ResistData new_data) {
  if (data.size() < CHART_LEN)
    data.push_back(new_data);
  else {
    // Wipe first element
    QVector<ResistData>::iterator iter = data.begin();
    data.erase(iter);
    data.push_back(new_data);
  }
}

QVector<QString> split_str(QString str) {
  QVector<QString> vec;
  int count = 0;

  QString str1 = "This\tis\ta\tstring.";

  for (int i = 0; i < str.size(); i++) {
    vec.push_back(*(new QString));

    while (i < str.size() && str[i] != '\t')
      vec[count].push_back(str[i++]);

    count++;
  }
  return vec;
}

bool diff(QPixmap pix_1, QPixmap pix_2, int thresh) {
  QImage img_1 = pix_1.toImage();
  QImage img_2 = pix_2.toImage();
  int num = 0;

  for (int i = 0; i < pix_1.width(); ++i) {
    for (int j = 0; j < pix_1.height(); ++j) {
      int gray_1 = qGray(img_1.pixel(i, j));
      int gray_2 = qGray(img_2.pixel(i, j));
      if (gray_1 != gray_2) {
        num++;
        if (num > thresh) {
          printf("True = %d\n", num);
          return true;
        }
      }
    }
  }
  printf("False = %d\n", num);
  return false;
}
