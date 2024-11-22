#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void communicateWithServer(int clientSocket) {
    string command;

    while (true) {
        cout << "Enter command (INSERT, DELETE, SELECT, EXIT): ";
        getline(cin, command);

        if (command == "EXIT") {
            send(clientSocket, "EXIT", 4, 0);
            cout << "Close connection \n";
            break;
        }
        if (command.empty()) continue;

        send(clientSocket, command.c_str(), command.length(), 0);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            cout << "Answer server: " << endl << buffer << endl;
        } else if (bytesRead == 0) {
            cout << "Close connection\n";
            break;
        } else {
            cerr << "Error get data from server\n";
        }
    }

    close(clientSocket);
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Failed to create a socket\n";
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons(7432);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Server connection error\n";
        close(clientSocket);
        return 1;
    }

    thread clientThread(communicateWithServer, clientSocket);

    clientThread.join();

    close(clientSocket);
    return 0;
}
