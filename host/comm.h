#ifndef COMM_H
#define COMM_H

#include "util.h"

#define HOST_ADDR "192.168.137.10"
#define SLAVE_ADDR "192.168.137.32"
#define PORT 2333

#define SYS_INFO 0
#define CYCLE 1
#define HEARTBEAT 2
#define DATA_RECV 3
#define DATA_RESIST 4
#define DATA_IMAGE 5
#define DATA_STR 6
#define REQUEST 7

using namespace std;

// Class of data to be transmitted in communication
class CommPack {
public:
  int type;            // Type of data (full/resist/image)
  QString desc;        // Description of data pack
  QByteArray byteData; // Contents to send or receive

  CommPack();

  // Overload '=' operation
  CommPack &operator=(const CommPack &other) {
    type = other.type;
    desc = other.desc;
    byteData = QByteArray(other.byteData);
    return *this;
  };

  int get_size(); // Get size of comm pack

  // Convert byte data to struct or image
  QString cvt2str();
  ResistData cvt2resist();
  QPixmap cvt2image();

  // Streaming struct data
#ifndef QT_NO_DATASTREAM
  friend QDataStream &operator<<(QDataStream &data_out, const CommPack &pack) {
    data_out << pack.type << pack.desc << pack.byteData;
    return data_out;
  }

  friend QDataStream &operator>>(QDataStream &data_in, CommPack &pack) {
    data_in >> pack.type >> pack.desc >> pack.byteData;
    return data_in;
  }
#endif
};

// Class of communication
class Comm : public QObject {
  Q_OBJECT
public:
  QString host_addr;  // IP address of host computer
  QString slave_addr; // IP address of slave computer

  QTcpServer *server; // TCP server
  QTcpSocket *socket; // TCP socket

  int data_size;
  bool sizeReceived;

  CommPack recv_pack, send_pack;

  Comm(QString host_addr = HOST_ADDR, QString slave_addr = SLAVE_ADDR);

  void init_net();    // Initialize network connection
  void disable_net(); // Disable network connection

  // Customized signals for UI class
signals:
  void server_connected();
  void server_disabled();
  void server_error();
  void socket_succeed();
  void socket_error();
  void data_received();
  void data_sent();

public slots:
  void connected();
  void send();
  void receive();
};

// Convert multiple data formats to pack
CommPack str2pack(QString str, int opt = SYS_INFO);
CommPack image2pack(QPixmap image);
CommPack resist2pack(ResistData data);

#endif // COMM_H
