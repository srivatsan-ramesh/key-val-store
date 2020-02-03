#include "concurrent_trie.h"

template <class V>
class ConcurrentTrie<V>::ConcurrentTrieNode {
    public:
        ConcurrentTrieNode *children[ALPHABET_SIZE];
        V value;
        bool isEndOfKey;
        std::mutex read, write;

        ConcurrentTrieNode() {
            memset(children, 0, sizeof(children));
            isEndOfKey = false;
        }
};

template <class V>
ConcurrentTrie<V>::ConcurrentTrie() {
    root = new ConcurrentTrieNode();
}

int hashChar(int c) {
    return c - 'a';
}

template <class V>
void ConcurrentTrie<V>::insert(std::string key, V value) {
    root->read.lock();
    ConcurrentTrieNode *parent = root;
    ConcurrentTrieNode *child;
    int it = 0;
    while(parent) {
        if(!parent->children[hashChar(key[it])]) {
            parent->children[hashChar(key[it])] = new ConcurrentTrieNode();
        }
        child = parent->children[hashChar(key[it])];
        child->read.lock();
        parent->read.unlock();
        parent = child;
        it++;
        if(it == key.size()) {
            break;
        }
    }
    parent->value = value;
    parent->isEndOfKey = true;
    parent->read.unlock();
}

template <class V>
V ConcurrentTrie<V>::find(std::string key) {
    ConcurrentTrieNode *parent = root;
    int it = 0;
    while(parent) {
        if(!parent->children[hashChar(key[it])]) {
            return V();
        }
        parent = parent->children[hashChar(key[it])];
        it++;
        if(it == key.size()) {
            break;
        }
    }
    V value = V();
    if(parent->isEndOfKey) {
        parent->read.lock();
        value = parent->value;
        parent->read.unlock();
    }
    return value;
}

template <class V>
void ConcurrentTrie<V>::getAllValues(ConcurrentTrieNode *node, std::string &key, std::vector<std::pair<std::string, V> > &list) {
    if(!node) return;
    if(node->isEndOfKey) {
        node->read.lock();
        list.push_back(make_pair(key, node->value));
        node->read.unlock();
    }
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        ConcurrentTrieNode *child = node->children[i];
        key.push_back('a' + i);
        getAllValues(child, key, list);
        key.pop_back();
    }
}

template <class V>
std::vector<std::pair<std::string, V> > ConcurrentTrie<V>::findAll(std::string prefix) {
    ConcurrentTrieNode *parent = root;
    int it = 0;
    while(parent) {
        if(!parent->children[hashChar(prefix[it])]) {
            return std::vector<std::pair<std::string, V> >();
        }
        parent = parent->children[hashChar(prefix[it])];
        it++;
        if(it == prefix.size()) {
            break;
        }
    }
    std::vector<std::pair<std::string, V> > result;
    getAllValues(parent, prefix, result);
    return result;
}

template class ConcurrentTrie<std::string>;