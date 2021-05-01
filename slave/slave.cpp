#include "slave.h"
#include "ui_slave.h"

CurvePlot::CurvePlot(QWidget *parent) : QwtPlot(parent) {
    canvas = new QwtPlotCanvas;
    canvas->setPalette(Qt::white);
    canvas->setBorderRadius(10);
    setCanvas(canvas);

    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->attach(this);

    setCanvasBackground(Qt::white);

    setAxisScale(xBottom, 0, CHART_LEN);
    setAxisScale(yLeft, 0, 10000);

    plotLayout()->setAlignCanvasToScales(true);

    curve = new QwtPlotCurve;
    curve->setPen(QPen(Qt::red));
    curve->attach(this);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

void CurvePlot::refresh(QVector<ResistData> data) {
    coord_y.clear();

    for (int i = 0; i < data.size(); i++) {
        coord_x.push_back(i);
        coord_y.push_back(data[i].resistance.toDouble());
    }

    curve->setSamples(coord_x, coord_y);
    replot();
}

Slave::Slave(QWidget *parent) : QMainWindow(parent), ui(new Ui::Slave) {
    ui->setupUi(this);

    setWindowTitle("Slave Computer Application");
    setFixedSize(480, 272);

    widget = centralWidget();
    layout = new QGridLayout;
    widget->setLayout(layout);

    monitor = new Monitor; // Warning: Camera needs to be initialized before
                           // curve plot

    init_widgets();

    comm = new Comm;
    pack = new CommPack;

    tm_sample = new QTimer;

    tabs->addTab(panel, "Info");
    tabs->addTab(gallery, "Image");
    // tabs->addTab(visual, "Chart");
    tabs->addTab(cv_plot, "Curve");
    tabs->addTab(ctrl, "Control");

    // Buffer on top
    lb_buf_0 = new QLabel;
    lb_buf_0->setFixedHeight(10);

    lb_info = new QLabel;
    lb_info->setFixedHeight(25);
    lb_info->setText("Welcome. Please sign in !");
    //  lb_info->setFont(*ft_info);
    lb_info->setAlignment(Qt::AlignCenter);

    lb_buf_1 = new QLabel;
    lb_buf_1->setFixedHeight(25);

    // Panel
    lb_entry = new QLabel;
    lb_entry->setFixedSize(120, 15);
    lb_entry->setText("Resistance: ");
    // lb_entry->setFont(*ft_btext);
    lb_entry->setAlignment(Qt::AlignRight);

    lb_value = new QLabel;
    lb_value->setFixedSize(80, 15);
    // lb_value->setText("1024 Ohm");
    // lb_value->setFont(*ft_text);
    lb_value->setAlignment(Qt::AlignLeft);

    lb_state = new QLabel;
    lb_state->setFixedSize(100, 15);
    lb_state->setText("Status: ");
    // lb_state->setFont(*ft_btext);
    lb_state->setAlignment(Qt::AlignRight);

    // To-do: add multi-color identifier
    lb_warn = new QLabel;
    lb_warn->setFixedSize(80, 15);
    // lb_warn->setText("Normal");
    // lb_warn->setFont(*ft_text);
    lb_warn->setAlignment(Qt::AlignLeft);

    lb_host = new QLabel;
    lb_host->setFixedSize(120, 15);
    lb_host->setText("Host: ");
    // lb_host->setFont(*ft_btext);
    lb_host->setAlignment(Qt::AlignRight);

    lb_addr = new QLabel;
    lb_addr->setFixedSize(120, 15);
    // lb_addr->setFont(*ft_btext);
    lb_addr->setAlignment(Qt::AlignLeft);

    lb_net_state = new QLabel;
    lb_net_state->setFixedSize(100, 15);
    lb_net_state->setText("Status: ");
    // lb_net_state->setFont(*ft_btext);
    lb_net_state->setAlignment(Qt::AlignRight);

    lb_cnct = new QLabel;
    lb_cnct->setFixedSize(80, 15);
    lb_cnct->setText("Disabled");
    // lb_cnct->setFont(*ft_btext);
    lb_cnct->setAlignment(Qt::AlignLeft);

    lb_cycle = new QLabel;
    lb_cycle->setFixedSize(120, 15);
    lb_cycle->setText("Period(ms): ");
    // lb_cycle->setFont(*ft_btext);
    lb_cycle->setAlignment(Qt::AlignRight);

    cb_cycle = new QComboBox;
    cb_cycle->addItem("1000");
    cb_cycle->addItem("500");
    cb_cycle->addItem("200");
    cb_cycle->addItem("100");
    cb_cycle->addItem("50");
    cb_cycle->addItem("custom");
    current_cycle = DEFAULT_CYCLE;

    btn_apply = new QPushButton;
    btn_apply->setFixedSize(55, 20);
    btn_apply->setText("Apply");
    btn_apply->setDisabled(true);

    lb_buf_2 = new QLabel;

    table = new QTableWidget;
    table->setColumnCount(3);
    table->setFixedSize(400, 100);
    table->setColumnWidth(0, 160);
    table->setColumnWidth(1, 110);
    table->setColumnWidth(2, 110);
    table->verticalHeader()->setDefaultSectionSize(12);
    table->verticalHeader()->setVisible(false);
    QStringList headers;
    headers << "Time"
            << "Resistance"
            << "Status";
    table->setHorizontalHeaderLabels(headers);
    // table->setFont(*ft_code);
    // table->horizontalHeader()->setFont(*ft_btext);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Login
    lb_buf_3 = new QLabel;
    lb_buf_4 = new QLabel;
    lb_buf_3->setFixedHeight(20);
    lb_buf_4->setFixedWidth(250);

    lb_buf_5 = new QLabel;
    lb_buf_5->setFixedHeight(20);

    lb_id = new QLabel;
    lb_id->setFixedSize(175, 20);
    lb_id->setText("Username: ");
    // lb_id->setFont(*ft_btext);
    lb_id->setAlignment(Qt::AlignRight);

    le_id = new QLineEdit;
    le_id->setFixedSize(160, 20);
    le_id->setPlaceholderText("Enter Username");
    // le_id->setFont(*ft_code);
    le_id->setAlignment(Qt::AlignCenter);

    lb_pwd = new QLabel;
    lb_pwd->setFixedSize(175, 20);
    lb_pwd->setText("Password: ");
    // lb_pwd->setFont(*ft_btext);
    lb_pwd->setAlignment(Qt::AlignRight);

    le_pwd = new QLineEdit;
    le_pwd->setFixedSize(160, 20);
    le_pwd->setPlaceholderText("Enter Password");
    // le_pwd->setFont(*ft_code);
    le_pwd->setAlignment(Qt::AlignCenter);
    le_pwd->setEchoMode(QLineEdit::Password);

    btn_login = new QPushButton;
    btn_login->setFixedSize(95, 30);
    btn_login->setText("Login");

    lb_image = new QLabel;
    lb_image->setFixedSize(220, 165);
    lb_image->setFrameShape(QFrame::Box);

    btn_prev = new QPushButton;
    btn_next = new QPushButton;
    btn_prev->setText("Previous");
    btn_next->setText("Next");
    btn_prev->setFixedSize(95, 20);
    btn_next->setFixedSize(95, 20);

    lb_buf_6 = new QLabel;
    lb_buf_6->setFixedSize(95, 60);

    btn_folder = new QPushButton;
    btn_folder->setFixedSize(95, 20);
    btn_folder->setText("Folder");

    // Control
    lb_logout = new QLabel;
    lb_logout->setFixedSize(180, 20);
    lb_logout->setText("User:");
    lb_logout->setAlignment(Qt::AlignRight);

    btn_logout = new QPushButton;
    btn_logout->setFixedSize(95, 20);
    btn_logout->setText("Sign Out");

    lb_net = new QLabel;
    lb_net->setFixedSize(180, 20);
    lb_net->setText("Network: ");
    lb_net->setAlignment(Qt::AlignRight);

    btn_net = new QPushButton;
    btn_net->setFixedSize(95, 20);
    btn_net->setText("Enable");

    lb_cam = new QLabel;
    lb_cam->setFixedSize(180, 20);
    lb_cam->setText("Camera:");
    lb_cam->setAlignment(Qt::AlignRight);

    btn_cam = new QPushButton;
    btn_cam->setFixedSize(95, 20);
    btn_cam->setText("Open");

    lb_sys = new QLabel;
    lb_sys->setFixedSize(180, 20);
    lb_sys->setText("System:");
    lb_sys->setAlignment(Qt::AlignRight);

    btn_sys = new QPushButton;
    btn_sys->setFixedSize(95, 20);
    btn_sys->setText("Start");

    lb_buf_7 = new QLabel;
    lb_buf_7->setFixedWidth(50);

    // debug
    //  lb_info->setFrameShape(QFrame::Box);
    //  lb_entry->setFrameShape(QFrame::Box);
    //  lb_value->setFrameShape(QFrame::Box);
    //  lb_state->setFrameShape(QFrame::Box);
    //  lb_warn->setFrameShape(QFrame::Box);
    //  lb_host->setFrameShape(QFrame::Box);
    //  lb_addr->setFrameShape(QFrame::Box);
    //  lb_net_state->setFrameShape(QFrame::Box);
    //  lb_cnct->setFrameShape(QFrame::Box);
    //  lb_cycle->setFrameShape(QFrame::Box);
    //  lb_id->setFrameShape(QFrame::Box);
    //  lb_pwd->setFrameShape(QFrame::Box);
    //  lb_buf_0->setFrameShape(QFrame::Box);
    //  lb_buf_1->setFrameShape(QFrame::Box);
    //  lb_buf_2->setFrameShape(QFrame::Box);
    //  lb_buf_3->setFrameShape(QFrame::Box);
    //  lb_buf_4->setFrameShape(QFrame::Box);
    //  lb_buf_5->setFrameShape(QFrame::Box);
    //  lb_buf_6->setFrameShape(QFrame::Box);

    // Set panel layout
    layout_panel->addWidget(lb_entry, 0, 0, 1, 1);
    layout_panel->addWidget(lb_value, 0, 1, 1, 1);
    layout_panel->addWidget(lb_state, 0, 2, 1, 1);
    layout_panel->addWidget(lb_warn, 0, 3, 1, 1);
    layout_panel->addWidget(lb_host, 1, 0, 1, 1);
    layout_panel->addWidget(lb_addr, 1, 1, 1, 1);
    layout_panel->addWidget(lb_net_state, 1, 2, 1, 1);
    layout_panel->addWidget(lb_cnct, 1, 3, 1, 1);
    layout_panel->addWidget(lb_cycle, 2, 0, 1, 1);
    layout_panel->addWidget(cb_cycle, 2, 1, 1, 1);
    layout_panel->addWidget(btn_apply, 2, 2, 1, 1);
    layout_panel->addWidget(table, 3, 0, 1, 4);

    panel->setLayout(layout_panel);

    // Set login layout
    layout_login->addWidget(lb_buf_3, 0, 0, 1, 2);
    layout_login->addWidget(lb_id, 1, 0, 1, 1);
    layout_login->addWidget(le_id, 1, 1, 1, 1);
    layout_login->addWidget(lb_pwd, 2, 0, 1, 2);
    layout_login->addWidget(le_pwd, 2, 1, 1, 2);
    layout_login->addWidget(lb_buf_4, 3, 0, 1, 2);
    layout_login->addWidget(btn_login, 3, 2, 1, 1);
    layout_login->addWidget(lb_buf_5, 4, 0, 1, 2);

    login->setLayout(layout_login);

    // Set gallery layout
    layout_gallery->addWidget(lb_image, 0, 0, 4, 1);
    layout_gallery->addWidget(btn_prev, 0, 1, 1, 1);
    layout_gallery->addWidget(btn_next, 1, 1, 1, 1);
    layout_gallery->addWidget(lb_buf_6, 2, 1, 1, 1);
    layout_gallery->addWidget(btn_folder, 3, 1, 1, 1);

    gallery->setLayout(layout_gallery);

    // Set control layout
    layout_ctrl->addWidget(lb_logout, 0, 0, 1, 1);
    layout_ctrl->addWidget(btn_logout, 0, 1, 1, 1);
    layout_ctrl->addWidget(lb_buf_7, 0, 2, 4, 1);
    layout_ctrl->addWidget(lb_net, 1, 0, 1, 1);
    layout_ctrl->addWidget(btn_net, 1, 1, 1, 1);
    layout_ctrl->addWidget(lb_cam, 2, 0, 1, 1);
    layout_ctrl->addWidget(btn_cam, 2, 1, 1, 1);
    layout_ctrl->addWidget(lb_sys, 3, 0, 1, 1);
    layout_ctrl->addWidget(btn_sys, 3, 1, 1, 1);

    ctrl->setLayout(layout_ctrl);

    sys_state = false;

    init_table();
    init_image();
    init_connection();

    stop();
    change_ui(VISITOR);
}

Slave::~Slave() { delete ui; }

void Slave::init_widgets() {
    tabs = new QTabWidget;

    panel = new QWidget;
    layout_panel = new QGridLayout;

    gallery = new QWidget;
    layout_gallery = new QGridLayout;

    visual = new QWidget;
    cv_plot = new CurvePlot;

    login = new QWidget;
    layout_login = new QGridLayout;

    ctrl = new QWidget;
    layout_ctrl = new QGridLayout;
}

void Slave::init_connection() {
    connect(tm_sample, SIGNAL(timeout()), monitor, SLOT(sample()));
    connect(tm_sample, SIGNAL(timeout()), monitor, SLOT(film()));

    connect(btn_prev, SIGNAL(clicked()), this, SLOT(show_prev_image()));
    connect(btn_next, SIGNAL(clicked()), this, SLOT(show_next_image()));
    connect(btn_folder, SIGNAL(clicked()), this, SLOT(folder()));

    connect(btn_logout, SIGNAL(clicked()), this, SLOT(usr_logout()));
    connect(btn_net, SIGNAL(clicked()), this, SLOT(alter_net()));
    connect(btn_sys, SIGNAL(clicked()), this, SLOT(alter_state()));
    connect(btn_cam, SIGNAL(clicked()), this, SLOT(alter_cam()));

    connect(btn_apply, SIGNAL(clicked()), this, SLOT(apply_cycle()));
    connect(btn_login, SIGNAL(clicked()), this, SLOT(usr_login()));

    connect(comm, SIGNAL(client_connected()), this, SLOT(client_connected()));
    connect(comm, SIGNAL(client_disconnected()), this,
            SLOT(client_disconnected()));
    connect(comm, SIGNAL(client_disabled()), this, SLOT(client_disabled()));

    connect(comm, SIGNAL(data_received()), this, SLOT(proc_msg()));
}

void Slave::init_image() {
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
        lb_image->setPixmap(imgs[imgs.size() - 1]);
        img_rsv = imgs[imgs.size() - 1];
    }
    if (count <= 1)
        btn_next->setDisabled(true);
    btn_prev->setDisabled(true);
}

void Slave::init_table() {
    std::string filename = "data.txt";
    std::ifstream data_file((DATA_PATH + filename).data());

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

void Slave::change_ui(int opt) {
    if (opt == VISITOR) {
        comm->disable_net();

        // Change widget layout
        clear_layout(layout);

        layout->addWidget(lb_buf_0, 0, 0, 1, 1);
        layout->addWidget(lb_info, 1, 0, 1, 1);
        // layout->addWidget(lb_buf_1, 1, 1, 1, 1);
        layout->addWidget(login, 2, 0, 1, 1);
        // layout->addWidget(lb_buf_2, 2, 1, 1, 1);

        reset();
    } else if (opt == USER) {
        // Change widget layout
        clear_layout(layout);
        layout->addWidget(tabs, 0, 0);

        // debug
        // cv_plot->refresh(data);
    } else
        return;
}

void Slave::add_image(QPixmap img) {
    imgs.push_back(img);
    lb_image->setPixmap(imgs[imgs.size() - 1]);
    img_id = imgs.size() - 1;
    btn_next->setDisabled(true);
    if (imgs.size() > 1)
        btn_prev->setEnabled(true);
}

bool Slave::run() {
    if (!monitor->init_adc()) {
        if (comm->net_state != DISABLED) {
            QString msg = "Error initializing ADC !";
            comm->send_pack = str2pack(msg);
            comm->send();
        }
        return false;
    }

    if (comm->net_state != DISABLED) {
        QString msg = "Initialized ADC";
        comm->send_pack = str2pack(msg);
        comm->send();
    }

    connect(monitor, SIGNAL(sampled()), this, SLOT(proc_data()));
    connect(monitor, SIGNAL(filmed()), this, SLOT(proc_image()));

    tm_sample->start(current_cycle);

    btn_apply->setEnabled(true);
}

void Slave::stop() {
    btn_apply->setDisabled(true);

    disconnect(monitor, SIGNAL(sampled()), this, SLOT(proc_data()));
    disconnect(monitor, SIGNAL(filmed()), this, SLOT(proc_image()));

    tm_sample->stop();
}

void Slave::reset() {
    lb_value->clear();
    lb_warn->clear();
    cb_cycle->setCurrentIndex(0);
}

// Slot functions
void Slave::folder() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open File"), "/download/slave/image",
        tr("Images (*.png *.xpm *.jpg)"));
}

void Slave::show_prev_image() {
    if (img_id == 0) {
        return;
    } else {
        lb_image->setPixmap(imgs[--img_id]);
        btn_next->setEnabled(true);
        if (img_id == 0)
            btn_prev->setDisabled(true);
    }
}

void Slave::show_next_image() {
    if (img_id == imgs.size() - 1)
        return;
    else {
        lb_image->setPixmap(imgs[++img_id]);
        btn_prev->setEnabled(true);
        if (img_id == imgs.size() - 1)
            btn_next->setDisabled(true);
    }
}

void Slave::alter_state() {
    if (sys_state == true) {
        stop();
        sys_state = false;
        btn_sys->setText("Start");
    } else {
        if (run()) {
            sys_state = true;
            btn_sys->setText("Stop");
        }
    }
}

void Slave::alter_cam() {
    if (monitor->cam->isOpen) {
        monitor->cam->CloseDevice();
        btn_cam->setText("Open");
    } else {
        if (monitor->cam->OpenDevice())
            btn_cam->setText("Close");
    }
}

void Slave::alter_net() {
    if (comm->net_state == DISABLED)
        comm->init_net();
    else
        comm->disable_net();
}

void Slave::apply_cycle() {
    QString cycle_str = cb_cycle->currentText();
    if (cycle_str == "custom") {
        QMessageBox::warning(NULL, "Error", "Custom period undefined !",
                             QMessageBox::Yes);
        return;
    }

    int cycle = cycle_str.toInt();

    if (cycle == current_cycle)
        return;
    else {
        current_cycle = cycle;
        tm_sample->setInterval(current_cycle);
        if (comm->net_state != DISABLED) {
            send_cycle();
        }
        cb_cycle->setItemText(cb_cycle->count() - 1, "custom");
        QMessageBox::information(NULL, "Notice", "Period Value Applied !",
                                 QMessageBox::Yes);
    }
}

void Slave::usr_login() {
    QString id = le_id->text(), pwd = le_pwd->text();
    QString name;

    change_ui(USER);
    //  switch (verify_user(id, pwd, name)) {
    //  case 0: {
    //    this->usr = new User(name, id, pwd);
    //    change_ui(USER);
    //    le_id->clear();
    //    le_pwd->clear();
    //    break;
    //  }
    //  case 1: {
    //    le_pwd->clear();
    //    QMessageBox::warning(NULL, "Error", "Wrong Password !");
    //    break;
    //  }
    //  case 2: {
    //    QMessageBox::warning(NULL, "Error", "User ID Not Exist !");
    //    break;
    //  }
    //  default:
    //    break;
    //  }
}

void Slave::usr_logout() {
    // delete usr;
    change_ui(VISITOR);
}

void Slave::send_cycle() {
    comm->send_pack = str2pack(cb_cycle->currentText(), CYCLE);
    comm->send_pack.type = CYCLE;
    comm->send();
}

void Slave::proc_msg() {
    *pack = comm->recv_pack;
    if (pack->byteData.size() > 0) {
        if (pack->type == CYCLE) {
            QString cycle_str = pack->cvt2str();
            current_cycle = cycle_str.toInt();
            tm_sample->setInterval(current_cycle);
            for (int i = 0; i < cb_cycle->count() - 1; i++) {
                if (cb_cycle->itemText(i) == cycle_str) {
                    cb_cycle->setCurrentIndex(i);
                    cb_cycle->setItemText(cb_cycle->count() - 1, "custom");
                    return;
                }
                cb_cycle->setCurrentIndex(cb_cycle->count() - 1);
                cb_cycle->setItemText(cb_cycle->count() - 1, cycle_str);
                return;
            }
        } else if (pack->type == HEARTBEAT) {
            printf("Heartbeat pack received.\n");
            comm->check_hbt();
        } else if (pack->type == REQUEST) {
            // send_cycle();
        }
    }
}

void Slave::proc_data() {
    QString resist = QString::number(monitor->resist);
    ResistData new_data = *(new ResistData(resist));

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
    cv_plot->refresh(data);

    if (comm->net_state != DISABLED) {
        comm->send_pack = resist2pack(new_data);
        comm->send();
    }
}

void Slave::proc_image() {
    QPixmap img = monitor->img;
    write_image(img);
    add_image(img);

    if (comm->net_state != DISABLED) {
        comm->send_pack = image2pack(img);
        comm->send();
    }
    printf("Sending image\n");
    img_rsv = img;
}

void Slave::client_connected() {
    btn_net->setText("Disable");
    lb_addr->setText(HOST_ADDR);
    lb_cnct->setText("Connected");
}

void Slave::client_disconnected() {
    btn_net->setText("Disable");
    lb_addr->setText(HOST_ADDR);
    lb_cnct->setText("Disconnected");
}

void Slave::client_disabled() {
    btn_net->setText("Enable");
    lb_addr->clear();
    lb_cnct->setText("Disabled");
}
