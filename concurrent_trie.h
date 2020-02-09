#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <mutex>

#define ALPHABET_SIZE 256

template <class V>
class ConcurrentTrie {
    public:
        class ConcurrentTrieNode;
        ConcurrentTrieNode *root;
        ConcurrentTrie<V>();
        void getAllValues(ConcurrentTrieNode *node, std::string &key, std::vector<std::pair<std::string, V> > &list);
        void insert(std::string key, V value, bool persist=true);
        V find(std::string key);
        std::vector<std::pair<std::string, V> > findAll(std::string prefix);
};

#endif