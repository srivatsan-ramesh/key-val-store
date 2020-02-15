#include "storage.h"

std::ofstream outfile;
int fd;
std::mutex fileMutex;

const std::string FILE_NAME = "data.log";

// djb2 hashing
uint32_t checksum(std::string str) {
  uint32_t hashVal = 5381;
  
  for(std::string::size_type i = 0; i < str.size(); ++i) {
    hashVal = ((hashVal << 5) + hashVal) + str[i];
}

  return hashVal;
}

void buildTrieFromFile(ConcurrentTrie<std::string> &map) {
	std::ifstream file(FILE_NAME);
	if (!file.is_open()) { 
        // std::cout << "No file found\n"; 
        return;
    } 
	std::string str;
	while(std::getline(file, str)) {
		std::size_t ind = str.find(' ');
		if(ind == std::string::npos) {
			continue;
		}
		std::string key = str.substr(0, ind);

		std::size_t ind2 = str.find(' ', ind+1);
		if(ind2 == std::string::npos) {
			continue;
		}
		std::string value = str.substr(ind + 1, ind2 - ind - 1);

		std::string chksum = str.substr(ind2 + 1);

		uint32_t hashVal = checksum(key+value);

		if(chksum.compare(std::to_string(hashVal))!=0) {
			continue;
		}
		
		map.insert(key, value, false);
	}
	
	// Inserting newline at EOF if not present
	file.seekg(-1, std::ios_base::end);
	char c;
	file.get(c);
	file.close();
	if(c != '\n'){
	    outfile.open(FILE_NAME, std::ios_base::app);
	    outfile<<'\n';
	    outfile.close();
	}
}

void persistData(std::string key, std::string value) {
	fileMutex.lock();

	// Converting filename to char*
	char cfile[FILE_NAME.size() + 1];
	strcpy(cfile, FILE_NAME.c_str());

	if(fd == 0 || fd == -1) {
		fd = open(cfile, O_WRONLY | O_APPEND);
		if(fd == -1) {
			std::ofstream file(FILE_NAME);
			std::cout<<"New file created";
			file.close();
			fd = open(cfile, O_WRONLY | O_APPEND);
		}
	}

	uint32_t hashVal = checksum(key+value);

	// Convert string to be written to char*	
	std::string str = key + " " + value + " " + std::to_string(hashVal) + "\n";
	// Convert string to be written to char*	
	std::string str = key + " " + value + "\n";
	char cstr[str.size() + 1];
	strcpy(cstr, str.c_str());

	write(fd, cstr, sizeof(cstr)-1);
	fsync(fd);

	fileMutex.unlock();
}