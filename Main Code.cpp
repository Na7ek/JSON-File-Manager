#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <chrono>
#include <string>
#include "C:\Users\User\Documents\C++ Libraries\picosha2.h"

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

class JsonFile {
public:

	void Create() {

		cout << "Enter the name of your file:";
		getline(cin, filename);
		string filepath = folder + "\\" + filename + ".json";

		if (!fs::exists(folder)) {
			cout << "Such folder does not exist,creating..." << endl;
			fs::create_directory(folder);
		}

		if (!fs::exists(filepath)) {
			string password;
			string pwconfirm;
			bool pwcreated = false;
			while (!pwcreated) {
				cout << "Create a password:";
				getline(cin, password);
				if (password.length() >= 8 && password.length() <= 24) {
					cout << "Confirm your password:";
					getline(cin, pwconfirm);
					if (password == pwconfirm) {
						ofstream o(filepath, ofstream::trunc);
						o << "{}";
						o.close();
						cout << "File created successfully." << endl;
						pwcreated = true;
						DatabaseAppend(password, filepath);
					}
					else {
						cout << "The entered passwords have to be the same, try again." << endl;
					}
				}
				else {
					cout << "Password can contain from 8 to 24 characters, try again." << endl;
				}
			}
		}
		else {
			cout << "File already exists." << endl;
		}

	}

	void Program() {

		string command;
		bool commanded = true;
		while (commanded) {
			cout << "Choose an action -> [create, write, clear, delete, read, edit, help, exit]:";
			getline(cin, command);
			if (command == "create") {
				Create();
				Divider();
			}
			else if (command == "write") {
				Write();
				Divider();
			}
			else if (command == "clear") {
				Clear();
				Divider();
			}
			else if (command == "delete") {
				Delete();
				Divider();
			}
			else if (command == "read") {
				Read();
				Divider();
			}
			else if (command == "edit") {
				Edit();
				Divider();
			}
			else if (command == "exit") {
				cout << "Program exited successfully." << endl;
				commanded = false;
			}
			else if (command == "help") {
				Help();
				Divider();
			}
			else {
				cout << "Invalid command." << endl;
			}
		}

	}

	string IdGenerator() {

		auto now = chrono::system_clock::now();

		auto millis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

		string millis_str = to_string(millis);

		string id = millis_str.substr(millis_str.length() - 6);

		cout << "Your ID:" << id << endl;
		return id;
	}

	void DatabaseAppend(const string& password, const string& filepath) {

		string saltedPassword = staticSalt + password;
		string hashedPassword = picosha2::hash256_hex_string(saltedPassword);

		ifstream inputFile(data);
		json j;

		if (inputFile.is_open()) {
			inputFile >> j;
			inputFile.close();
		}
		else {
			j = json::array();
		}

		string id = IdGenerator();

		bool idExists = false;
		for (auto& entry : j) {
			if (entry.contains(id)) {
				idExists = true;
				break;
			}
		}

		while (idExists) {
			id = IdGenerator();
			idExists = false;
			for (auto& entry : j) {
				if (entry.contains(id)) {
					idExists = true;
					break;
				}
			}
		}

		json newEntry;
		newEntry[id] = {
			{"password", hashedPassword},
			{"filepath", filepath}
		};

		j.push_back(newEntry);

		ofstream outputFile(data);
		if (outputFile.is_open()) {
			outputFile << j.dump(4);
			outputFile.close();
		}
		else {
			cout << "Error opening database for writing." << endl;
		}
	}

	string IsValid() const {
		string id;
		string password, filepath;

		while (true) {
			cout << "Enter ID or [exit] to cancel:";
			getline(cin, id);

			if (id == "[exit]") {
				cout << "Exiting to main menu." << endl;
				return "";
			}

			ifstream inputFile(data);
			json j;
			bool idExists = false;

			if (inputFile.is_open()) {
				inputFile >> j;
				inputFile.close();
			}
			else {
				cout << "Error opening database for reading." << endl;
				return "";
			}

			for (auto& entry : j) {
				if (entry.contains(id)) {
					idExists = true;
					password = entry[id]["password"];
					filepath = entry[id]["filepath"];
					break;
				}
			}

			if (!idExists) {
				cout << "ID not found, try again or type [exit] to cancel." << endl;
				continue;
			}

			while (true) {
				string enteredPassword;
				cout << "Enter password or [exit] to cancel:";
				getline(cin, enteredPassword);

				if (enteredPassword == "[exit]") {
					cout << "Exiting to main menu." << endl;
					return "";
				}

				string saltedEntered = staticSalt + enteredPassword;
				string hashedEnteredPassword = picosha2::hash256_hex_string(saltedEntered);

				if (hashedEnteredPassword == password) {
					return filepath;
				}
				else {
					cout << "Incorrect password, try again or type [exit] to cancel." << endl;
				}
			}
		}
	}


	void Write() {
		string validFilepath = IsValid();

		ifstream outputFile(validFilepath);
		json j;

		if (!LoadJson(validFilepath, j)) {
			return;
		}

		cout << "File content:" << endl << j.dump(4) << endl;

		string value;
		string key;
		string choice;
		bool ended = false;

		json arr = json::array();
		string arrayChoice;
		bool adding = true;

		while (!ended) {
			cout << "Enter the name of key or [done] to finish:";
			getline(cin, key);
			if (key == "[done]") {
				ended = true;
				cout << "Exited successfully." << endl;
				break;
			}

			if (j.contains(key)) {
				cout << "Key named \"" << key << "\" already exists, please enter a unique key. If you want to edit already existing keys use the edit function." << endl;
				continue;
			}

			cout << "Enter the type of value's data you want to add:" << endl
				<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null:";
			cin >> choice;
			cin.ignore();

			if (choice == "1") {
				cout << "Enter the string value:";
				getline(cin, value);
				j[key] = value;
			}
			else if (choice == "2") {
				cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
				getline(cin, value);
				if (value == "true" || value == "1") {
					j[key] = true;
				}
				else if (value == "false" || value == "0") {
					j[key] = false;
				}
				else {
					cout << "Invalid bool value." << endl;
					continue;
				}
			}
			else if (choice == "3") {
				cout << "Enter the int value:";
				getline(cin, value);
				try {
					int intValue = stoi(value);
					j[key] = intValue;
				}
				catch (...) {
					cout << "Invalid int value." << endl;
					continue;
				}
			}
			else if (choice == "4") {
				cout << "Enter the double value:";
				getline(cin, value);
				try {
					double doubleValue = stod(value);
					j[key] = doubleValue;
				}
				catch (...) {
					cout << "Invalid double value." << endl;
					continue;
				}
			}
			else if (choice == "5") {
				j[key] = NestedArray();
			}
			else if (choice == "6") {
				j[key] = NestedObject();
			}
			else if (choice == "7") {
				j[key] = nullptr;
			}
			else {
				cout << "Invalid option, try again." << endl;
				continue;
			}

			ofstream inputFile(validFilepath, ofstream::trunc);
			if (!inputFile.is_open()) {
				cerr << "Could not open the file for writing." << endl;
			}
			else {
				inputFile << j.dump(4) << endl;
				inputFile.close();
				cout << "Data has been updated and written to file!" << endl;
			}
		}
	}

	void Clear() const {

		string validFilepath = IsValid();

		ofstream file(validFilepath, ofstream::trunc);

		file << "{}";
		file.close();
		cout << "File cleared successfully." << endl;

	}

	void Delete() const {

		string validFilepath = IsValid();

		if (fs::exists(validFilepath)) {
			fs::remove(validFilepath);
			cout << "File deleted successfully." << endl;

			ifstream inputFile(data);
			json j;

			if (inputFile.is_open()) {
				inputFile >> j;
				inputFile.close();
			}
			else {
				cout << "Error opening database for reading." << endl;
				return;
			}

			auto it = std::remove_if(j.begin(), j.end(), [&validFilepath](json& entry) {
				for (auto& [key, value] : entry.items()) {
					if (value["filepath"] == validFilepath) {
						return true;
					}
				}
				return false;
				});

			if (it != j.end()) {
				j.erase(it, j.end());
				ofstream outputFile(data);
				if (outputFile.is_open()) {
					outputFile << j.dump(4);
					outputFile.close();
				}
				else {
					cout << "Error opening database for writing." << endl;
				}
			}
			else {
				cout << "No corresponding entry found in the database." << endl;
			}
		}
		else {
			cout << "File does not exist." << endl;
		}

	}

	void Read() {

		string validFilepath = IsValid();
		json loadedConfig;

		if (!LoadJson(validFilepath, loadedConfig)) {
			return;
		}

		cout << "File content:" << endl << loadedConfig.dump(4) << endl;

	}

	void Edit() {
		string validFilepath = IsValid();

		ifstream outputFile(validFilepath);
		json j;

		if (!LoadJson(validFilepath, j)) {
			return;
		}

		cout << "File content:" << endl << j.dump(4) << endl;

		string keyForEditing;
		string action;
		bool ended = false;
		string order;

		while (!ended) {
			cout << "Enter name of an existing key, [help] for guide or [finish] to exit:";
			getline(cin, keyForEditing);

			if (keyForEditing == "[finish]") {
				ended = true;
				cout << "Exited successfully." << endl;
				break;
			}

			if (keyForEditing == "[help]") {
				HelpEdit();
			}

			if (j.contains(keyForEditing)) {
				cout << "Choose an action -> [delete, rename, update]:";
				getline(cin, action);
				if (action == "delete") {
					j.erase(keyForEditing);
					cout << "Key deleted successfully." << endl;
				}
				else if (action == "rename") {
					string newKey;
					bool changed = false;
					while (!changed) {
						cout << "Enter the new name of the key:";
						getline(cin, newKey);
						if (j.contains(newKey)) {
							cout << "A key named \"" << newKey << "\" already exists, try again.";
						}
						else {
							j[newKey] = j[keyForEditing];
							j.erase(keyForEditing);
							changed = true;
							break;
						}
					}
				}
				else if (action == "update") {
					if (j[keyForEditing].is_array()) {
						cout << "The key's value type is an array, do you want to edit the array or replace it with a new value? -> [edit, replace]:";
						getline(cin, order);
						if (order == "edit") {
							EditArray(keyForEditing, j);
						}
						else if (order == "replace") {
							EditValue(keyForEditing, j);
						}
						else {
							cout << "Invalid option." << endl;
							continue;
						}
					}
					else if (j[keyForEditing].is_object()) {
						cout << "The key's value type is an object, do you want to edit the object or replace it with a new value? -> [edit, replace]:";
						getline(cin, order);
						if (order == "edit") {
							EditObject(keyForEditing, j);
						}
						else if (order == "replace") {
							EditValue(keyForEditing, j);
						}
						else {
							cout << "Invalid option." << endl;
							continue;
						}
					}
					else {
						EditValue(keyForEditing, j);
					}
				}
				else {
					cout << "Invalid command." << endl;
					continue;
				}

				ofstream outputFile(validFilepath, ofstream::trunc);
				if (outputFile.is_open()) {
					outputFile << j.dump(4);
					outputFile.close();
					cout << "Changes saved successfully!" << endl;
				}
				else {
					cout << "Error saving changes to the file." << endl;
				}

			}
			else {
				cout << "Json file doesnt contain a key named \"" << keyForEditing << "\"." << endl;
			}
		}

	}

	json NestedArray() {
		json arr = json::array();
		string input;
		bool adding = true;

		while (adding) {
			cout << "Enter the type of value's data you want to add:" << endl
				<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null" << endl << "[finish]-finish:";

			getline(cin, input);

			if (input == "1") {
				string val;
				cout << "Enter the string value:";
				getline(cin, val);
				arr.push_back(val);
				cout << "Appended to array!" << endl;
			}
			else if (input == "2") {
				string val;
				cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
				getline(cin, val);
				if (val == "true" || val == "1") {
					arr.push_back(true);
					cout << "Appended to array!" << endl;
				}
				else if (val == "false" || val == "0") {
					arr.push_back(false);
					cout << "Appended to array!" << endl;
				}
				else {
					cout << "Invalid boolean value." << endl;
					continue;
				}
			}
			else if (input == "3") {
				string val;
				cout << "Enter the int value:";
				getline(cin, val);
				try {
					arr.push_back(stoi(val));
					cout << "Appended to array!" << endl;
				}
				catch (...) {
					cout << "Invalid int value." << endl;
					continue;
				}
			}
			else if (input == "4") {
				string val;
				cout << "Enter the double value:";
				getline(cin, val);
				try {
					arr.push_back(stod(val));
					cout << "Appended to array!" << endl;
				}
				catch (...) {
					cout << "Invalid double value." << endl;
					continue;
				}
			}
			else if (input == "5") {
				arr.push_back(NestedArray());
				cout << "Appended to array!" << endl;
			}
			else if (input == "6") {
				arr.push_back(NestedObject());
				cout << "Appended to array!" << endl;
			}
			else if (input == "7") {
				arr.push_back(nullptr);
				cout << "Appended to array!" << endl;
			}
			else if (input == "[finish]") {
				adding = false;
				cout << "Array created successfully." << endl;
				break;
			}
			else {
				cout << "Invalid option." << endl;
				continue;
			}
		}

		return arr;
	}

	json NestedObject() {
		json obj;
		string minikey;
		string minivalue;
		string option;
		bool done = false;

		while (!done) {
			cout << "Enter the name of object's key or [finish] to finish:";
			getline(cin, minikey);
			if (minikey == "[finish]") {
				done = true;
				cout << "Exited successfully." << endl;
				break;
			}

			if (obj.contains(minikey)) {
				cout << "Key named \"" << minikey << "\" already exists. Please enter a unique key, to edit already existing keys use the edit function." << endl;
				continue;
			}

			cout << "Enter the type of value's data you want to add:" << endl
				<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null:";
			cin >> option;
			cin.ignore();

			if (option == "1") {
				cout << "Enter the string value:";
				getline(cin, minivalue);
				obj[minikey] = minivalue;
				cout << "Appended to object!" << endl;
			}
			else if (option == "2") {
				cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
				getline(cin, minivalue);
				if (minivalue == "true" || minivalue == "1") {
					obj[minikey] = true;
					cout << "Appended to object!" << endl;
				}
				else if (minivalue == "false" || minivalue == "0") {
					obj[minikey] = false;
					cout << "Appended to object!" << endl;
				}
				else {
					cout << "Invalid bool value." << endl;
					continue;
				}
			}
			else if (option == "3") {
				cout << "Enter the int value:";
				getline(cin, minivalue);
				try {
					int intValue = stoi(minivalue);
					obj[minikey] = intValue;
					cout << "Appended to object!" << endl;
				}
				catch (...) {
					cout << "Invalid int value." << endl;
					continue;
				}
			}
			else if (option == "4") {
				cout << "Enter the double value:";
				getline(cin, minivalue);
				try {
					double doubleValue = stod(minivalue);
					obj[minikey] = doubleValue;
					cout << "Appended to object!" << endl;
				}
				catch (...) {
					cout << "Invalid double value." << endl;
					continue;
				}
			}
			else if (option == "5") {
				obj[minikey] = NestedArray();
				cout << "Appended to object!" << endl;
			}
			else if (option == "6") {
				obj[minikey] = NestedObject();
				cout << "Appended to object!" << endl;
			}
			else if (option == "7") {
				obj[minikey] = nullptr;
				cout << "Appended to object!" << endl;
			}
			else {
				cout << "Invalid option, try again." << endl;
				continue;
			}
		}
		return obj;
	}

	void Help() {
		cout << "Hello, this is a program for managing JSON files securely." << endl;
		cout << "You can use it to create, read, write, edit, and delete JSON files," << endl;
		cout << "with password protection and unique file identification." << endl << endl;

		cout << "Available commands:" << endl;
		cout << " - create : Create a new JSON file with a password and unique ID" << endl;
		cout << " - write  : Add new keys and values to an existing JSON file" << endl;
		cout << " - read   : Display the contents of a JSON file" << endl;
		cout << " - edit   : Modify, rename or delete specific keys in a JSON file" << endl;
		cout << " - clear  : Remove all contents of a JSON file" << endl;
		cout << " - delete : Completely delete a JSON file and its record from the database" << endl;
		cout << " - help   : Show this help menu" << endl;
		cout << " - exit   : Exit the program" << endl << endl;

		cout << "How it works:" << endl;
		cout << " - When you create a file, it's saved with a unique ID." << endl;
		cout << " - You’ll need this ID and your password to access the file later." << endl;
		cout << " - All passwords are hashed and stored securely with a salt." << endl << endl;

		cout << "Tips:" << endl;
		cout << " - Use 'edit' to change existing data." << endl;
		cout << " - Use 'clear' if you want to reset the file but not delete it." << endl;
		cout << " - You can create nested arrays and objects easily using write/edit commands." << endl << endl;

		cout << "Important notes:" << endl;
		cout << " - The program is case sensitive. Make sure to enter commands, keys, and values exactly as they are intended." << endl;
		cout << " - For example, 'create' is different from 'Create' and 'READ' is different from 'read'." << endl << endl;

		cout << "Your data is kept private and secure. Make sure to remember your ID and password!" << endl;
	}

	void HelpEdit() {

		cout << "Edit function is used for modifying already existing keys." << endl << endl;

		cout << "Available commands:" << endl;
		cout << " - delete : Delete the key and it's value from file" << endl;
		cout << " - rename : Rename the key without changing it's value" << endl;
		cout << " - update : Change the value of entered key" << endl << endl;

		cout << "Important notes:" << endl;
		cout << " - If the value is an array or a nested object, after choosing update you'll be able to choose the next actions:" << endl;
		cout << " - edit : Append= or delete elements from array or nested object" << endl;
		cout << " - replace : Replace the value (array or nested object) with new value" << endl << endl;
	}

	void EditValue(const string& key, json& j) {

		string newValue;
		string pick;

		cout << "Choose new type of the value:" << endl
			<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null:";
		getline(cin, pick);

		if (pick == "1") {
			cout << "Enter the string value:";
			getline(cin, newValue);
			j[key] = newValue;
		}
		else if (pick == "2") {
			cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
			getline(cin, newValue);
			if (newValue == "true" || newValue == "1") {
				j[key] = true;
			}
			else if (newValue == "false" || newValue == "0") {
				j[key] = false;
			}
			else {
				cout << "Invalid bool value." << endl;
			}
		}
		else if (pick == "3") {
			cout << "Enter the int value:";
			getline(cin, newValue);
			try {
				int intValue = stoi(newValue);
				j[key] = intValue;
			}
			catch (...) {
				cout << "Invalid int value." << endl;
			}
		}
		else if (pick == "4") {
			cout << "Enter the double value:";
			getline(cin, newValue);
			try {
				double doubleValue = stod(newValue);
				j[key] = doubleValue;
			}
			catch (...) {
				cout << "Invalid double value." << endl;
			}
		}
		else if (pick == "5") {
			j[key] = NestedArray();
		}
		else if (pick == "6") {
			j[key] = NestedObject();
		}
		else if (pick == "7") {
			j[key] = nullptr;
		}
		else {
			cout << "Invalid option, try again." << endl;
		}
	}
	void EditObject(const string& key, json& j) {
		string nestedKey;
		bool editing = true;
		string command;
		cout << "Nested object:" << endl << j[key].dump(4) << endl;
		while (editing) {
			cout << "Enter the name of nested object's key or [finish] to finish:";
			getline(cin, nestedKey);

			if (nestedKey == "[finish]") {
				cout << "Object exited successfully." << endl;
				editing = false;
				break;
			}

			if (j[key].find(nestedKey) == j[key].end()) {
				cerr << "Nested key not found";
				continue;
			}
			else {
				cout << "Choose an action -> [delete, rename, update]:";
				getline(cin, command);
				if (command == "delete") {
					j[key].erase(nestedKey);
					cout << "The key \"" << nestedKey << "\ deleted from object" << endl;
				}
				else if (command == "rename") {
					string newKey;
					j[key][nestedKey] = j[newKey];
					j[key].erase(nestedKey);
					cout << "Key named \"" << nestedKey << "\" renamed to \"" << newKey << " \" ." << endl;
				}
				else if (command == "update") {
					string newValue;
					string pick;

					cout << "Choose new type of the value:" << endl
						<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null:";
					getline(cin, pick);

					if (pick == "1") {
						cout << "Enter the string value:";
						getline(cin, newValue);
						j[key][nestedKey] = newValue;
						cout << "Appended to object!" << endl;
					}
					else if (pick == "2") {
						cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
						getline(cin, newValue);
						if (newValue == "true" || newValue == "1") {
							j[key][nestedKey] = true;
							cout << "Appended to object!" << endl;
						}
						else if (newValue == "false" || newValue == "0") {
							j[key][nestedKey] = false;
							cout << "Appended to object!" << endl;
						}
						else {
							cout << "Invalid bool value." << endl;
							continue;
						}
					}
					else if (pick == "3") {
						cout << "Enter the int value:";
						getline(cin, newValue);
						try {
							int intValue = stoi(newValue);
							j[key][nestedKey] = intValue;
							cout << "Appended to object!" << endl;
						}
						catch (...) {
							cout << "Invalid int value." << endl;
							continue;
						}
					}
					else if (pick == "4") {
						cout << "Enter the double value:";
						getline(cin, newValue);
						try {
							double doubleValue = stod(newValue);
							j[key][nestedKey] = doubleValue;
							cout << "Appended to object!" << endl;
						}
						catch (...) {
							cout << "Invalid double value." << endl;
							continue;
						}
					}
					else if (pick == "5") {
						j[key][nestedKey] = NestedArray();
						cout << "Appended to object!" << endl;
					}
					else if (pick == "6") {
						j[key][nestedKey] = NestedObject();
						cout << "Appended to object!" << endl;
					}
					else if (pick == "7") {
						j[key][nestedKey] = nullptr;
						cout << "Appended to object!" << endl;
					}
					else {
						cout << "Invalid option, try again." << endl;
						continue;
					}
				}
				else {
					cout << "Invalid command.";
					continue;
				}
			}
		}
	}
	void EditArray(const string& key, json& j) {
		string action;
		bool finished = false;
		cout << "Array:" << endl << j[key].dump(4) << endl;
		while (!finished) {
			cout << "Choose an action -> [add, remove, exit]:";
			getline(cin, action);
			if (action == "add") {
				string input;
				bool adding = true;

				while (adding) {
					cout << "Enter the type of value's data you want to add:" << endl
						<< "1-string" << endl << "2-bool" << endl << "3-int" << endl << "4-double" << endl << "5-array" << endl << "6-nested object" << endl << "7-null" << endl << "[finish]-finish:";

					getline(cin, input);

					if (input == "1") {
						string val;
						cout << "Enter the string value:";
						getline(cin, val);
						j[key].push_back(val);
						cout << "Appended to array!" << endl;
					}
					else if (input == "2") {
						string val;
						cout << "Enter the bool value (true or 1 for true, false or 0 for false):";
						getline(cin, val);
						if (val == "true" || val == "1") {
							j[key].push_back(true);
							cout << "Appended to array!" << endl;
						}
						else if (val == "false" || val == "0") {
							j[key].push_back(false);
							cout << "Appended to array!" << endl;
						}
						else {
							cout << "Invalid boolean value." << endl;
							continue;
						}
					}
					else if (input == "3") {
						string val;
						cout << "Enter the int value:";
						getline(cin, val);
						try {
							j[key].push_back(stoi(val));
							cout << "Appended to array!" << endl;
						}
						catch (...) {
							cout << "Invalid int value." << endl;
							continue;
						}
					}
					else if (input == "4") {
						string val;
						cout << "Enter the double value:";
						getline(cin, val);
						try {
							j[key].push_back(stod(val));
							cout << "Appended to array!" << endl;
						}
						catch (...) {
							cout << "Invalid double value." << endl;
							continue;
						}
					}
					else if (input == "5") {
						j[key].push_back(NestedArray());
						cout << "Appended to array!" << endl;
					}
					else if (input == "6") {
						j[key].push_back(NestedObject());
						cout << "Appended to array!" << endl;
					}
					else if (input == "7") {
						j[key].push_back(nullptr);
						cout << "Appended to array!" << endl;
					}
					else if (input == "[finish]") {
						adding = false;
						cout << "Array edited successfully." << endl;
						break;
					}
					else {
						cout << "Invalid option." << endl;
						continue;
					}
				}
			}
			else if (action == "remove") {
				string strIndex;
				bool removing = true;
				while (removing) {
					cout << "Enter the index of element you want to delete or [finish] to exit:";
					getline(cin, strIndex);
					if (strIndex == "[finish]") {
						cout << "Exited successfully" << endl;
						removing = false;
						break;
					}
					try {
						int index = stoi(strIndex);

						if (index >= 0 && index < j[key].size()) {
							j[key].erase(j[key].begin() + index);
							cout << "Element on index " << index << " deleted successfully."<<endl;
						}
						else {
							cout << "Invalid index" << endl;
						}
					}
					catch (...) {
						cout << "Invalid int value";
						continue;
					}
				}
			}
			else if (action == "exit") {
				cout << "Array exited successfully" << endl;
				finished = true;
				break;
			}
			else {
				cout << "Invalid option, try again" << endl;
				continue;
			}
		}
	}

private:
	string config = "C:\\Users\\User\\Documents\\Practice\\MarchP\\Day1Mr\\config.json";
	string folder = Folder();
	string data = Data();
	string filename;
	const string staticSalt = Salt();

	void Divider() {
		cout << endl << "==============================" << endl << endl;
	}

	bool LoadJson(const string& path, json& j) {
		ifstream file(path);
		if (!file.is_open()) return false;

		try {
			file >> j;
			file.close();
			return true;
		}
		catch (const json::parse_error& e) {
			cerr << "Failed to parse JSON at " << path << ":" << e.what() << endl;
			file.close();
			return false;
		}
	}

	string Data() const {
		ifstream configFile(config);
		json j;
		configFile >> j;
		string database = j["data"];
		return database;
	}

	string Folder() const {
		ifstream configFile(config);
		json j;
		configFile >> j;
		string folder = j["folder"];
		return folder;
	}

	string Salt() const {
		ifstream configFile(config);
		json j;
		configFile >> j;
		string salt = j["salt"];
		return salt;
	}
};

int main() {
	JsonFile am;
	am.Program();
}
