// Define the target Windows version to use the Windows Sockets 2 (Winsock) library
#define _WIN32_WINNT 0x501

// Include the necessary Windows Sockets and Internet Protocol libraries
#include <winsock2.h>
#include <ws2tcpip.h>

// Link the program with the Winsock library during compilation
#pragma comment(lib, "ws2_32.lib")  

// Include standard libraries for input/output, data structures, and other utilities
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm> // For std::min function
#include <fstream>   // For file handling (reading/writing files)
#include <regex>     // For regular expressions
#include <queue>     // For priority_queue data structure

using namespace std;

// Define a maximum integer value for certain operations (avoiding the need for <limits>)
#define INT_MAX 2147483647

// Define default port number as a string for network communication
#define DEFAULT_PORT "8080"

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
int boo = loadDictionary(trie, "random.txt");  // Load dictionary file into trie and store success status

// Function to extract the query parameter from the JSON request string
string extractQuery(const std::string& request) {
    std::smatch match;
    std::regex queryRegex(R"("query"\s*:\s*"([^"]*)\")"); // Regex to find "query" parameter

    // Search for the query pattern and extract it if found
    if (std::regex_search(request, match, queryRegex) && match.size() > 1) {
        return match.str(1); // Return the extracted query part
    }
    return ""; // Return an empty string if no match is found
}

// Function to handle the incoming request, generate suggestions, and return an HTTP response
string handleRequest(const std::string& request) {
    std::string query = extractQuery(request); // Extract query from JSON request

    vector<string> suggestions;  // Vector to store all suggestions

    // Get suggestions based on prefix match
    vector<string> prefixMatches = trie.searchPrefix(query);
    suggestions.insert(suggestions.end(), prefixMatches.begin(), prefixMatches.end());

    // Autocorrect suggestions based on infix match if prefix match is not enough
    vector<string> infixMatches = trie.searchInfix(query);
    for (const auto& word : infixMatches) {
        // Add only unique suggestions
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    // Add suffix matches, avoiding duplicate suggestions
    vector<string> suffixMatches = trie.searchSuffix(query);
    for (const auto& word : suffixMatches) {
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    // Add top searched words, avoiding duplicates and limiting to top 5 suggestions
    vector<string> topSearches = trie.getTopSearchedWords();
    for (const auto& word : topSearches) {
        if (find(suggestions.begin(), suggestions.end(), word) == suggestions.end()) {
            suggestions.push_back(word);
        }
    }

    // Build the JSON array response containing all suggestions
    std::string jsonResponse = "[";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        jsonResponse += "\"" + suggestions[i] + "\""; // Add each suggestion as a JSON string
        if (i < suggestions.size() - 1) jsonResponse += ","; // Add comma between items
    }
    jsonResponse += "]"; // Close the JSON array

    // Build the HTTP response with headers and JSON response body
    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                               "Access-Control-Allow-Headers: Content-Type\r\n\r\n"
                               + jsonResponse;
    return httpResponse; // Return the full HTTP response
}

int main() {
    WSADATA wsaData;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;  // Return error if Winsock initialization fails
    }

    struct addrinfo* result = NULL, hints = {};
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // Stream socket (TCP)
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol
    hints.ai_flags = AI_PASSIVE;     // Passive socket for binding

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup(); // Clean up Winsock
        return 1;
    }

    // Create a socket for connecting to the client
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result); // Free address information
        WSACleanup();         // Clean up Winsock
        return 1;
    }

    // Bind the socket to the address
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);         // Free address information
        closesocket(ListenSocket);     // Close the listening socket
        WSACleanup();                 // Clean up Winsock
        return 1;
    }

    freeaddrinfo(result); // Free the address information as it is no longer needed

    // Listen for incoming connections
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);     // Close the listening socket
        WSACleanup();                 // Clean up Winsock
        return 1;
    }

    // Print a message indicating that the server is listening for incoming connections
    std::cout << "Server is listening on port " << DEFAULT_PORT << std::endl;

    string input;

    while (true) {
    // Accept a client connection
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket); // Close the listening socket
        WSACleanup();              // Clean up Winsock
        return 1;                  // Exit if accept fails
    }

    // Buffer for receiving data from the client
    char recvbuf[BUFFER_SIZE];
    int recvbuflen = BUFFER_SIZE;

    // Receive data from the client
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        std::string request(recvbuf, iResult); // Convert received data to a string

        // Step 1: Check if the request method is OPTIONS
        if (request.find("OPTIONS") == 0) {
            // Step 2: Send a CORS preflight response and return immediately
            std::string optionsResponse = "HTTP/1.1 204 No Content\r\n"
                                          "Access-Control-Allow-Origin: *\r\n"
                                          "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                                          "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
            send(ClientSocket, optionsResponse.c_str(), optionsResponse.size(), 0); // Send preflight response
            closesocket(ClientSocket); // Close client connection after handling preflight
            continue;                  // Move to the next client request
        }

        // Process and generate a response for the client request
        std::string response = handleRequest(request);

        // Send the response, including CORS headers, for regular requests
        send(ClientSocket, response.c_str(), response.size(), 0);
    }

    // Close the client socket after handling the request
    closesocket(ClientSocket);
}

// Clean up: Close the listening socket and clean up Winsock
closesocket(ListenSocket);
WSACleanup();
return 0;
}
