#include "storage.h"
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

int hash(char c) {
    return c - 33;
}

char deHash(int i) {
    return i + 33;
}

template <class V>
void ConcurrentTrie<V>::insert(std::string key, V value, bool persist) {
    root->read.lock();
    ConcurrentTrieNode *parent = root;
    ConcurrentTrieNode *child;
    int it = 0;
    while(parent) {
        if(!parent->children[hash(key[it])]) {
            parent->children[hash(key[it])] = new ConcurrentTrieNode();
        }
        child = parent->children[hash(key[it])];
        child->read.lock();
        parent->read.unlock();
        parent = child;
        it++;
        if(it == key.size()) {
            break;
        }
    }
    if(persist)
        persistData(key, value);
    parent->value = value;
    parent->isEndOfKey = true;
    parent->read.unlock();
}

template <class V>
V ConcurrentTrie<V>::find(std::string key) {
    ConcurrentTrieNode *parent = root;
    int it = 0;
    while(parent) {
        if(!parent->children[hash(key[it])]) {
            return V();
        }
        parent = parent->children[hash(key[it])];
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
        key.push_back(deHash(i));
        getAllValues(child, key, list);
        key.pop_back();
    }
}

template <class V>
std::vector<std::pair<std::string, V> > ConcurrentTrie<V>::findAll(std::string prefix) {
    ConcurrentTrieNode *parent = root;
    int it = 0;
    while(parent) {
        if(!parent->children[hash(prefix[it])]) {
            return std::vector<std::pair<std::string, V> >();
        }
        parent = parent->children[hash(prefix[it])];
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