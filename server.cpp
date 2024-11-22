#include <iostream>
#include <fstream>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring> 
#include "json.hpp" 
#include <sstream> 
#include "structures.h"
#include "locker.h"
#include "commands.h"
#include "server.h"
#include <mutex>

using namespace std;
using json = nlohmann::json;

mutex dataM;

void Request(int client_socket, BaseData& bd) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            break;
        }
        string query(buffer);
        istringstream iss(query);
        string action;
        iss >> action;

        try {
            if (action == "INSERT") {
                string table;
                iss >> table;

                Array args;
                string arg;

                while (iss >> arg) {
                    args.addEnd(arg);
                }

                size_t expected_arg_count = bd.getColumnCount(table);
                if (args.getSize() > expected_arg_count) {
                    string error_message = "Error: Too many arguments for INSERT command.";
                    send(client_socket, error_message.c_str(), error_message.size(), 0);
                    continue;
                } else if (args.getSize() < 2) {
                    string error_message = "Error: Not enough arguments for INSERT command.";
                    send(client_socket, error_message.c_str(), error_message.size(), 0);
                    continue;
                }

                json entry = {
                    {"name", args.get(0)},
                    {"age", stoi(args.get(1))}
                };

                if (args.getSize() > 2) {
                    entry["adress"] = args.get(2);
                } else {
                    entry["adress"] = ""; 
                }

                if (args.getSize() > 3) {
                    entry["number"] = args.get(3);
                } else {
                    entry["number"] = "";
                }
                {
                    lock_guard<mutex> lock(dataM);
                    insert(bd, table, entry);
                }
                string success_message = "Data successfully inserted.";
                send(client_socket, success_message.c_str(), success_message.size(), 0);

            } else if (action == "SELECT") {
                string column, column2, from, tables;
                iss >> from >> tables;

                if (tables == "tables:") {
                    string table1, and_word, table2, WHERE, filter_column1, filter_value1, OPER, filter_column2, tablef, filter_value2;
                    iss >> table1 >> column >> and_word >> table2 >> column2 >> WHERE >> tablef >> filter_column1 >> filter_value1 >> OPER >> filter_column2 >> filter_value2;

                    Pars<string, string> filter1(filter_column1, filter_value1);
                    Pars<string, string> filter2(filter_column2, filter_value2);

                    stringstream result_stream;
                    {
                    lock_guard<mutex> lock(dataM);
                    selectFromMultipleTables(bd, column, column2, table1, table2, filter1, filter2, WHERE, OPER, tablef,result_stream);
                    }
                    string result = result_stream.str();
                    send(client_socket, result.c_str(), result.size(), 0);

                } else {
                    string table = tables;
                    stringstream result_stream;
                    {
                    lock_guard<mutex> lock(dataM);
                    selectFromTable(bd, table, {"", ""}); 
                    }
                    string result = result_stream.str();
                    send(client_socket, result.c_str(), result.size(), 0);
                }

            } else if (action == "DELETE") {
                string column, from, value, table;
                iss >> from >> table >> column >> value;
                {
                    lock_guard<mutex> lock(dataM);
                    deleteRow(bd, column, value, table);
                }
                string success_message = "Row deleted successfully.";
                send(client_socket, success_message.c_str(), success_message.size(), 0);
                
            } else {
                string error_message = "Unknown command: " + query;
                send(client_socket, error_message.c_str(), error_message.size(), 0);
            }
        } catch (const exception& e) {
            string error_message = "Error: " + string(e.what());
            send(client_socket, error_message.c_str(), error_message.size(), 0);
        }
    }
    close(client_socket);
}
