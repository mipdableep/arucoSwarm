//
// Created by amit on 10/2/22.
//

#include <cstring>
#include "../include/SerialTello.h"

SerialTello::SerialTello(const std::string &dev) : dev(dev) {
    serial_port = open("/dev/ttyUSB0", O_RDWR); //TODO: this should open the devicein the dev parameter instead of always "/dev/ttyUSB0"

    // Check for errors
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return;
    }

    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return;
    }
}

std::optional<std::string>  SerialTello::SendCommandWithResponse(const std::string &command) {
    SendCommand(command);
    return GetResponse();
}

void SerialTello::SendCommand(const std::string &command) {
    auto msg = command.c_str();
    write(serial_port, msg, command.size());
}

std::optional<std::string> SerialTello::GetResponse() {
    // Allocate memory for read buffer, set size according to your needs
    char read_buf [256];
    char read_buf_2 [256];

    // Normally you wouldn't do this memset() call, but since we will just receive
    // ASCII data for this example, we'll set everything to 0 so we can
    // call printf() easily.
    memset(&read_buf, '\0', sizeof(read_buf));

    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME
    int num_bytes = -1;
    for (int i = 0; i < 6; i++){
        if (num_bytes = read(serial_port, &read_buf, sizeof(read_buf)); num_bytes > 0)
            break;
    }

    // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
    if (num_bytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return std::optional<std::string>();
    }
    else if (num_bytes == 0){
        printf("Timeout reading\n");
        return std::optional<std::string>();
    }
    else{
        memset(&read_buf_2, '\0', sizeof(read_buf_2));
        int num_bytes_2 = read(serial_port, &read_buf_2, sizeof(read_buf));
        while(num_bytes_2 > 0){
            for(int i = 0; i < num_bytes_2; i++){
                read_buf[num_bytes+i] = read_buf_2[i];
            }
            num_bytes+=num_bytes_2;
            memset(&read_buf_2, '\0', sizeof(read_buf_2));
            num_bytes_2 = read(serial_port, &read_buf_2, sizeof(read_buf));
        }


        // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
        // print it to the screen like this!)
        //printf("Read %i bytes. Received message: %s", num_bytes, read_buf);
        return std::optional<std::string>(read_buf);
    }
}
