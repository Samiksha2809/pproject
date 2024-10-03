#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Function to connect to the tracker
int connect_to_tracker(const std::string& ip, int port) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in tracker_addr;
    tracker_addr.sin_family = AF_INET;
    tracker_addr.sin_port = htons(port);
    tracker_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_socket, (sockaddr*)&tracker_addr, sizeof(tracker_addr)) < 0) {
        std::cerr << "Failed to connect to tracker" << std::endl;
        close(client_socket);
        return -1;
    }
    return client_socket;
}

// Function to send a command to the tracker and get a response
void send_command(int client_socket, const std::string& command) {
    write(client_socket, command.c_str(), command.length());
    char buffer[1024];
    int n = read(client_socket, buffer, 1024);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Response: " << buffer << std::endl;
    } else {
        std::cerr << "No response from server" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./client <IP>:<PORT>" << std::endl;
        return 1;
    }

    std::string tracker_address = argv[1];
    size_t colon_pos = tracker_address.find(':');
    std::string ip = tracker_address.substr(0, colon_pos);
    int port = std::stoi(tracker_address.substr(colon_pos + 1));

    int client_socket = connect_to_tracker(ip, port);
    if (client_socket < 0) {
        return 1;
    }

    // Example interaction
    std::string command;

    while (true) {
        std::cout << "Enter command (or 'quit' to exit): ";
        std::getline(std::cin, command);

        if (command == "quit") {
            break;
        }

        send_command(client_socket, command + "\n");
    }

    close(client_socket);
    return 0;
}
