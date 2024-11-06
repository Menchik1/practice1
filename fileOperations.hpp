#ifndef FILEOPERATIONS_HPP
#define FILEOPERATIONS_HPP

#include <iostream>
#include <fstream>
#include "structures.hpp"
#include "json.hpp"

using namespace std;

void loadSchema(dbase& db, const string& schema_file);
void createDirectories(dbase& db, const json& structure);
void saveSingleEntryToCSV(dbase& db, const string& table, const json& entry);
void rewriteCSV(dbase& db, const string& table);

#endif 
