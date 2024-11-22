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
#include "WorkWithFiles.h"

using namespace std;
using json = nlohmann::json;

void lockPrimaryKey(dbase& db);
void unlockPrimaryKey(dbase& db);
void initializePrimaryKey(dbase& db);
void updatePrimaryKey(dbase& db);
