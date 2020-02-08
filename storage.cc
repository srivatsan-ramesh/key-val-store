#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

std::ofstream outfile;
std::mutex fileMutex;

void readFile(std::string filename) {
	std::ifstream file(filename);
	if (!file.is_open()) { 
        // std::cout << "No file found\n"; 
        return;
    } 
	std::string str;
	while (std::getline(file, str)) {
		std::size_t ind = str.find(' ');
		if(ind==std::string::npos) {
			continue;
		}
		std::string key = str.substr(0, ind);
		std::string value = str.substr(ind+1);
		std::cout << key<<":"<<value<< "\n";

		// TODO: Checksum validation

		// TODO: Call insert of Trie
	}
	file.close();
}

void appendData(std::string filename, std::string key, std::string value) {
	fileMutex.lock();

	if (!outfile.is_open()) { 
        // std::cout << "File not open so opening\n"; 
        outfile.open(filename, std::ios_base::app);
    }
    
    // TODO: Checksum computation and writing

	outfile << key<<" "<<value<<"\n";
	outfile.flush();

	fileMutex.unlock();
}

// int main () {
	// readFile("test.txt");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
	// appendData("test.txt", "apple0", "orange0");
// }