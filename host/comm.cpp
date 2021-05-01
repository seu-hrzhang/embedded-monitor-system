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

  server = new QTcpServer;
  socket = new QTcpSocket;

  connect(server, SIGNAL(newConnection()), this, SLOT(connected()));
}

void Comm::init_net() {
  server->listen(QHostAddress::Any, PORT);
  // server->listen(QHostAddress(SLAVE_ADDR), PORT); // Abandoned
}

void Comm::disable_net() {
  server->close();
  socket->close();
  emit server_disabled();
}

// Slot functions
void Comm::connected() {
  emit server_connected();
  socket = server->nextPendingConnection();
  if (socket) {
    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
    emit socket_succeed();
  }
}

void Comm::send() {
  // Socket not exist or disconnected
  if ((!socket) || (socket->state() != QAbstractSocket::ConnectedState)) {
    emit server_error();
    return;
  }
  QDataStream data_out(socket);
  data_out << send_pack.get_size() << send_pack;
  socket->flush();

  emit data_sent(); // Send data sent signal
}

void Comm::receive() {
  // Socket not exist or disconnected
  if ((!socket) || (socket->state() != QAbstractSocket::ConnectedState)) {
    emit server_error();
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
      emit server_error();
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
  if (opt == SYS_INFO)
    pack.desc = "System Info";
  else if (opt == CYCLE)
    pack.desc = "Cycle Value";
  else
    pack.desc = "String";
  pack.byteData = str.toUtf8();
  return pack;
}

CommPack image2pack(QPixmap image) {
  CommPack pack;
  pack.type = DATA_IMAGE;
  pack.desc = "Image";
  QBuffer buf(&pack.byteData);
  buf.open(QIODevice::ReadWrite);
  image.save(&buf, "JPG", 50); // Set to lower quality to ensure speed
  return pack;
}

CommPack resist2pack(ResistData data) {
  CommPack pack;
  pack.type = DATA_RESIST;
  pack.desc = "Resistance";
  pack.byteData = data.merge().toUtf8();
  return pack;
}
