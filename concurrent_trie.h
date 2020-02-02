#include <string>
#include <vector>
#include <mutex>

#define ALPHABET_SIZE 26

template <class V>
class ConcurrentTrie {
    public:
        class ConcurrentTrieNode;
        ConcurrentTrieNode *root;
        ConcurrentTrie<V>();
        void getAllValues(ConcurrentTrieNode *node, std::string &key, std::vector<std::pair<std::string, V> > &list);
        void insert(std::string key, V value);
        V find(std::string key);
        std::vector<std::pair<std::string, V> > findAll(std::string prefix);
};