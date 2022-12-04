#include "drone_client.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

const std::string DroneClient::takeoff_msg = "takeoff";

DroneClient::DroneClient(const std::string& drone_name,
                         const std::string& server_ip, int port)
    : drone_name(drone_name), server_ip(server_ip), port(port) {}

void DroneClient::wait_for_takeoff() {
    if (!connected) return;

    std::unique_lock<std::mutex> lk(cv_m);

    cv.wait(lk, [&] {
        char msg[1024] = {0};
        int recvd = recv(client_socket, msg, sizeof(msg), 0);

        return recvd == takeoff_msg.length() && std::string(msg) == takeoff_msg;
    });

    close(client_socket);
    connected = false;
}

bool DroneClient::connect_to_server() {
    struct hostent* host = gethostbyname(server_ip.c_str());
    sockaddr_in send_sock_addr = {0};
    send_sock_addr.sin_family = AF_INET;
    send_sock_addr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    send_sock_addr.sin_port = htons(port);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (!(connected = connect(client_socket, (sockaddr*)&send_sock_addr,
                              sizeof(send_sock_addr)) >= 0))
        return false;

    return send(client_socket, drone_name.c_str(), drone_name.length(), 0) ==
           drone_name.length();
}

DroneClient::~DroneClient() { close(client_socket); }
