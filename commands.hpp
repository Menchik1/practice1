#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include "structures.hpp"
#include "json.hpp"
#include "locker.hpp"
#include "fileOperations.hpp"

using namespace std;

void insert(dbase& db, const string& table, json entry);
void select(dbase& db, const string& column, const string& table, const pair<string, string> filters[], int filter_count, const string& filter_type);
void Delete(dbase& db, const string& column, const string& value, const string& table);
bool applyAndFilters(const json& entry, const pair<string, string> filters[], int filter_count);
bool applyOrFilters(const json& entry, const pair<string, string> filters[], int filter_count);

#endif 
