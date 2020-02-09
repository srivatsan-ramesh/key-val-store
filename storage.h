#ifndef STORAGE_H
#define STORAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

#include "concurrent_trie.h"

extern std::ofstream outfile;
extern std::mutex fileMutex;

void buildTrieFromFile(ConcurrentTrie<std::string> &map);
void persistData(std::string key, std::string value);

#endif