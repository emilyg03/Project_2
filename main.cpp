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
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const set<string>& targetWords);
vector<string> readParagraphs(const string& filename);
vector<string> readFile(const string& filename);

// Function to read the contents of a file into a string
vector<string> readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening " << filename << endl;
        return {};
    }
    stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file into a stringstream
    return { buffer.str() }; // Return as a vector of one string
}

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
    sort(phrases.begin(), phrases.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second; // Sort by frequency
    });
    if (phrases.size() > 10) {
        phrases.resize(10); // Keep only top 10
    }
    return phrases;
}

// Function to read paragraphs from a file
vector<string> readParagraphs(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening " << filename << endl;
        return {};
    }
    vector<string> paragraphs;
    string paragraph, line;
    while (getline(file, line)) {
        if (line.empty() && !paragraph.empty()) { // New paragraph
            paragraphs.push_back(paragraph);
            paragraph.clear();
        } else {
            paragraph += line + " ";
        }
    }
    if (!paragraph.empty()) { // Last paragraph
        paragraphs.push_back(paragraph);
    }
    return paragraphs;
}

// Function to find the most similar paragraphs
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const set<string>& targetWords) {
    vector<pair<string, int>> similarities;

    for (const string& paragraph : paragraphs) {
        auto words = tokenize(paragraph);
        set<string> paragraphWords(words.begin(), words.end()); // Unique words in paragraph

        // Count common words
        int commonCount = 0;
        for (const string& word : paragraphWords) {
            if (targetWords.find(word) != targetWords.end()) {
                commonCount++;
            }
        }

        similarities.emplace_back(paragraph, commonCount);
    }

    // Sort by number of common words (similarity)
    sort(similarities.begin(), similarities.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });

    if (similarities.size() > 10) {
        similarities.resize(10); // Keep only top 10
    }

    return similarities;
}

int main() {
    // Start the timer
    auto start = chrono::high_resolution_clock::now();

    // Read files
    string tomText = readFile("TomSawyer.txt")[0];
    string huckText = readFile("HuckleberryFinn.txt")[0];
    vector<string> longParagraphs = readParagraphs("LongParagraph.txt");

    // Prepare to store phrase frequencies
    ofstream phrasesFile("TopPhrases.txt");
    if (!phrasesFile.is_open()) {
        cerr << "Error opening TopPhrases.txt for writing." << endl;
        return 1;
    }

    // Tokenize text into words
    vector<string> tomWords = tokenize(tomText);
    vector<string> huckWords = tokenize(huckText);

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

    // Find similar paragraphs
    set<string> targetWords;
    for (const string& longParagraph : longParagraphs) {
        auto words = tokenize(longParagraph);
        targetWords.insert(words.begin(), words.end());
    }

    vector<string> tomParagraphs = readParagraphs("TomSawyer.txt");
    vector<string> huckParagraphs = readParagraphs("HuckleberryFinn.txt");

    vector<pair<string, int>> tomSimilar = findSimilarParagraphs(tomParagraphs, targetWords);
    vector<pair<string, int>> huckSimilar = findSimilarParagraphs(huckParagraphs, targetWords);

    // Write similar paragraphs to TopParagraphs.txt
    ofstream paragraphsFile("TopParagraphs.txt");
    if (!paragraphsFile.is_open()) {
        cerr << "Error opening TopParagraphs.txt for writing." << endl;
        return 1;
    }

    paragraphsFile << "Top 10 Similar Paragraphs from Tom Sawyer (Similarity based on common word count):\n";
    for (size_t i = 0; i < tomSimilar.size(); ++i) {
        paragraphsFile << i + 1 << ". (Common words: " << tomSimilar[i].second << ") " << tomSimilar[i].first << "\n";
    }

    paragraphsFile << "\nTop 10 Similar Paragraphs from Huckleberry Finn (Similarity based on common word count):\n";
    for (size_t i = 0; i < huckSimilar.size(); ++i) {
        paragraphsFile << i + 1 << ". (Common words: " << huckSimilar[i].second << ") " << huckSimilar[i].first << "\n";
    }

    // Closing files
    phrasesFile.close();
    paragraphsFile.close();

    // Stop the timer
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Print the elapsed time in seconds
    cout << "Elapsed time: " << elapsed.count() << " seconds." << endl;

    return 0; // Indicate successful completion
}
