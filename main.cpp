#include <iostream>
#include <fstream>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <cstring> 
#include "json.hpp" 
#include <sstream> 
#include "structures.h"
#include "locker.h"
#include "commands.h"
#include "server.h"
#include "WorkWithFiles.h"

using namespace std;
using json = nlohmann::json;

int main() {
    dbase db;
    loadSchema(db, "schema.json");
    db.load();

    int server_socket;
    struct sockaddr_in server_addr;

    // Создание сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error creating socket." << endl;
        return 1;
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(7432);

    // Привязка сокета к адресу
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket." << endl;
        close(server_socket);
        return 1;
    }

    // Ожидание подключения клиентов
    listen(server_socket, 5);
    cout << "Server listening on 7432 "  << "..." << endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            cerr << "Error accepting connection." << endl;
            continue;
        }
        cout << "Client connected." << endl;

        // Создаем новый поток для обработки клиента
        thread client_thread(handleClient, client_socket, ref(db));
        client_thread.detach(); // Отделяем поток, чтобы он мог работать независимо
    }

    close(server_socket);
    return 0;
}
