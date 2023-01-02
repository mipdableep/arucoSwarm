#ifndef DRONE_CLIENT_H_
#define DRONE_CLIENT_H_

#include <netinet/in.h>

#include <condition_variable>
#include <mutex>
#include <string>

class DroneClient {
    std::string drone_name;
    std::string server_ip;
    int port;

    int client_socket = -1;
    bool connected = false;
    std::condition_variable cv;
    std::mutex cv_m;

    static const std::string takeoff_msg;

   public:
    DroneClient(const std::string& drone_name, const std::string& server_ip,
                int port);
    ~DroneClient();
    bool connect_to_server();

    void wait_for_takeoff();
};

#endif  // DRONE_CLIENT_H_
