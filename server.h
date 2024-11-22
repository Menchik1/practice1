#pragma once
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

using namespace std;
using json = nlohmann::json;

void handleClient(int client_socket, dbase& db);
