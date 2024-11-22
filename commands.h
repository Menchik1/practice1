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

using namespace std;
using json = nlohmann::json;

bool applyFilter(const json& entry, const Pars<string, string>& filter);
void selectFromMultipleTables(dbase& db, const string& column, const string& column2, const string& table1, const string& table2, const Pars<string, string>& filter1, const Pars<string, string>& filter2, const string& WHERE, const string& filter_type, const string& tablef, stringstream& result_stream);
void selectFromTable(dbase& db, const string& table, const Pars<string, string>& filter);
void insert(dbase& db, const string& table, json entry);
void deleteRow(dbase& db, const string& column, const string& value, const string& table);
