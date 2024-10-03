#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

unordered_map<string,string> login_info;

using namespace std;



int main(int argc, char* argv[]){
    string line;
   vector<pair<string,int>> tracker_list;
   int tracker_no = stoi(argv[2]);

    if( argc != 3){
    cout << "Sufficient arguments not provided. Give tracker info file name and tracker no.";
    return -1;
    }
  
  ifstream file(argv[1]);
  if(!file){
    cout << "File could not be opened";
    return 1;
  }
  string ip;
  int port;

  while(file >> ip >> port){
    tracker_list.push_back({ip, port});
  }

int trackerSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in trackerAddress;
    trackerAddress.sin_family = AF_INET;
    trackerAddress.sin_port = htons(tracker_list[tracker_no].second);
    trackerAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(trackerSocket, (struct sockaddr*)&trackerAddress,
         sizeof(trackerAddress));

    // listening to the assigned socket
    listen(trackerSocket, 5);

    // accepting connection request
    int clientSocket
        = accept(trackerSocket, nullptr, nullptr);

    // recieving data
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Message from client: " << buffer
              << endl;

    // closing the socket.
    close(trackerSocket);



while(true){
  sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_socket = accept(trackerSocket, (sockaddr*)&client_addr, &addr_size);
 char buffer[1024];
    int n;
    while ((n = read(client_socket, buffer, 1024)) > 0) {
        buffer[n] = '\0';
        std::string request(buffer);

       
        if (request.find("create_user") == 0) {
         
        } else if (request.find("login") == 0) {
           
        } else if (request.find("create_group") == 0) {
           
        }
       
    }



}






    return 0;
}