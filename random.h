#ifndef RANDOM_H
#define RANDOM_H

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#define ASCII_MAX 126
#define ASCII_MIN 33

extern std::vector<std::pair<std::string, std::string>> keyVals;

class Random {
    public:
     static std::string generateRandomString(int size);
     static std::vector<std::pair<std::string, std::string>>& generateRandomKeyVals(int keySize, int valSize, int n);
     static std::vector<std::pair<std::string, std::string>>& getKeyVals();
};

#endif