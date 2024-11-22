#include <iostream>
#include <fstream>
#include <sys/stat.h> // Для mkdir
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring> // Для memset
#include "json.hpp" // Библиотека для работы с JSON
#include <sstream> // Для istringstream
#include "structures.h"
#include "locker.h"
#include "WorkWithFiles.h"

using namespace std;
using json = nlohmann::json;

void lockPrimaryKey(dbase& db) {
    try {
        string pk_filename = db.schema_name + "/table_pk_sequence.txt";
        ofstream pk_file(pk_filename);
        if (pk_file) {
            pk_file << db.current_pk << "\nlocked";
        } else {
            throw runtime_error("Failed to lock primary key file: " + pk_filename);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void unlockPrimaryKey(dbase& db) {
    try {
        string pk_filename = db.schema_name + "/table_pk_sequence.txt";
        ofstream pk_file(pk_filename);
        if (pk_file) {
            pk_file << db.current_pk << "\nunlocked";
        } else {
            throw runtime_error("Failed to unlock primary key file: " + pk_filename);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void initializePrimaryKey(dbase& db) {
    try {
        string pk_filename = db.schema_name + "/table_pk_sequence.txt";
        ifstream pk_file(pk_filename);
        
        if (pk_file) {
            pk_file >> db.current_pk;
        } else {
            db.current_pk = 0;
            ofstream pk_file_out(pk_filename);
            if (pk_file_out) {
                pk_file_out << db.current_pk << "\nunlocked";
            } else {
                throw runtime_error("Failed to create file: " + pk_filename);
            }
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void updatePrimaryKey(dbase& db) {
    try {
        string pk_filename = db.schema_name + "/table_pk_sequence.txt";

        ifstream pk_file(pk_filename);
        if (pk_file) {
            pk_file >> db.current_pk;
        } else {
            db.current_pk = 0;
        }
        pk_file.close();

        db.current_pk++;

        ofstream pk_file_out(pk_filename);
        if (pk_file_out) {
            pk_file_out << db.current_pk << "\nlocked";
        } else {
            throw runtime_error("Failed to open file for updating: " + pk_filename);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}
