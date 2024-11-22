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

void loadSchema(dbase& db, const string& schema_file);
void createDirectories(dbase& db, const json& structure);
void saveSingleEntryToCSV(dbase& db, const string& table, const json& entry);
void rewriteCSV(dbase& db, const string& table);
