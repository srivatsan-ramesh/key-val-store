#include "storage.h"

std::ofstream outfile;
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
}

void persistData(std::string key, std::string value) {
	fileMutex.lock();

	if (!outfile.is_open()) { 
        // std::cout << "File not open so opening\n"; 
        outfile.open(FILE_NAME, std::ios_base::app);
    }
    
    // TODO: Checksum computation and writing

	outfile<<key<<" "<<value<<"\n";
	outfile.flush();

	fileMutex.unlock();
}

// int main () {
	// readFile("test.txt");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
// }