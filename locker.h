#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "structures.h"

using namespace std;

void lockPrimaryKey(dbase& db);
void unlockPrimaryKey(dbase& db);
void initializePrimaryKey(dbase& db);
void updatePrimaryKey(dbase& db);

