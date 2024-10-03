#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>

using namespace std;

// Global containers for user and group data
std::map<std::string, std::string> user_credentials;   // User ID -> Password
std::map<std::string, std::vector<std::string>> group_members;  // Group ID -> List of members
std::map<std::string, std::string> group_owners;  // Group ID -> Owner (User ID)
std::map<std::string, std::vector<std::string>> pending_requests;  // Group ID -> List of pending join requests
std::mutex tracker_mutex;  // Mutex for synchronizing access to data

// Helper function to send a response to the client
void send_response(int client_socket, const std::string& response) {
    write(client_socket, response.c_str(), response.size());
}

// Helper function to trim whitespace (including newlines) from the beginning and end of a string
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \n\r\t");
    size_t end = str.find_last_not_of(" \n\r\t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to handle incoming requests
void handle_client(int client_socket) {
    char buffer[1024];
    int n;

    while ((n = read(client_socket, buffer, 1024)) > 0) {
        buffer[n] = '\0';  // Null terminate the buffer

        std::string request(buffer);
        request = trim(request);  // Trim leading and trailing whitespace, including newlines

        // Tokenize the request
        std::vector<std::string> tokens;
        size_t pos = 0;
        while ((pos = request.find(' ')) != std::string::npos) {
            tokens.push_back(request.substr(0, pos));
            request.erase(0, pos + 1);
        }
        tokens.push_back(request);  // Add the last token

        // Process the command
        std::lock_guard<std::mutex> lock(tracker_mutex);  // Protect shared data

        if (tokens[0] == "create_user") {
            if (tokens.size() == 3) {
                std::string user_id = tokens[1];
                std::string password = tokens[2];
                user_credentials[user_id] = password;
                send_response(client_socket, "User created successfully\n");
            } else {
                send_response(client_socket, "Usage: create_user <user_id> <password>\n");
            }
        } else if (tokens[0] == "login") {
            send_response(client_socket, "Login successful\n");
        } else if (tokens[0] == "create_group") {
            send_response(client_socket, "Group created successfully\n");
        } else {
            send_response(client_socket, "Unknown command\n");
        }
    }

    close(client_socket);  // Close the client connection after processing
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./tracker tracker_info.txt tracker_no" << std::endl;
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in trackerAddress;
    trackerAddress.sin_family = AF_INET;
    trackerAddress.sin_port = htons(8080);  // Example hardcoded port for this tracker
    trackerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&trackerAddress, sizeof(trackerAddress)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    std::cout << "Tracker running on port 8080" << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_size);

        if (client_socket < 0) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();  // Handle the client in a separate thread
    }

    close(server_socket);
    return 0;
}
