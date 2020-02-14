#include "storage.h"

// #include <iostream>
// #include <fstream>
// #include <string>
// #include <mutex>
// #include <fcntl.h>
// #include <unistd.h>
// #include <cstring>

std::ofstream outfile;
int fd;
std::mutex fileMutex;

const std::string FILE_NAME = "data.log";

void buildTrieFromFile(ConcurrentTrie<std::string> &map) {
	std::ifstream file(FILE_NAME);
	if (!file.is_open()) { 
        // std::cout << "No file found\n"; 
        return;
    } 
	std::string str;
	while (std::getline(file, str)) {
		std::size_t ind = str.find(' ');
		if(ind == std::string::npos) {
			continue;
		}
		std::string key = str.substr(0, ind);
		std::string value = str.substr(ind + 1);

		// TODO: Checksum validation

		map.insert(key, value, false);
	}
	file.close();

	// Inserting newline at EOF if not present
	std::ifstream file(FILE_NAME);
	file.seekg(-1,std::ios_base::end);
	char c;
	file.get(c);
	file.close();
	if(c!='\n'){
	    outfile.open(FILE_NAME, std::ios_base::app);
	    outfile<<"\n";
	    outfile.close();
	}
}

// void persistData(std::string key, std::string value) {
// 	fileMutex.lock();

// 	if (!outfile.is_open()) { 
//         // std::cout << "File not open so opening\n"; 
//         outfile.open(FILE_NAME, std::ios_base::app);
//     }
    
//     // TODO: Checksum computation and writing

// 	outfile<<key<<" "<<value<<"\n";
// 	outfile.flush();

// 	fileMutex.unlock();
// }

void persistData(std::string key, std::string value) {
	fileMutex.lock();

	// Converting filename to char*
	char cfile[FILE_NAME.size() + 1];
	strcpy(cfile, FILE_NAME.c_str());

	if (fd ==0 || fd==-1)
	{
		fd = open(cfile, O_WRONLY | O_APPEND);
		if(fd==-1){
			std::ofstream file(FILE_NAME);
			std::cout<<"New file created";
			file.close();
			fd = open(cfile, O_WRONLY | O_APPEND);
		}
	}
	
	// TODO: Checksum computation and writing

	// Convert string to be written to char*	
	std::string str = key + " " + value + "\n";
	char cstr[str.size() + 1];
	strcpy(cstr, str.c_str());

	write(fd, cstr, sizeof(cstr)-1);
	fsync(fd);

	fileMutex.unlock();
}

// int main () {
	// readFile("test.txt");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
// }