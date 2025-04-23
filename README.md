# JSON-File-Manager
A C++ console program for Windows used for creating, modifying, deleting JSON files that are saved securely with a password

# How to use it
The program has 8 commands: create, write, edit, read, clear, delete, help and exit.
● create - is used for creating a file
● write - is used to add new key-value pairs, requires validation
● edit - is used to edit the existing key-value pairs, requires validation
● read - is used to read the files, requires validation
● clear - is used to empty file content, but keeps the file itself, requires validation
● delete - is used to completely deleting the file, requires validation
● help - provides a built-in manual for the program
● exit - exits the program

# create command:
Input create in console to use this command, after entering the command you need to choose a name for the file. If a file with that name already exists, the program will ask for a new name until it's correct. Then you'll need to create a password, from 8 to 24 characters long after creating and confirming it the program will create the file, salt and hash the password, create an ID and save all in database. The ID will be outputed on console, it is used for validation, don't forget it.

# validation
After creating a file, the program will generate an ID. To use write, edit, read, clear and delete commands you need to enter the ID and the password, if they dont exist or are incorrect the program will ask for new ID and password until they are correct or the keyword [exit] is written. After entering correct ID and password you'll be able to make changes with your file, if the keyword [exit] is entered the program exits to main menu.

# write command
Input write in console, after entering the command and passing the validation, the program will ask for a key or keyword [done]. After entering [done] the program will exit to main menu. If the entered key already exists the program will ask for new key. After entering a key's name you'll have to choose the value type:
1 - string,
2 - bool,
3 - int,
4 - double,
5 - array,
6 - nested object,
7 - null.
After entering the value type the program will now ask for a value, if it's incorrect the program will ask for a new key. If the value type you chose is array the program will ask for value type and value until the keyword [finish] is written, after that the program appends key-value pair to file. If the value type you chose is nested object the program will ask for key-value pairs until the keyword [finish] is written, after that the program appends key-value pair to file. If you choose another type the program will just append the key-value pair to file.

# edit command
Input edit in console, after entering the command and passing the validation the program will ask for a key or [finish] keyword, which exits to main menu. If the entered key doesn't exist, the program will ask for new key until it exists, if the key exists the program give 3 choices - rename, delete or update:
● rename - renames the key
● delete - deletes the key-value pair
● update - updates the value.
After choosing update the program will check the value type:
● if the value type is array - the program gives us 3 choices - add, remove or exit. After choosing add the program will ask for new value type and value until the keyword [finish] is written, it exits the array. After choosing remove the program will ask for element's index or keyword [finish] which exits the array, if the index is correct the program will remove the value. After choosing exit the program exits the array.
● if the value type is nested object - the program gives us 3 choices - delete, rename or update, they have the same functionality as the 3 commands in edit's menu, but they edit nested object's key-value pairs.
● other types - program will ask for new value type and new value.

# read command
Input delete in console to use this command, after entering the command and passing the validation the content of file will be outputed on console.

# clear command
Input clear in console to use this command, after entering the command and passing the validation the content of file will be deleted.

# delete command
Input delete in console to use this command, after entering the command and passing the validation the file will be deleted.

# help command
Input help in console to use this command, after entering the command the built-in manual will be outputed on console.

# exit commans
Input exit in console to use this command, after entering the command the program will be exited.

# Requirements
● Console app
● C++ 17 or later
● nlohmann/json and picosha2 libraries
● Windows operation system

# Advices
● Code is case sensitive, enter the commands as they are written on console, otherwise the program won't work
● The passwords can have from 8 to 24 symbols
● Don't close the program while appending data, it may cause errors
● Don't forget the ID and password of your file - once forgotten, it's impossible to recover
● clear and delete are used for different purposes, if you accidental deleted a file, it can't be recovered
● In code, change the written path of picosha2 to it's actual path
