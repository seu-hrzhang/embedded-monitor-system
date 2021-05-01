#include "comm.h"

CommPack::CommPack() {
  type = 0;
  desc = "data";
  byteData = QByteArray(0);
}

int CommPack::get_size() {
  return sizeof(int) + desc.size() * 2 + 4 + byteData.size() + 4;
}

QString CommPack::cvt2str() {
  if (type == SYS_INFO || type == CYCLE || type == DATA_STR) {
    return QString::fromLocal8Bit(byteData);
  } else
    return "";
}

ResistData CommPack::cvt2resist() {
  if (type == DATA_RESIST) {
    QVector<QString> vec = split_str(QString::fromLocal8Bit(byteData));
    return *(new ResistData(vec[2], vec[3], vec[0], vec[1]));
  } else
    return *(new ResistData);
}

QPixmap CommPack::cvt2image() {
  if (type == DATA_IMAGE) {
    QPixmap img;
    img.loadFromData(byteData);
    return img;
  } else
    return *(new QPixmap);
}

Comm::Comm(QString host_addr, QString slave_addr) {
  this->host_addr = host_addr;
  this->slave_addr = slave_addr;

  socket = new QTcpSocket;

  net_state = DISCONNECTED;

  trying = 0;
  missed = 0;

  sizeReceived = false;
  data_size = 0;

  tm_hbt_send = new QTimer;
  tm_hbt_recv = new QTimer;

  connect(tm_hbt_send, SIGNAL(timeout()), this, SLOT(send_hbt()));
  connect(tm_hbt_recv, SIGNAL(timeout()), this, SLOT(hbt_missed()));
}

bool Comm::init_net() {
  connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));

  socket->connectToHost(QHostAddress(host_addr), PORT);

  if (socket->waitForConnected(3000) == false) {
    if (net_state == DISABLED)
      QMessageBox::warning(NULL, "Error", "Error Connecting to Server !");
    return false;
  }
  tm_hbt_send->start(HEARTBEAT_SEND);
  tm_hbt_recv->stop();
  emit client_connected();
  net_state = CONNECTED;
  return true;
}

void Comm::disable_net() {
  tm_hbt_send->stop();
  tm_hbt_recv->stop();
  socket->disconnectFromHost();
  net_state = DISABLED;
  emit client_disabled();
}

bool Comm::check_hbt() {
  tm_hbt_recv->stop();
  if (recv_pack.desc == hbt_pack.desc) {
    tm_hbt_send->start(HEARTBEAT_SEND);
    return true;
  } else {
    disable_net();
    QMessageBox::warning(NULL, "Error", "Heartbeat Pack Check Faild !");
    return false;
  }
}

// Slot functions
void Comm::send() {
  tm_hbt_send->stop();
  // Socket not exist or disconnected
  if ((!socket) || (socket->state() != QAbstractSocket::ConnectedState)) {
    emit client_error();
    // Try connecting to server before sending data
    if (!init_net()) {
      net_state = DISCONNECTED;
      emit client_disconnected();
      missed++;
      if (missed > MAX_MISS) {
        disable_net();
        missed = 0;
      }
      return;
    }
    if (trying < MAX_TRY) {
      trying++;
      send();
    }
    net_state = DISCONNECTED;
    emit client_disconnected();
    missed++;
    trying = 0;
    if (missed > MAX_MISS) {
      disable_net();
      missed = 0;
    }
    return;
  }
  QDataStream data_out(socket);
  data_out << send_pack.get_size() << send_pack;
  socket->flush();

  trying = MAX_TRY;
  emit data_sent(); // Send data sent signal
  tm_hbt_send->start(HEARTBEAT_SEND);
}

void Comm::send_hbt() {
  printf("Sending Heartbeat Pack\n");
  tm_hbt_send->stop();

  // Get latest heartbeat pack
  hbt_pack = str2pack(" ", HEARTBEAT);

  // Skipping socket status check
  QDataStream data_out(socket);
  data_out << hbt_pack.get_size() << hbt_pack;
  socket->flush();

  tm_hbt_recv->start(HEARTBEAT_RECV);
}

void Comm::hbt_missed() {
  disable_net();
  QMessageBox::warning(NULL, "Error", "Heartbeat Missed !");
}

void Comm::receive() {
  // Socket not exist or disconnected
  if ((!socket) || (socket->state() != QAbstractSocket::ConnectedState)) {
    emit client_error();
    return;
  }

  // Step 1: receive size of data
  if (!sizeReceived) {
    if (static_cast<unsigned int>(socket->bytesAvailable()) < sizeof(int))
      return;
    else {
      QDataStream data_in(socket);
      data_in >> data_size;
      sizeReceived = true;
    }
  }

  // Step 2: receive data
  if (sizeReceived) {
    // Socket not exist or disconnected
    if ((!socket) || (socket->state() != QAbstractSocket::ConnectedState)) {
      emit client_error();
      return;
    } else {
      QDataStream data_in(socket);
      data_in >> recv_pack;
      data_size = 0;
      sizeReceived = false;
    }

    emit data_received(); // Send data received signal

    // Continue receiving if data remain in socket
    if (socket->bytesAvailable())
      receive();
  }
}

CommPack str2pack(QString str, int opt) {
  CommPack pack;
  pack.type = opt;
  pack.desc = get_current_time(TIME_FULL);
  pack.byteData = str.toUtf8();
  return pack;
}

CommPack image2pack(QPixmap image) {
  CommPack pack;
  pack.type = DATA_IMAGE;
  pack.desc = get_current_time(TIME_FULL);
  QBuffer buf(&pack.byteData);
  buf.open(QIODevice::ReadWrite);
  image.save(&buf, "JPG", 10); // Set to lower quality to ensure speed
  return pack;
}

CommPack resist2pack(ResistData data) {
  CommPack pack;
  pack.type = DATA_RESIST;
  pack.desc = get_current_time(TIME_FULL);
  pack.byteData = data.merge().toUtf8();
  return pack;
}
