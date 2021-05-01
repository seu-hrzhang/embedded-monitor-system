#include "host.h"
#include "ui_host.h"

Host::Host(QWidget *parent) : QMainWindow(parent), ui(new Ui::Host) {
  ui->setupUi(this);

  setWindowTitle("Host Computer Application");

  QWidget *widget_center = takeCentralWidget();
  if (widget_center)
    delete widget_center;
  setDockNestingEnabled(true);

  widget = new QWidget;
  widget->setFixedSize(840, 600);
  layout = new QGridLayout;
  widget->setLayout(layout);
  setCentralWidget(widget);

  init_menu();
  init_font();
  init_widgets();

  comm = new Comm;
  pack = new CommPack;

  // Buffer on top
  lb_buf_0 = new QLabel;
  lb_buf_0->setFixedHeight(10);

  // User info bar
  lb_info = new QLabel;
  lb_info->setFixedSize(400, 30);
  lb_info->setFont(*ft_info);
  lb_info->setAlignment(Qt::AlignCenter);

  lb_buf_1 = new QLabel;
  lb_buf_1->setFixedHeight(30);

  // Panel
  lb_entry = new QLabel;
  lb_entry->setFixedSize(95, 20);
  lb_entry->setText("Resistance: ");
  lb_entry->setFont(*ft_btext);
  lb_entry->setAlignment(Qt::AlignRight);

  lb_value = new QLabel;
  lb_value->setFixedSize(95, 20);
  // lb_value->setText("1024 Ohm");
  lb_value->setFont(*ft_text);
  lb_value->setAlignment(Qt::AlignLeft);

  lb_state = new QLabel;
  lb_state->setFixedSize(95, 20);
  lb_state->setText("Status: ");
  lb_state->setFont(*ft_btext);
  lb_state->setAlignment(Qt::AlignRight);

  // To-do: add multi-color identifier
  lb_warn = new QLabel;
  lb_warn->setFixedSize(95, 20);
  // lb_warn->setText("Normal");
  lb_warn->setFont(*ft_text);
  lb_warn->setAlignment(Qt::AlignLeft);

  lb_cycle = new QLabel;
  lb_cycle->setFixedSize(95, 20);
  lb_cycle->setText("Period(ms): ");
  lb_cycle->setFont(*ft_btext);
  lb_cycle->setAlignment(Qt::AlignRight);

  le_cycle = new QLineEdit;
  le_cycle->setFixedSize(70, 20);
  le_cycle->setPlaceholderText("period");
  le_cycle->setFont(*ft_code);
  le_cycle->setAlignment(Qt::AlignCenter);
  QString cycle_init = QString::number(DEFAULT_CYCLE);
  current_cycle = DEFAULT_CYCLE;

  btn_apply = new QPushButton;
  btn_apply->setFixedSize(95, 30);
  btn_apply->setText("Apply");
  btn_apply->setDisabled(true);

  lb_image = new QLabel;
  lb_image->setFixedSize(380, 285);
  lb_image->setFrameShape(QFrame::Box);

  btn_prev = new QPushButton;
  btn_next = new QPushButton;
  btn_prev->setText("←");
  btn_next->setText("→");
  btn_prev->setFixedSize(95, 30);
  btn_next->setFixedSize(95, 30);

  lb_buf_2 = new QLabel;
  lb_buf_2->setFixedSize(95, 30);

  btn_folder = new QPushButton;
  btn_folder->setFixedSize(95, 30);
  btn_folder->setText("Folder");

  // Line chart
  axis_x = new QValueAxis;
  axis_x->setRange(0, 10);
  axis_x->setTickCount(CHART_LEN);
  axis_x->setLabelsFont(*ft_code);
  axis_x->setLabelFormat("%d");
  axis_x->hide();

  axis_y = new QValueAxis;
  axis_y->setRange(0, 10000);
  axis_y->setTickCount(11);
  axis_y->setLabelsFont(*ft_code);
  axis_y->setLabelFormat("%d");

  pen = new QPen;
  pen->setColor(Qt::red);
  pen->setWidth(1);

  series = new QLineSeries;
  series->setPen(*pen);

  chart = new QChart;
  chart->addSeries(series);
  chart->setTitle("Resistance Data Curve");
  chart->setAxisX(axis_x, series);
  chart->setAxisY(axis_y, series);
  chart->legend()->hide();
  chart->setTitleFont(*ft_btext);

  view = new QChartView(chart);
  view->setRenderHint(QPainter::Antialiasing); // Anti-alias
  view->setFixedSize(380, 335);
  view->setFont(*ft_code);

  // Terminal
  table = new QTableWidget;
  table->setColumnCount(3);
  table->setFixedSize(380, 200);
  table->setColumnWidth(0, 160);
  table->setColumnWidth(1, 108);
  table->setColumnWidth(2, 108);
  table->verticalHeader()->setDefaultSectionSize(12);
  table->verticalHeader()->setVisible(false);
  QStringList headers;
  headers << "Time"
          << "Resistance"
          << "Status";
  table->setHorizontalHeaderLabels(headers);
  table->setFont(*ft_code);
  table->horizontalHeader()->setFont(*ft_btext);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  term = new QTextEdit;
  term->setFixedSize(380, 220);
  term->setReadOnly(true);
  term->setFont(*ft_code);

  // debug
  //  ResistData *item_high = new ResistData("10000");
  //  ResistData *item_normal = new ResistData("5000");
  //  ResistData *item_low = new ResistData("300");
  //  add_table_item(table, item_high);
  //  add_table_item(table, item_low);
  //  add_table_item(table, item_normal);

  // Login
  lb_buf_3 = new QLabel;
  lb_buf_4 = new QLabel;
  lb_buf_3->setFixedSize(380, 50);
  lb_buf_4->setFixedWidth(380);

  lb_buf_5 = new QLabel;
  lb_buf_5->setFixedHeight(20);

  lb_buf_6 = new QLabel;
  lb_buf_6->setFixedSize(400, 450);

  lb_id = new QLabel;
  lb_id->setFixedSize(130, 20);
  lb_id->setText("Username: ");
  lb_id->setFont(*ft_btext);
  lb_id->setAlignment(Qt::AlignRight);

  le_id = new QLineEdit;
  le_id->setFixedSize(190, 20);
  le_id->setPlaceholderText("Enter Username");
  le_id->setFont(*ft_code);
  le_id->setAlignment(Qt::AlignCenter);

  lb_pwd = new QLabel;
  lb_pwd->setFixedSize(130, 20);
  lb_pwd->setText("Password: ");
  lb_pwd->setFont(*ft_btext);
  lb_pwd->setAlignment(Qt::AlignRight);

  le_pwd = new QLineEdit;
  le_pwd->setFixedSize(190, 20);
  le_pwd->setPlaceholderText("Enter Password");
  le_pwd->setFont(*ft_code);
  le_pwd->setAlignment(Qt::AlignCenter);
  le_pwd->setEchoMode(QLineEdit::Password);

  btn_login = new QPushButton;
  btn_login->setFixedSize(95, 30);
  btn_login->setText("Login");

  // debug
  //  lb_buf_0->setFrameShape(QFrame::Box);
  //  lb_info->setFrameShape(QFrame::Box);
  //  lb_entry->setFrameShape(QFrame::Box);
  //  lb_value->setFrameShape(QFrame::Box);
  //  lb_state->setFrameShape(QFrame::Box);
  //  lb_warn->setFrameShape(QFrame::Box);
  //  lb_cycle->setFrameShape(QFrame::Box);
  //  lb_id->setFrameShape(QFrame::Box);
  //  lb_pwd->setFrameShape(QFrame::Box);
  //  lb_buf_1->setFrameShape(QFrame::Box);
  //  lb_buf_2->setFrameShape(QFrame::Box);
  //  lb_buf_3->setFrameShape(QFrame::Box);
  //  lb_buf_4->setFrameShape(QFrame::Box);
  //  lb_buf_5->setFrameShape(QFrame::Box);

  // Set panel layout
  layout_panel->addWidget(lb_entry, 0, 0, 1, 1);
  layout_panel->addWidget(lb_value, 0, 1, 1, 1);
  layout_panel->addWidget(lb_state, 0, 2, 1, 1);
  layout_panel->addWidget(lb_warn, 0, 3, 1, 1);

  layout_panel->addWidget(lb_cycle, 1, 0, 1, 1);
  layout_panel->addWidget(le_cycle, 1, 1, 1, 1);
  layout_panel->addWidget(btn_apply, 1, 2, 1, 1);

  layout_panel->addWidget(lb_image, 2, 0, 1, 4);
  //  layout_panel->addWidget(view, 2, 0, 1, 4);

  layout_panel->addWidget(btn_prev, 3, 0, 1, 1);
  layout_panel->addWidget(btn_next, 3, 1, 1, 1);
  layout_panel->addWidget(lb_buf_2, 3, 2, 1, 1);
  layout_panel->addWidget(btn_folder, 3, 3, 1, 1);

  panel->setLayout(layout_panel);

  // Set inspector layout
  layout_inspect->addWidget(table, 0, 0, 1, 1);
  layout_inspect->addWidget(term, 1, 0, 1, 1);

  inspect->setLayout(layout_inspect);

  // Set login layout
  layout_login->addWidget(lb_buf_3, 0, 0, 1, 2);

  layout_login->addWidget(lb_id, 1, 0, 1, 1);
  layout_login->addWidget(le_id, 1, 1, 1, 1);

  layout_login->addWidget(lb_pwd, 2, 0, 1, 2);
  layout_login->addWidget(le_pwd, 2, 1, 1, 2);

  layout_login->addWidget(lb_buf_5, 3, 0, 1, 2);
  layout_login->addWidget(btn_login, 3, 2, 1, 1);

  layout_login->addWidget(lb_buf_4, 4, 0, 1, 2);

  login->setLayout(layout_login);

  // debug
  //  for (int i = 0; i < 10; i++)
  //    print_line(term, "This is a very very very very very very very very very
  //    "
  //                     "very very very long sentence.");

  // Connect signals with slots
  init_connection();

  change_ui(VISITOR);
}

void Host::init_font() {
  ft_info = new QFont;
  ft_info->setFamily("Charter");
  ft_info->setPointSize(20);
  ft_info->setBold(true);

  ft_text = new QFont;
  ft_text->setFamily("Charter");
  ft_text->setPointSize(14);

  ft_btext = new QFont;
  ft_btext->setFamily("Charter");
  ft_btext->setPointSize(14);
  ft_btext->setBold(true);

  ft_code = new QFont;
  ft_code->setFamily("SF Mono");
  ft_code->setPointSize(11);
}

void Host::init_menu() {
  mbar = new QMenuBar;

  menu_usr = new QMenu("User");
  menu_view = new QMenu("View");
  menu_net = new QMenu("Network");

  act_usr = new QAction("Username");
  act_usr->setDisabled(true);

  act_logout = new QAction("Sign Out");
  act_logout->setDisabled(true);

  act_line = new QAction("Show History Chart");
  act_line->setDisabled(true);

  act_image = new QAction("Show Images");
  act_image->setDisabled(true);

  act_status = new QAction("Server Offline");
  act_status->setDisabled(true);

  act_net = new QAction("Start Server");
  act_net->setDisabled(true);

  menu_usr->addAction(act_usr);
  menu_usr->addAction(act_logout);

  menu_view->addAction(act_line);
  menu_view->addAction(act_image);

  menu_net->addAction(act_status);
  menu_net->addSeparator();
  menu_net->addAction(act_net);

  mbar->addMenu(menu_usr);
  mbar->addMenu(menu_view);
  mbar->addMenu(menu_net);

  setMenuBar(mbar);
}

void Host::init_widgets() {
  panel = new QWidget;
  panel->setFixedSize(400, 450);
  layout_panel = new QGridLayout;

  inspect = new QWidget;
  inspect->setFixedSize(400, 450);
  layout_inspect = new QGridLayout;

  login = new QWidget;
  login->setFixedSize(400, 450);
  layout_login = new QGridLayout;
}

void Host::init_connection() {
  connect(btn_apply, SIGNAL(clicked()), this, SLOT(apply_cycle()));

  connect(btn_prev, SIGNAL(clicked()), this, SLOT(show_prev_image()));
  connect(btn_next, SIGNAL(clicked()), this, SLOT(show_next_image()));
  connect(btn_folder, SIGNAL(clicked()), this, SLOT(folder()));

  connect(btn_login, SIGNAL(clicked()), this, SLOT(usr_login()));

  connect(act_logout, SIGNAL(triggered()), this, SLOT(usr_logout()));

  connect(act_line, SIGNAL(triggered()), this, SLOT(show_line_chart()));
  connect(act_image, SIGNAL(triggered()), this, SLOT(show_image()));

  connect(act_net, SIGNAL(triggered()), this, SLOT(alter_net()));

  connect(comm, SIGNAL(server_connected()), this, SLOT(server_connected()));
  connect(comm, SIGNAL(server_disabled()), this, SLOT(server_disabled()));
  connect(comm, SIGNAL(server_error()), this, SLOT(server_error()));
  connect(comm, SIGNAL(socket_succeed()), this, SLOT(socket_succeed()));
  connect(comm, SIGNAL(data_received()), this, SLOT(proc_msg()));
}

void Host::init_table() {
  std::string filename = "data.txt";
  std::ifstream data_file(DATA_PATH + filename);

  std::string date, time, resistance, status;

  ResistData *new_data;

  while (data_file >> date >> time >> resistance >> status) {
    new_data = new ResistData(
        QString::fromStdString(resistance), QString::fromStdString(status),
        QString::fromStdString(date), QString::fromStdString(time));

    add_table_item(table, new_data);

    reorder_data(data, *new_data);

    delete new_data;
  }
}

void Host::init_image() {
  QString path = IMG_PATH;
  QDir dir(path);

  QStringList list;
  list << "*.JPG";

  dir.setNameFilters(list);
  int count = dir.count();

  for (int i = 0; i < count; i++) {
    imgs.push_back(QPixmap::fromImage(*(new QImage(path + dir[i]))));
    if (imgs.size() > MAX_IMG_NUM) {
      QVector<QPixmap>::iterator iter = imgs.begin();
      imgs.erase(iter);
    }
  }
  img_id = 0;
  if (count > 0) {
    lb_image->setPixmap(imgs[0]);
  }
  if (count <= 1)
    btn_next->setDisabled(true);
  btn_prev->setDisabled(true);
}

void Host::change_ui(int opt) {
  term->clear();
  if (opt == VISITOR) {
    // Change menu bar
    act_usr->setText("Visitor");
    act_usr->setDisabled(true);
    act_logout->setDisabled(true);

    act_line->setDisabled(true);
    act_image->setDisabled(true);

    // Change widget layout
    clear_layout(layout);

    lb_info->setText("Welcome. Please sign in !");

    layout->addWidget(lb_buf_0, 0, 0, 1, 2);
    layout->addWidget(lb_info, 1, 0, 1, 1);
    layout->addWidget(lb_buf_1, 1, 1, 1, 1);
    layout->addWidget(login, 2, 0, 1, 1);
    layout->addWidget(lb_buf_6, 2, 1, 1, 1);

    widget->setLayout(layout);

    // Set action
    act_net->setDisabled(true);

    reset();
  } else if (opt == USER) {
    // Read history value
    init_table();

    // Read images
    init_image();

    // Change menu bar
    act_usr->setText(usr->id);
    act_logout->setEnabled(true);

    act_line->setEnabled(true);
    act_image->setDisabled(true);

    // Change widget layout
    clear_layout(layout);

    lb_info->setText("Welcome, " + usr->name);

    layout->addWidget(lb_buf_0, 0, 0, 1, 2);
    layout->addWidget(lb_info, 1, 0, 1, 1);
    layout->addWidget(lb_buf_1, 1, 1, 1, 1);
    layout->addWidget(panel, 2, 0, 1, 1);
    layout->addWidget(inspect, 2, 1, 1, 1);

    widget->setLayout(layout);

    // Set action
    act_net->setEnabled(true);

    // debug
    refresh_chart();
  } else
    return;
}

void Host::refresh_chart() {
  series->clear();

  for (int i = 0; i < data.size(); i++)
    series->append(i, data[i].resistance.toInt());
}

void Host::add_image(QPixmap img) {
  imgs.push_back(img);
  // Initial image vector empty
  if (imgs.size() == 1 && img_id == 0) {
    lb_image->setPixmap(imgs[0]);
    btn_next->setEnabled(true);
  }
  // Enable next image button
  if (img_id < imgs.size() - 1)
    btn_next->setEnabled(true);
  // Remove ealier images
  if (imgs.size() > MAX_IMG_NUM) {
    QVector<QPixmap>::iterator iter = imgs.begin();
    imgs.erase(iter);
    img_id--;
    if (img_id <= 0) {
      btn_prev->setDisabled(true);
      img_id = 0;
    }
  }
}

void Host::run() {
  print_line(term, "Running system...");
  btn_apply->setEnabled(true);
}

void Host::stop() {
  print_line(term, "Stoping system...");
  btn_apply->setDisabled(true);
}

void Host::reset() {
  lb_value->clear();
  lb_warn->clear();

  le_cycle->clear();

  while (table->rowCount() > 0)
    table->removeRow(table->rowCount() - 1);

  term->clear();
}

Host::~Host() { delete ui; }

// Slot functions
void Host::folder() {
  QString path = IMG_PATH;
  open_folder(path);
  print_line(term, "Opened folder: " + path);
}

void Host::show_prev_image() {
  if (img_id == 0) {
    return;
  } else {
    lb_image->setPixmap(imgs[--img_id]);
    btn_next->setEnabled(true);
    print_line(term, "Displayed previous image");
    if (img_id == 0)
      btn_prev->setDisabled(true);
  }
}

void Host::show_next_image() {
  if (img_id == imgs.size() - 1)
    return;
  else {
    lb_image->setPixmap(imgs[++img_id]);
    btn_prev->setEnabled(true);
    print_line(term, "Displayed next image");
    if (img_id == imgs.size() - 1)
      btn_next->setDisabled(true);
  }
}

void Host::apply_cycle() {
  QString cycle_str = le_cycle->text();
  int cycle = cycle_str.toInt();

  // Error with input string
  if (cycle <= 0) {
    print_line(term, "Error: incorrect period format Period: " +
                         QString::number(current_cycle) + "ms");
    le_cycle->setText(QString::number(current_cycle));
    QMessageBox::warning(NULL, "Error",
                         "Please Check Format of Input Period Value !",
                         QMessageBox::Cancel);
    return;
  } else if (cycle == current_cycle)
    return;
  else {
    current_cycle = cycle;
    send_cycle();
    print_line(term, "New period applied Period: " +
                         QString::number(current_cycle) + "ms");
    QMessageBox::information(NULL, "Notice", "Period Value Applied !",
                             QMessageBox::Yes);
  }
}

void Host::usr_login() {
  QString id = le_id->text(), pwd = le_pwd->text();
  QString name;

  switch (verify_user(id, pwd, name)) {
  case 0: {
    this->usr = new User(name, id, pwd);
    change_ui(USER);
    le_id->clear();
    le_pwd->clear();
    print_line(term, "User \'" + id + "\' signed in");
    break;
  }
  case 1: {
    le_pwd->clear();
    QMessageBox::warning(NULL, "Error", "Wrong Password !");
    print_line(term, "Login error: wrong password");
    break;
  }
  case 2: {
    QMessageBox::warning(NULL, "Error", "User ID Not Exist !");
    print_line(term, "Login error: user id not exist");
    break;
  }
  default:
    break;
  }
}

void Host::usr_logout() {
  delete usr;
  change_ui(VISITOR);
}

void Host::show_line_chart() {
  view->show();
  layout_panel->replaceWidget(lb_image, view);
  lb_image->setVisible(false);
  btn_prev->setVisible(false);
  btn_next->setVisible(false);
  lb_buf_2->setVisible(false);
  btn_folder->setVisible(false);

  act_line->setDisabled(true);
  act_image->setEnabled(true);
}

void Host::show_image() {
  lb_image->show();
  layout_panel->replaceWidget(view, lb_image);
  view->hide();
  btn_prev->setVisible(true);
  btn_next->setVisible(true);
  lb_buf_2->setVisible(true);
  btn_folder->setVisible(true);

  act_image->setDisabled(true);
  act_line->setEnabled(true);
}

void Host::alter_net() {
  if (act_status->text() == "Server Offline") {
    print_line(term, "Start listening...");
    comm->init_net();
  } else if (act_status->text() == "Server Online") {
    print_line(term, "Disabling connection...");
    comm->disable_net();
  }
}

void Host::send_cycle() {
  print_line(term, "Sending period information...");
  comm->send_pack = str2pack(le_cycle->text(), CYCLE);
  comm->send_pack.type = CYCLE;
  comm->send();
}

void Host::proc_msg() {
  *pack = comm->recv_pack;
  if (pack->byteData.size() > 0) {
    if (pack->type == SYS_INFO) {
      QString info = pack->cvt2str();
      print_line(term, "System: " + info);
    } else if (pack->type == CYCLE) {
      QString cycle = pack->cvt2str();
      le_cycle->setText(cycle);
      print_line(term, "Sampling period set to " + cycle + "ms");
    } else if (pack->type == HEARTBEAT) {
      print_line(term, "Heartbeat pack received, replying...");
      comm->send_pack = *pack;
      comm->send();
    } else if (pack->type == DATA_RESIST) {
      ResistData new_data = pack->cvt2resist();
      lb_value->setText(new_data.resistance);

      lb_warn->setText(new_data.status);
      QPalette plt;
      if (new_data.status == "high")
        plt.setColor(QPalette::WindowText, Qt::red);
      else if (new_data.status == "normal")
        plt.setColor(QPalette::WindowText, Qt::green);
      else
        plt.setColor(QPalette::WindowText, Qt::yellow);
      lb_warn->setPalette(plt);

      write_data(&new_data);
      add_table_item(table, &new_data);
      reorder_data(data, new_data);
      refresh_chart();
      print_line(term, "Processed new resistance data");
    } else if (pack->type == DATA_IMAGE) {
      QPixmap new_image = pack->cvt2image();
      add_image(new_image);
      write_image(new_image);
      print_line(term, "Processed new image");
    } else if (pack->type == DATA_STR) {
      print_line(term, "Received string: " + pack->cvt2str());
    } else
      print_line(term, "Error: unidentified data");
  }
}

void Host::server_connected() {
  print_line(term, "Server online");
  act_status->setText("Server Online");
  act_net->setText("Stop Server");
  run();
}

void Host::server_disabled() {
  print_line(term, "Server disabled");
  act_status->setText("Server Offline");
  act_net->setText("Start Server");
  stop();
}

void Host::server_error() { print_line(term, "Error in connection"); }

void Host::socket_succeed() { print_line(term, "Start listening..."); }
