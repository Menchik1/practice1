#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp" 

using namespace std;
using json = nlohmann::json;

struct Array {
    string* arr;
    size_t capacity;
    size_t size;

    Array() : capacity(10), size(0) {
        arr = new string[capacity];
    }

    ~Array() {
        delete[] arr;
    }

    void addEnd(const string& value) {
        if (size >= capacity) {
            capacity *= 2;
            string* new_arr = new string[capacity];
            copy(arr, arr + size, new_arr);
            delete[] arr;
            arr = new_arr;
        }
        arr[size++] = value;
    }

    string get(size_t index) const {
        if (index >= size) throw out_of_range("Index out of range");
        return arr[index];
    }

    size_t getSize() const {
        return size;
    }
};

struct Node {
    string name;
    Array data;
    Node* next;

    Node(const string& name) : name(name), next(nullptr) {}
};

struct dbase {
    string filename; 
    string schema_name;
    Node* head;
    int current_pk;

    dbase() : head(nullptr), current_pk(0) {}

    ~dbase() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    Node* findNode(const string& table_name) const {
        Node* current = head;
        while (current) {
            if (current->name == table_name) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    void addNode(const string& table_name) {
        Node* new_node = new Node(table_name);
        new_node->next = head;
        head = new_node;
    }

    size_t getColumnCount(const string& table) const {
        Node* table_node = findNode(table);
        if (table_node) {
            string filename = schema_name + "\\" + table + "\\1.csv";
            ifstream file(filename);
            if (file) {
                string header;
                if (getline(file, header)) {
                    return count(header.begin(), header.end(), ',') + 1;
                }
            }
        }
        return 0; 
    }

    void load() {
        Node* current = head;
        while (current) {
            try {
                filename = schema_name + "\\" + current->name + "\\1.csv"; 
                ifstream file(filename);
                if (file) {
                    string line;
                    bool is_header = true;
                    while (getline(file, line)) {
                        line.erase(0, line.find_first_not_of(" \t"));
                        line.erase(line.find_last_not_of(" \t") + 1);

                        if (is_header) {
                            is_header = false;
                            continue;
                        }

                        istringstream iss(line);
                        Array fields;
                        string field;

                        while (getline(iss, field, ',')) {
                            field.erase(0, field.find_first_not_of(" \t"));
                            field.erase(field.find_last_not_of(" \t") + 1);
                            if (!field.empty()) {
                                fields.addEnd(field);
                            }
                        }

                        if (fields.getSize() == 4) {
                            json entry;
                            entry["name"] = fields.get(0);
                            entry["age"] = fields.get(1);
                            entry["adress"] = fields.get(2);
                            entry["number"] = fields.get(3);

                            current->data.addEnd(entry.dump());
                        } 
                    }
                } else {
                    throw runtime_error("Failed to open data file: " + filename);
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            current = current->next;
        }
    }
};

#endif 
