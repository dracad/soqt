#ifndef SERIAL_READER_H
#define SERIAL_READER_H

#include <QObject>
#include <QDebug>

#include <QTimer>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <poll.h> //polling serial port

namespace Serial {

static const QList<QString> allowedBitsPerByte = {"5", "6", "7", "8"};

struct Settings // default values are most common settings
{
    // C-Flags (Contol modes)
    bool enableParity_  = false;
    bool parityEven_    = true;
    bool use2StopBits_  = false;
    bool flowControl_   = false;
    bool hangUp_        = false;
    bool CRead_         = true;
    bool CLocal_        = true;
    QString bitsPerByte = "8";

    // I-Flags (Local Modes)
    bool cannonMode_    = false;
    bool ISig_          = false;
    bool echo_          = false;
    bool erasure_       = false;
    bool newLineEcho_   = false;

    // i-Flags (Input modes)
    bool swFlowControl_ = false;
    bool specialHandle_ = false;

    // O-Flags (Output modes)
    bool NLCR_          = false;
    bool outInterpret_  = false;
};
}

class serial_reader : public QObject
{
    Q_OBJECT
public:
    explicit serial_reader(QObject *parent = nullptr);
    explicit serial_reader(const char* port, int baud, const Serial::Settings &settings, QObject *parent = nullptr);
    explicit serial_reader(const int file_handle, QObject *parent = nullptr);
    ~serial_reader();

signals:
    void SerialIn(const QByteArray &line);
    void connected(const QString &line, const int baud);
    void failed(const QString &line, const int baud);

public slots:
    void work();
    void write(const QByteArray &line);

private:
    int wait_on_serial(char *buff, size_t buf_size, int port);
    int open_port(const char* port, int baud);

    void get_serial(int port);

    static speed_t num_to_baud(int num);

    int m_serial_port;
    Serial::Settings settings_;
};

#endif // SERIAL_READER_H
