#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

using namespace std;

// Function to tokenize text into words
vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        // Clean word from punctuation
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        words.push_back(word);
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

int main() {
    string tomSawyerFile = "TomSawyer.txt"; // Input files
    string huckleberryFinnFile = "HuckleberryFinn.txt";
    
    // Read files
    ifstream tomFile(tomSawyerFile);
    ifstream huckFile(huckleberryFinnFile);
    
    string tomText((istreambuf_iterator<char>(tomFile)), istreambuf_iterator<char>());
    string huckText((istreambuf_iterator<char>(huckFile)), istreambuf_iterator<char>());
    
    // Tokenize text into words
    vector<string> tomWords = tokenize(tomText);
    vector<string> huckWords = tokenize(huckText);
    
    // Prepare to store phrase frequencies
    for (int N = 1; N <= 10; ++N) {
        unordered_map<string, int> tomPhrases;
        unordered_map<string, int> huckPhrases;

        // Find phrases for both novels
        findCommonPhrases(tomWords, N, tomPhrases);
        findCommonPhrases(huckWords, N, huckPhrases);
        
        // Get top 10 phrases
        auto tomTop = getTopPhrases(tomPhrases);
        auto huckTop = getTopPhrases(huckPhrases);

        // Output results to a file
        ofstream outputFile("TopPhrases.txt", ios::app);
        outputFile << "Top 10 Most Frequent Phrases of Length " << N << ":\n";
        outputFile << left << setw(30) << "Phrase" << setw(25) << "Frequency in Tom Sawyer" << "Frequency in Huckleberry Finn\n";

        for (size_t i = 0; i < 10; ++i) {
            string tomPhrase = (i < tomTop.size()) ? tomTop[i].first : "";
            string huckPhrase = (i < huckTop.size()) ? huckTop[i].first : "";
            int tomFreq = (i < tomTop.size()) ? tomTop[i].second : 0;
            int huckFreq = (i < huckTop.size()) ? huckTop[i].second : 0;
            outputFile << left << setw(30) << tomPhrase << setw(25) << tomFreq << huckFreq << "\n";
        }
        outputFile << "\n";
    }

    return 0;
}
