#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

// Define a maximum integer value for certain operations (avoiding the need for <limits>)
#define INT_MAX 2147483647

// Define default port number as a string for network communication
#define DEFAULT_PORT 8080

// Define buffer size for reading/writing data in chunks
#define BUFFER_SIZE 1024

// Trie Node definition
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;   // Maps each character to its child node
    bool isEndOfWord;                          // Marks the end of a valid word

    // Constructor initializes end-of-word marker to false
    TrieNode() : isEndOfWord(false) {}
};

// Trie Class Definition
class Trie {
private:
    TrieNode* root;                       // Root node for standard trie
    TrieNode* reverse_root;               // Root node for reverse trie
    unordered_map<string, int> frequency; // Frequency map to track search popularity

public:
    // Constructor initializes both root and reverse_root nodes
    Trie() : root(new TrieNode()), reverse_root(new TrieNode()) {}

    // Function to insert a word into the trie
    void insert(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            // If character node doesn't exist, create it
            if (!current->children.count(ch))
                current->children[ch] = new TrieNode();
            current = current->children[ch];    // Move to the child node
        }
        current->isEndOfWord = true;            // Mark end of the word
        rev_insert(word);                       // Insert into reverse trie as well
    }

    // Function to insert the reversed word into the reverse trie
    void rev_insert(const string& word) {
        TrieNode* current = reverse_root;
        string rev_word = word;                 // Copy word for reversal
        reverse(rev_word.begin(), rev_word.end());    // Reverse the word
        for (char ch : rev_word) {
            // If character node doesn't exist, create it
            if (!current->children.count(ch))
                current->children[ch] = new TrieNode();
            current = current->children[ch];    // Move to the child node
        }
        current->isEndOfWord = true;            // Mark end of the reversed word
    }

    // Update frequency on search
    void updateFrequency(const string& word) {
        frequency[word]++;                      // Increment the frequency count for the word
    }

    // Prefix search function
    vector<string> searchPrefix(const string& prefix) {
        TrieNode* current = root;

        // Update frequency if prefix is found in frequency map
        if (frequency.find(prefix) != frequency.end()) 
            updateFrequency(prefix);

        // Traverse trie for the given prefix
        for (char ch : prefix) {
            if (!current->children.count(ch))
                return {}; // Return empty if prefix is not found
            current = current->children[ch];
        }

        // Collect words that start with the prefix
        vector<string> results;
        getWordsFromNode(current, prefix, results);

        // Sort results based on frequency
        return sortByFrequency(results);
    }

    // Infix (substring) search function
    vector<string> searchInfix(const string& infix) {
        vector<string> results;

        // Update frequency if infix is found in frequency map
        if (frequency.find(infix) != frequency.end()) 
            updateFrequency(infix);

        // Find all words containing the infix substring
        getInfixMatches(root, "", infix, results);

        // Sort results based on frequency
        return sortByFrequency(results);
    }

    // Suffix search function
    vector<string> searchSuffix(const string& suffix) {
        // Update frequency if suffix is found in frequency map
        if (frequency.find(suffix) != frequency.end()) 
            updateFrequency(suffix);

        // Reverse the suffix to match with reverse trie
        string rev_suffix = suffix;
        reverse(rev_suffix.begin(), rev_suffix.end());

        TrieNode* current = reverse_root;

        // Traverse reverse trie for the reversed suffix
        for (char ch : rev_suffix) {
            if (!current->children.count(ch))
                return {}; // Return empty if suffix is not found
            current = current->children[ch];
        }

        // Collect words that end with the suffix
        vector<string> results;
        getWordsFromNode(current, rev_suffix, results);

        // Reverse each word to restore original order
        vector<string> rev_words = sortByFrequency(results);
        for (auto& word : rev_words) {
            reverse(word.begin(), word.end());
        }

        return rev_words;
    }

    // Retrieve the top 5 most searched terms
    vector<string> getTopSearchedWords() {
        // Convert frequency map to vector of pairs
        vector<pair<string, int>> freqVec(frequency.begin(), frequency.end());

        // Priority queue to store words by frequency
        priority_queue<pair<int, string>> pq;
        vector<string> final_string;

        // Push each frequency entry into the priority queue
        for (auto entry : freqVec) {
            pq.push({entry.second, entry.first});
        }

        // Retrieve top 5 most frequent words
        for (int i = 0; i < 5 && !pq.empty(); i++) {
            final_string.push_back(pq.top().second);
            pq.pop();
        }

        return final_string;
    }

private:
    // Helper function to gather all words from a given node
    // This function performs a DFS from the node, adding each complete word to results
    void getWordsFromNode(TrieNode* node, const string& currentPrefix, vector<string>& results) {
        // If the current node marks the end of a word, add it to the results
        if (node->isEndOfWord) {
            results.push_back(currentPrefix);
        }

        // Recurse over each child node, building the word with currentPrefix + character
        for (auto it = node->children.begin(); it != node->children.end(); ++it) {
            char ch = it->first;
            TrieNode* childNode = it->second;
            getWordsFromNode(childNode, currentPrefix + ch, results);
        }
    }

    // Recursive function to find matches for infix (substring) search
    // Searches for words that contain the specified infix, starting from the given node
    void getInfixMatches(TrieNode* node, string current, const string& infix, vector<string>& results) {
        // If the current node marks the end of a word and contains the infix, add it to results
        if (node->isEndOfWord && current.find(infix) != string::npos) {
            results.push_back(current);
        }

        // Recurse over each child node, building the word with current + character
        for (auto it = node->children.begin(); it != node->children.end(); ++it) {
            char ch = it->first;
            TrieNode* childNode = it->second;
            getInfixMatches(childNode, current + ch, infix, results);
        }
    }

    // Sort results by frequency
    // Sorts the words in descending order of frequency and returns the top 5 results
    vector<string> sortByFrequency(vector<string>& results) {
        priority_queue<pair<int, string>> pq;  // Priority queue to store words by frequency
        vector<string> final_string;

        // Push each word's frequency and the word itself into the priority queue
        for (const string& word : results) {
            pq.push({frequency[word], word});
        }

        // Retrieve the top 5 most frequent words
        for (int i = 0; i < 15 && !pq.empty(); i++) {
            final_string.push_back(pq.top().second);
            pq.pop();
        }
        return final_string;
    }
};

// Function to load words from a dictionary file into the trie
bool loadDictionary(Trie& trie, const string& filename) {
    ifstream file(filename);  // Open the file for reading
    if (!file.is_open()) {    // Check if the file opened successfully
        cerr << "Failed to open dictionary file." << endl;  // Print error if failed
        return 0;  // Return false (0) if file could not be opened
    }

    string word;
    // Read each word from the file and insert it into the trie
    while (getline(file, word)) {
        trie.insert(word);
    }
    file.close();  // Close the file after reading all words
    return 1;      // Return true (1) to indicate successful loading
}
    
Trie trie;  // Create a Trie instance
int boo = loadDictionary(trie, "cricket_words.txt");  // Load dictionary file into trie and store success status

std::string sanitizeForJson(const std::string& input) {
    std::string sanitized;
    for (char c : input) {
        switch (c) {
            case '\"': sanitized += "\\\""; break;
            case '\\': sanitized += "\\\\"; break;
            case '\b': sanitized += "\\b"; break;
            case '\f': sanitized += "\\f"; break;
            case '\n': sanitized += " "; break;      // Replace newlines with space
            case '\r': sanitized += " "; break;      // Replace carriage return
            case '\t': sanitized += " "; break;      // Replace tabs
            default:
                // Only allow ASCII printable characters
                if (c >= 0x20 && c <= 0x7E) {
                    sanitized += c;
                }
                break;
        }
    }
    return sanitized;
}

// Function to extract the query parameter from the JSON request string
string extractQuery(const std::string& request) {
    std::smatch match;
    std::regex queryRegex(R"("query"\s*:\s*\"([^\"]*)\")"); // Properly escaped double quotes
    if (std::regex_search(request, match, queryRegex) && match.size() > 1) {
        return match[1].str();
    }
    return "";
}

// FIX: Add logging for debugging query extraction
string handleRequest(const std::string& request) {
    std::string query = extractQuery(request);
    std::cerr << "Received query: " << query << std::endl; // Add debug log

    if (query.empty()) {
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or malformed query parameter.";
    }

    vector<string> suggestions;
    vector<string> prefixMatches = trie.searchPrefix(query);
    suggestions.insert(suggestions.end(), prefixMatches.begin(), prefixMatches.end());

    vector<string> infixMatches = trie.searchInfix(query);
    for (const auto& word : infixMatches) {
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    vector<string> suffixMatches = trie.searchSuffix(query);
    for (const auto& word : suffixMatches) {
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    vector<string> topSearches = trie.getTopSearchedWords();
    for (const auto& word : topSearches) {
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    std::string jsonResponse = "[";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        jsonResponse += "\"" + sanitizeForJson(suggestions[i]) + "\"";

        if (i < suggestions.size() - 1) jsonResponse += ",";
    }
    jsonResponse += "]";

    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                               "Access-Control-Allow-Headers: Content-Type\r\n\r\n"
                               + jsonResponse;
    return httpResponse;
}
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << DEFAULT_PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                                 (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            close(new_socket);
            continue;
        }

        std::string request(buffer, valread);

        if (request.find("OPTIONS") == 0) {
            std::string optionsResponse =
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
            send(new_socket, optionsResponse.c_str(), optionsResponse.size(), 0);
            close(new_socket);
            continue;
        }

        std::string response = handleRequest(request);
        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }

    return 0;
}
