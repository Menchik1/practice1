#include "commands.hpp"
#include<iostream>
#include <fstream>
#include "json.hpp"

void insert(dbase& db, const string& table, json entry) {
    Node* table_node = db.findNode(table);
    if (table_node) {
        updatePrimaryKey(db); 
        entry["id"] = db.current_pk; 

        table_node->data.addEnd(entry.dump());
        cout << "Inserted: " << entry.dump() << endl;

        saveSingleEntryToCSV(db, table, entry);
    } else {
        cout << "Table not found: " << table << endl;
    }
}

void select(dbase& db, const string& column, const string& table, const pair<string, string> filters[], int filter_count, const string& filter_type) {
    Node* table_node = db.findNode(table);
    if (table_node) {
        bool data_found = false;
        for (size_t i = 0; i < table_node->data.getSize(); ++i) {
            json entry = json::parse(table_node->data.get(i));
            bool valid_row = false;

            if (filter_type == "AND") {
                valid_row = applyAndFilters(entry, filters, filter_count);
            } else if (filter_type == "OR") {
                valid_row = applyOrFilters(entry, filters, filter_count);
            }

            if (valid_row) {
                data_found = true;
                if (column == "all") {
                    cout << "Data from table " << table << ": ";
                    cout << entry["name"].get<string>() << ", "
                         << entry["age"].get<int>() << ", "
                         << entry["adress"].get<string>() << ", "
                         << entry["number"].get<string>() << endl;
                } else if (entry.contains(column)) {
                    cout << "Data from " << column << " in " << table << ": ";
                    cout << entry[column].get<string>() << endl;
                } else {
                    cout << "Error: Column '" << column << "' does not exist in table '" << table << "'." << endl;
                }
            }
        }
        if (!data_found) {
            cout << "No such data found in the table." << endl;
        }
    } else {
        cout << "Table not found: " << table << endl;
    }
}

void Delete(dbase& db, const string& column, const string& value, const string& table) {
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
            cout << "Row with " << column << " = " << value << " not found in table " << table << endl;
        }
    } else {
        cout << "Table not found: " << table << endl;
    }
}

bool applyAndFilters(const json& entry, const pair<string, string> filters[], int filter_count) {
    for (int i = 0; i < filter_count; ++i) {
        const string& filter_column = filters[i].first;
        const string& filter_value = filters[i].second;

        if (!entry.contains(filter_column) || entry[filter_column].get<string>() != filter_value) {
            return false;
        }
    }
    return true;
}

bool applyOrFilters(const json& entry, const pair<string, string> filters[], int filter_count) {
    for (int i = 0; i < filter_count; ++i) {
        const string& filter_column = filters[i].first;
        const string& filter_value = filters[i].second;

        if (entry.contains(filter_column) && entry[filter_column].get<string>() == filter_value) {
            return true;
        }
    }
    return false;
}
