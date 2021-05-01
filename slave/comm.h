#ifndef COMM_H
#define COMM_H

#include "util.h"

#define HOST_ADDR "192.168.137.10"
#define SLAVE_ADDR "192.168.137.32"
#define PORT 2333

#define SYS_INFO 0    // System information
#define CYCLE 1       // Sampling period
#define HEARTBEAT 2   // Heartbeat pack
#define DATA_RECV 3   // Received flag
#define DATA_RESIST 4 // Resistance value
#define DATA_IMAGE 5  // Image
#define DATA_STR 6    // String
#define REQUEST 7     // Data request

#define CONNECTED 0
#define DISCONNECTED 1
#define DISABLED 2

#define MAX_TRY 1
#define MAX_MISS 5

#define HEARTBEAT_SEND 1000
#define HEARTBEAT_RECV 3000

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
    friend QDataStream &operator<<(QDataStream &data_out,
                                   const CommPack &pack) {
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

    QTcpSocket *socket; // TCP socket

    int net_state; // Status of client

    unsigned int trying; // Times of trying to connect to server
    unsigned int missed; // Missed packages (when missed > MAX_MISS, client set
                         // to disabled)

    int data_size;
    bool sizeReceived;

    CommPack recv_pack, send_pack, hbt_pack; // Packs

    QTimer *tm_hbt_send, *tm_hbt_recv; // Timer for heartbeat detection

    Comm(QString host_addr = HOST_ADDR, QString slave_addr = SLAVE_ADDR);

    bool init_net();    // Initialize network connection
    void disable_net(); // Disable network connection
    bool check_hbt();   // Check heartbeat reply

    // Customized signals for UI class
  signals:
    void client_connected();
    void client_disconnected();
    void client_disabled();
    void client_error();
    void data_received();
    void data_sent();

  public slots:
    void send();
    void send_hbt();
    void hbt_missed();
    void receive();
};

// Convert multiple data formats to pack
CommPack str2pack(QString str, int opt = SYS_INFO);
CommPack image2pack(QPixmap image);
CommPack resist2pack(ResistData data);

#endif // COMM_H
