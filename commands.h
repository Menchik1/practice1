#pragma once

#include <iostream>
#include "structures.h"
#include "json.hpp"
#include "locker.h"
#include "fileOperations.h"

using namespace std;

void insert(dbase& db, const string& table, json entry);
void select(dbase& db, const string& column, const string& table, const Pars<string, string> filters[], int filter_count, const string& filter_type);
void Delete(dbase& db, const string& column, const string& value, const string& table);
bool applyAndFilters(const json& entry, const Pars<string, string> filters[], int filter_count);
bool applyOrFilters(const json& entry, const Pars<string, string> filters[], int filter_count);
