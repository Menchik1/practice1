#include "fileOperations.hpp"
#include<iostream>
#include <fstream>
#include "json.hpp" 
#include <sys/stat.h>
void loadSchema(dbase& db, const string& schema_file) {
    try {
        ifstream file(schema_file);
        if (file) {
            json schema;
            file >> schema;
            db.schema_name = schema["name"];
            createDirectories(db, schema["structure"]);
            for (const auto& table : schema["structure"].items()) {
                db.addNode(table.key());
            }
        } else {
            throw runtime_error("Failed to open schema file.");
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void createDirectories(dbase& db, const json& structure) {
    try {
        _mkdir(db.schema_name.c_str());

        for (const auto& table : structure.items()) {
            string table_name = table.key();
            string table_path = db.schema_name + "\\" + table_name;

            _mkdir(table_path.c_str());
            db.filename = table_path + "\\1.csv";

            ifstream check_file(db.filename);
            if (!check_file) {
                ofstream file(db.filename);
                if (file.is_open()) {
                    auto& columns = table.value();
                    for (size_t i = 0; i < columns.size(); ++i) {
                        file << setw(10) << left << columns[i].get<string>() << (i < columns.size() - 1 ? ", " : "");
                    }
                    file << "\n";
                    file.close();
                }
            }
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void saveSingleEntryToCSV(dbase& db, const string& table, const json& entry) {
    try {
        string filename = db.schema_name + "\\" + table + "\\1.csv"; 
        ofstream file(filename, ios::app);
        if (file) {
            if (entry.contains("name") && entry.contains("age")) {
                file << setw(10) << left << entry["name"].get<string>() << ", "
                     << setw(10) << left << entry["age"];

                if (db.getColumnCount(table) > 2) {
                    file << ", " << setw(10) << left << entry["adress"].get<string>() << ", ";
                    file << setw(10) << left << entry["number"].get<string>();
                }

                file << "\n"; 
                cout << "Data successfully saved for: " << entry.dump() << endl;
            } else {
                throw runtime_error("Entry must contain 'name' and 'age'.");
            }
        } else {
            throw runtime_error("Failed to open data file for saving: " + filename);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void rewriteCSV(dbase& db, const string& table) {
    try {
        db.filename = db.schema_name + "\\" + table + "\\1.csv"; 
        ofstream file(db.filename); 

        if (file) {
            Node* table_node = db.findNode(table);
            if (table_node) {
                json columns = {"name", "age", "adress", "number"};

                for (const auto& column : columns) {
                    file << setw(10) << left << column.get<string>() << (column != columns.back() ? ", " : "");
                }
                file << "\n"; 

                for (size_t i = 0; i < table_node->data.getSize(); ++i) {
                    json entry = json::parse(table_node->data.get(i));
                    for (const auto& column : columns) {
                        file << setw(10) << left << entry[column.get<string>()].get<string>() << (column != columns.back() ? ", " : "");
                    }
                    file << "\n"; 
                }
            }
            file.close();
        } else {
            throw runtime_error("Failed to open data file for rewriting: " + db.filename);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}
