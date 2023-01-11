//
// Created by amit on 10/2/22.
//

#include <stdio.h>
#include <string>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <cerrno> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <optional>

#ifndef NOLIB_SERIALTELLO_H
#define NOLIB_SERIALTELLO_H


class SerialTello {
private:
    std::string dev;
    int serial_port;

public:
    explicit SerialTello(const std::string &dev);

    std::optional<std::string> SendCommandWithResponse(const std::string& command);
    void SendCommand(const std::string& command);
    std::optional<std::string> GetResponse();
};


#endif //NOLIB_SERIALTELLO_H
