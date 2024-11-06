#include <iostream>
#include <fstream>
#include <sys/stat.h> 
#include "json.hpp" 
#include <iomanip> 

#include "structures.hpp"
#include "locker.hpp"
#include "commands.hpp"
#include "fileOperations.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    dbase db;
    try {
        loadSchema(db, "schema.json");
        db.load();

        if (argc > 2 && string(argv[1]) == "--query") {
            string query = argv[2];
            istringstream iss(query);
            string action;
            iss >> action;

            try {
                if (action == "INSERT") {
                    string table;
                    iss >> table;

                    Array args;
                    string arg;

                    while (iss >> arg) {
                        args.addEnd(arg);
                    }

                    size_t expected_arg_count = db.getColumnCount(table);
                    if (args.getSize() > expected_arg_count) {
                        cout << "Error: Too many arguments (" << args.getSize() << ") for INSERT command." << endl;
                        return 1;
                    } else if (args.getSize() < 2) { 
                        cout << "Error: Not enough arguments (" << args.getSize() << ") for INSERT command." << endl;
                        return 1;
                    }

                    json entry = {
                        {"name", args.get(0)},
                        {"age", stoi(args.get(1))}
                    };

                    entry["adress"] = (args.getSize() > 2) ? args.get(2) : ""; 
                    entry["number"] = (args.getSize() > 3) ? args.get(3) : ""; 

                    insert(db, table, entry);
                } else if (action == "SELECT") {
                    string column, from, table, filter_type = "AND";
                    iss >> column >> from >> table;
                    if (from == "FROM") {
                        pair<string, string> filters[10];
                        int filter_count = 0;
                        string filter_part;

                        if (iss >> filter_part && filter_part == "WHERE") {
                            string filter_column, filter_value;
                            while (iss >> filter_column) {
                                string condition;
                                iss >> condition;
                                iss >> filter_value;
                                filters[filter_count++] = make_pair(filter_column, filter_value);

                                string connector;
                                iss >> connector;
                                if (connector == "OR") {
                                    filter_type = "OR";
                                } else if (connector != "AND") {
                                    break;
                                }
                            }
                        }
                        select(db, column, table, filters, filter_count, filter_type);
                    } else {
                        throw runtime_error("Invalid query format.");
                    }
                } else if (action == "Delete") {
                    string column, from, value, table;
                    iss >> from >> table >> column >> value;
                    Delete(db, column, value, table);
                } else {
                    throw runtime_error("Unknown command: " + query);
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        } else {
            cout << "Usage: " << argv[0] << " --query '<your query>'" << endl;
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
