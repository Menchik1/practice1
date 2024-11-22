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

bool applyFilter(const json& entry, const Pars<string, string>& filter) {
    const string& filter_column = filter.first; 
    const string& filter_value = filter.second;

    return entry.contains(filter_column) && entry[filter_column].get<string>() == filter_value;
}

void selectFromMultipleTables(dbase& db, const string& column, const string& column2, const string& table1, const string& table2, const Pars<string, string>& filter1, const Pars<string, string>& filter2, const string& WHERE, const string& filter_type, const string& tablef, stringstream& result_stream) {
    string column1 = column; 
    bool data_found = false;

    if (column2 != "") {         
        Node* table_node1 = db.findNode(table1);
        Node* table_node2 = db.findNode(table2);
        if (!table_node1 || !table_node2) {
            result_stream << "One or both tables not found: " << table1 << ", " << table2 << endl;
            return;
        }

        if (WHERE == "WHERE") {
            if (filter_type != "") {
                for (size_t i = 0; i < table_node1->data.getSize(); ++i) {
                    json entry1 = json::parse(table_node1->data.get(i));

                    for (size_t j = 0; j < table_node2->data.getSize(); ++j) {
                        json entry2 = json::parse(table_node2->data.get(j));

                        bool filter1_passed = applyFilter(entry1, filter1);
                        bool filter2_passed = applyFilter(entry2, filter2);

                        if ((filter_type == "AND" && filter1_passed && filter2_passed) ||
                            (filter_type == "OR" && (filter1_passed || filter2_passed))) {
                            if (entry1.contains(column1) && entry2.contains(column2)) {
                                result_stream << entry1[column1].get<string>() << ", " << entry2[column2].get<string>() << endl;
                                data_found = true;
                            }
                        }
                    }
                }
            } else {
                for (size_t i = 0; i < table_node1->data.getSize(); ++i) {
                    json entry1 = json::parse(table_node1->data.get(i));

                    for (size_t j = 0; j < table_node2->data.getSize(); ++j) {
                        json entry2 = json::parse(table_node2->data.get(j));

                        if (tablef == "table1") {
                            if (applyFilter(entry1, filter1)) {
                                if (entry1.contains(column1) && entry2.contains(column2)) {
                                    result_stream << entry1[column1].get<string>() << ", " << entry2[column2].get<string>() << endl;
                                    data_found = true;
                                }   
                            }
                        } else {
                            if (applyFilter(entry2, filter1)) {
                                if (entry1.contains(column1) && entry2.contains(column2)) {
                                    result_stream << entry1[column1].get<string>() << ", " << entry2[column2].get<string>() << endl;
                                    data_found = true;
                                }   
                            }
                        }
                    }
                }
            }
        } else {
            for (size_t i = 0; i < table_node1->data.getSize(); ++i) {
                json entry1 = json::parse(table_node1->data.get(i));

                for (size_t j = 0; j < table_node2->data.getSize(); ++j) {
                    json entry2 = json::parse(table_node2->data.get(j));

                    if (entry1.contains(column1) && entry2.contains(column2)) {
                        result_stream << entry1[column1].get<string>() << ", " << entry2[column2].get<string>() << endl;
                        data_found = true;
                    }
                }
            }
        }
    } else {
        result_stream << "Error: Missing column names for CROSS JOIN." << endl;
        return;
    }

    if (!data_found) {
        result_stream << "No data found in the cross join of " << table1 << " and " << table2 << endl;
    }
}

void selectFromTable(dbase& db, const string& table, const Pars<string, string>& filter) {
    Node* table_node = db.findNode(table);
    
    if (!table_node) {
        cout << "Table not found: " << table << endl;
        return;
    }

    bool data_found = false;
    for (size_t i = 0; i < table_node->data.getSize(); ++i) {
        json entry = json::parse(table_node->data.get(i));
        if (applyFilter(entry, filter)) {
            data_found = true; // We found at least one entry

            // Print the entry in the desired format
            cout << "name: \"" << entry["name"].get<string>() << "\", "
                 << "age: \"" << entry["age"].get<string>() << "\", "
                 << "adress: \"" << entry["adress"].get<string>() << "\", "
                 << "number: \"" << entry["number"].get<string>() << "\";" << endl;
        }
    }

    if (!data_found) {
        cout << "No data found in the table: " << table << endl;
    }
}

void insert(dbase& db, const string& table, json entry) {
    Node* table_node = db.findNode(table);
    if (table_node) {
        updatePrimaryKey(db); 
        entry["id"] = db.current_pk; 

        table_node->data.addEnd(entry.dump());

        saveSingleEntryToCSV(db, table, entry);
    } else {
        cout << "Table not found: " << table << endl;
    }
}
void deleteRow(dbase& db, const string& column, const string& value, const string& table) {
    Node* table_node = db.findNode(table);
    if (table_node) {
        Array new_data;
        bool found = false;

        for (size_t i = 0; i < table_node->data.getSize(); ++i) {
            json entry = json::parse(table_node->data.get(i));
            if (entry.contains(column) && entry[column].get<string>() == value) {
                found = true;
                cout << "Deleted row: " << entry.dump() << endl;
            } else {
                new_data.addEnd(table_node->data.get(i));
            }
        }

        if (found) {
            table_node->data = new_data;
            rewriteCSV(db, table);
        } else {
            cout << "Row with " << column << " = " << value << " not found in " << table << endl;
        }
    } else {
        cout << "Table not found: " << table << endl;
    }
}
