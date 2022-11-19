#include "serial_reader.h"

serial_reader::serial_reader(QObject *parent) : QObject(parent)
{

}

serial_reader::serial_reader(const char *port, int baud, const Serial::Settings &settings, QObject *parent) : QObject(parent)
{
    settings_ = settings;
    m_serial_port = open_port(port, baud);

    if(m_serial_port == -1){
        QTimer::singleShot(1000, this, [this, port, baud](){emit failed(QString(port), baud);});

    }else{
        QString port_str(port);
        QTimer::singleShot(1000, this, [this, baud, port_str](){emit connected(port_str, baud);});
    }
}

serial_reader::serial_reader(const int file_handle, QObject *parent)
{
    m_serial_port = file_handle;
}

serial_reader::~serial_reader()
{
    if(m_serial_port != -1){
        close(m_serial_port);
    }
}

void serial_reader::work()
{
    get_serial(m_serial_port);
}

void serial_reader::write(const QByteArray &line)
{
    auto res = ::write(m_serial_port, line.data(), line.length());

    (void)res;
}

int serial_reader::wait_on_serial(char *buff, size_t buf_size, int port)
{
    pollfd fd;

    fd.fd = port;
    fd.events = POLLIN;

//    tcflush(opened_port, TCIFLUSH);
    int ret = poll(&fd, 1, 500); // timeout after 500ms
    if(ret == -1){
        qDebug() << "Error " << errno << " from poll: " << strerror(errno);
        return 0;
    }
    if(!ret || fd.revents == 32){
        return 0;
    }
    auto bytes_read = read(port, buff, buf_size);
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //tcflush(serial_port, TCIFLUSH);
    return bytes_read;
}

int serial_reader::open_port(const char *port, int baud)
{
    auto serial_port = open(port, O_RDWR );

    if(serial_port == -1){
        qDebug() << "Error " << errno << " from open: " << strerror(errno);
        return -1;
    }
    //  Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
        qDebug() << "Error " << errno << " from tcgetattr: " << strerror(errno);
        return -1;
    }

    //Set C-Flags
    if(settings_.enableParity_){
        tty.c_cflag |= PARENB; // enable parity
        if(!settings_.parityEven_){
            tty.c_cflag |= PARODD; // even parity
        }else{
            tty.c_cflag &= ~PARODD; // odd parity
        }
    }else{
        tty.c_cflag &= ~PARENB; // enable parity
    }

    if(settings_.use2StopBits_){ // 2 stop bits
        tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
    }else{
        tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    }

    if(settings_.flowControl_){
        tty.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control
    }else{
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    }

    if(settings_.hangUp_){ // hangup on close
        tty.c_cflag |= HUPCL;
    }else{
        tty.c_cflag &= ~HUPCL;
    }

    if(settings_.CLocal_){  // ignore modem control lines
        tty.c_cflag |= CLOCAL;
    }else{
        tty.c_cflag &= ~CLOCAL;
    }

    if(settings_.CRead_){ // Enable Receiver
        tty.c_cflag |= CREAD;
    }else{
        tty.c_cflag &= ~CREAD;
    }

    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then set bits per byte

    if(settings_.bitsPerByte == "5"){
        tty.c_cflag |= CS5; // 5 bits per byte
    }else if(settings_.bitsPerByte == "6"){
        tty.c_cflag |= CS6; // 6 bits per byte
    }else if(settings_.bitsPerByte == "7"){
        tty.c_cflag |= CS7; // 7 bits per byte
    }else if(settings_.bitsPerByte == "8"){
        tty.c_cflag |= CS8; // 8 bits per byte
    }


    // Local Mode Flags

    if(settings_.cannonMode_){
        tty.c_lflag |= ICANON; // enable cannonical mode
    }else{
        tty.c_lflag &= ~ICANON; // disable cannonical mode
    }

    if(settings_.ISig_){
        tty.c_lflag |= ISIG; // enable interpretation of INTR, QUIT and SUSP
    }else{
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    }

    if(settings_.echo_){
        tty.c_lflag |= ECHO;
    }else{
        tty.c_lflag &= ~ECHO; // Disable echo
    }

    if(settings_.erasure_){
        tty.c_lflag |= ECHOE;
    }else{
        tty.c_lflag &= ~ECHOE; // Disable erasure
    }

    if(settings_.newLineEcho_){
        tty.c_lflag |= ECHONL;
    }else{
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
    }

    if(settings_.swFlowControl_){
        tty.c_iflag |= (IXON | IXOFF | IXANY); // Turn on s/w flow ctrl
    }else{
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    }

    if(settings_.specialHandle_){
        tty.c_iflag |= (IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Enable any special handling of received bytes
    }else{
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    }

    if(settings_.outInterpret_){
       tty.c_oflag |= OPOST; // special interpretation of output bytes (e.g. newline chars)
    }else{
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    }

    if(settings_.NLCR_){
        tty.c_oflag |= ONLCR; // conversion of newline to carriage return/line feed
    }else{
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    }
    // // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    // tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    // tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be the set baud rate
    cfsetispeed(&tty, num_to_baud(baud));
    cfsetospeed(&tty, num_to_baud(baud));

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        qDebug() << "Error " << errno << " from tcsetattr: " << strerror(errno);
        return -1;
    }

    qDebug() << "Opened port " << port << " with baud " << baud;
    return serial_port;
}

void serial_reader::get_serial(int port)
{
    char buffer[256];
    std::string line;

    //wait\n");
    ::memset(buffer, 0, sizeof(buffer));

    int ret = wait_on_serial(buffer, sizeof (buffer), port);

    //printf("done\n");


    if(ret < 1){
        emit SerialIn(QByteArray());
        return;
    }

    buffer[ret] = '\0'; //strip the \r\n characters at the end of a line

    QByteArray buf(buffer, ret);

    emit SerialIn(buf);
}

speed_t serial_reader::num_to_baud(int num)
{
    switch (num) {
        case 0        : return B0;
        case 50       : return B50;
        case 75       : return B75;
        case 110	    : return B110;
        case 134	    : return B134;
        case 150	    : return B150;
        case 200	    : return B200;
        case 300	    : return B300;
        case 600	    : return B600;
        case 1200	    : return B1200;
        case 1800	    : return B1800;
        case 2400	    : return B2400;
        case 4800	    : return B4800;
        case 9600	    : return B9600;
        case 19200    : return B19200;
        case 38400    : return B38400;
        case 57600    : return B57600;
        case 115200   : return B115200;
        case 230400   : return B230400;
        case 460800   : return B460800;
        case 500000   : return B500000;
        case 576000   : return B576000;
        case 921600   : return B921600;
        case 1000000	: return B1000000;
        case 1152000	: return B1152000;
        case 1500000	: return B1500000;
        case 2000000	: return B2000000;
        case 2500000	: return B2500000;
        case 3000000	: return B3000000;
        case 3500000	: return B3500000;
        case 4000000	: return B4000000;
      }

    qDebug() << "Unrecognised baud rate " << num;

    return 0;
}
