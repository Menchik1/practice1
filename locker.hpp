#ifndef LOCKER_HPP
#define LOCKER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "structures.hpp"

using namespace std;

void lockPrimaryKey(dbase& db);
void unlockPrimaryKey(dbase& db);
void initializePrimaryKey(dbase& db);
void updatePrimaryKey(dbase& db);

#endif 
