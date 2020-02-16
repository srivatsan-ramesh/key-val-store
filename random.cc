#include "random.h"

std::vector<std::pair<std::string, std::string>> keyVals;

std::string Random::generateRandomString(int size) {
    std::string randStr;
    for(int i = 0; i < size; i++) {
        randStr.push_back((std::rand()%(ASCII_MAX - ASCII_MIN + 1)) + ASCII_MIN);
    }
    return randStr;
}

std::vector<std::pair<std::string, std::string>>& Random::generateRandomKeyVals(int keySize, int valSize, int n) {
    if(keyVals.size() == 0) {
        srand(time(NULL));
    }
    keyVals.clear();
    // while(keyVals.size() < n) {
    //     keyVals.push_back(make_pair(generateRandomString(keySize), generateRandomString(valSize)));
    // }
    keyVals = std::vector<std::pair<std::string, std::string>>(n);
    return keyVals;
}

std::vector<std::pair<std::string, std::string>>& Random::getKeyVals() {
    return keyVals;
}