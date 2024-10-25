#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <set>
#include <chrono>

using namespace std;

// Function prototypes
vector<string> tokenize(const string& text);
void findCommonPhrases(const vector<string>& words, int N, unordered_map<string, int>& phraseFreq);
vector<pair<string, int>> getTopPhrases(const unordered_map<string, int>& phraseFreq);
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const vector<string>& targetWords);

// Function to tokenize text into words
vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        // Clean word from punctuation
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (!word.empty()) { // Avoid adding empty words
            words.push_back(word);
        }
    }
    return words;
}

// Function to find most common phrases of length N
void findCommonPhrases(const vector<string>& words, int N, unordered_map<string, int>& phraseFreq) {
    for (size_t i = 0; i <= words.size() - N; ++i) {
        string phrase;
        for (int j = 0; j < N; ++j) {
            phrase += words[i + j] + " ";
        }
        phraseFreq[phrase]++;
    }
}

// Function to get top 10 phrases
vector<pair<string, int>> getTopPhrases(const unordered_map<string, int>& phraseFreq) {
    vector<pair<string, int>> phrases(phraseFreq.begin(), phraseFreq.end());
    sort(phrases.begin(), phrases.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; // Sort by frequency
    });
    if (phrases.size() > 10) {
        phrases.resize(10); // Keep only top 10
    }
    return phrases;
}

// Function to find the most similar paragraphs
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const vector<string>& targetWords) {
    vector<pair<string, int>> similarities;

    for (const string& paragraph : paragraphs) {
        auto words = tokenize(paragraph);
        set<string> paragraphWords(words.begin(), words.end()); // Unique words in paragraph
        int commonCount = 0;

        for (const string& target : targetWords) {
            if (paragraphWords.find(target) != paragraphWords.end()) {
                commonCount++;
            }
        }

        similarities.emplace_back(paragraph, commonCount);
    }

    // Sort by number of common words (similarity)
    sort(similarities.begin(), similarities.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    if (similarities.size() > 10) {
        similarities.resize(10); // Keep only top 10
    }

    return similarities;
}

int main() {
    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Read files
    ifstream tomFile("TomSawyer.txt");
    ifstream huckFile("HuckleberryFinn.txt");
    
    if (!tomFile.is_open() || !huckFile.is_open()) {
        cerr << "Error opening files." << endl;
        return 1;
    }
    
    string tomText((istreambuf_iterator<char>(tomFile)), istreambuf_iterator<char>());
    string huckText((istreambuf_iterator<char>(huckFile)), istreambuf_iterator<char>());
    
    // Tokenize text into words
    vector<string> tomWords = tokenize(tomText);
    vector<string> huckWords = tokenize(huckText);
    
    // Prepare to store phrase frequencies
    ofstream phrasesFile("TopPhrases.txt");
    if (!phrasesFile.is_open()) {
        cerr << "Error opening TopPhrases.txt for writing." << endl;
        return 1;
    }

    for (int N = 1; N <= 10; ++N) {
        unordered_map<string, int> tomPhrases;
        unordered_map<string, int> huckPhrases;

        // Find phrases for both novels
        findCommonPhrases(tomWords, N, tomPhrases);
        findCommonPhrases(huckWords, N, huckPhrases);
        
        // Get top 10 phrases
        auto tomTop = getTopPhrases(tomPhrases);
        auto huckTop = getTopPhrases(huckPhrases);

        // Output results to phrases file
        phrasesFile << "Top 10 Most Frequent Phrases of Length " << N << ":\n";
        phrasesFile << left << setw(30) << "Phrase" << setw(25) << "Frequency in Tom Sawyer" << "Frequency in Huckleberry Finn\n";

        for (size_t i = 0; i < 10; ++i) {
            string tomPhrase = (i < tomTop.size()) ? tomTop[i].first : "";
            string huckPhrase = (i < huckTop.size()) ? huckTop[i].first : "";
            int tomFreq = (i < tomTop.size()) ? tomTop[i].second : 0;
            int huckFreq = (i < huckTop.size()) ? huckTop[i].second : 0;
            phrasesFile << left << setw(30) << tomPhrase << setw(25) << tomFreq << huckFreq << "\n";
        }
        phrasesFile << "\n";
    }

    // Find similar paragraphs (Add your implementation here)
    
    // Closing files and return
    phrasesFile.close();

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // Print the elapsed time in seconds
    cout << "Elapsed time: " << elapsed.count() << " seconds." << endl;

    return 0; // Indicate successful completion
}