#include <iostream>
#include <fstream>
#include <sstream>    // string stream operations
#include <unordered_map>
#include <vector>   // dynamic arrays
#include <algorithm>
#include <string>
#include <iomanip>
#include <set>      // sets use
#include <chrono>  // measuring elapsed time

using namespace std;

// function prototypes
vector<string> tokenize(const string& text);
void findCommonPhrases(const vector<string>& words, int N, unordered_map<string, int>& phraseFreq);
vector<pair<string, int>> getTopPhrases(const unordered_map<string, int>& phraseFreq);
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const set<string>& targetWords);
vector<string> readParagraphs(const string& filename);
vector<string> readFile(const string& filename);

// function -> read the contents of file into a string
vector<string> readFile(const string& filename) {
    ifstream file(filename);    // open files
    if (!file.is_open()) {
        cerr << "Error opening " << filename << endl;         // safe case
        return {};
    }
    stringstream buffer;
    buffer << file.rdbuf(); // read file into stringstream
    return { buffer.str() }; // return as vector of one string
}

// function -> tokenize text into words
vector<string> tokenize(const string& text) { 
    vector<string> words;     // store words
    stringstream ss(text);   // stringstream from the text
    string word;
    while (ss >> word) {    //extract words
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (!word.empty()) {                // safe case against adding empty words
            words.push_back(word);
        }
    }
    return words;   // lsit of words
}

// function -> find most common phrases of length N
void findCommonPhrases(const vector<string>& words, int N, unordered_map<string, int>& phraseFreq) {
    for (size_t i = 0; i <= words.size() - N; ++i) {
        string phrase;
        for (int j = 0; j < N; ++j) {
            phrase += words[i + j] + " ";      // construct phrase from N consecutive words
        }
        phraseFreq[phrase]++;      // increment frequency of phrase
    }
}

// function -> top 10 phrases
vector<pair<string, int>> getTopPhrases(const unordered_map<string, int>& phraseFreq) {
    vector<pair<string, int>> phrases(phraseFreq.begin(), phraseFreq.end());      // copy phrases to vector
    sort(phrases.begin(), phrases.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second; // sort by frequency
    });
    if (phrases.size() > 10) {
        phrases.resize(10); // only grab top10
    }
    return phrases; // return top 10
}

// function -> read paragraphs from file
vector<string> readParagraphs(const string& filename) {
    ifstream file(filename);  //open
    if (!file.is_open()) {
        cerr << "Error opening " << filename << endl;   // safe case
        return {};
    }
    vector<string> paragraphs; // store paragraphs
    string paragraph, line;
    while (getline(file, line)) {
        if (line.empty() && !paragraph.empty()) {          // new paragraph
            paragraphs.push_back(paragraph);        // store completed paragraph
            paragraph.clear();         // clear string for next paragraph
        } else {
            paragraph += line + " ";
        }
    }
    if (!paragraph.empty()) { // last paragraph
        paragraphs.push_back(paragraph);
    }
    return paragraphs;  // list of paragraphjs
}

// function -> find the most similar paragraphs
vector<pair<string, int>> findSimilarParagraphs(const vector<string>& paragraphs, const set<string>& targetWords) {
    vector<pair<string, int>> similarities;       // vector -> store paragraph and similarity count

    for (const string& paragraph : paragraphs) {
        auto words = tokenize(paragraph);
        set<string> paragraphWords(words.begin(), words.end()); // unique words in paragraph

        // count common words
        int commonCount = 0;
        for (const string& word : paragraphWords) {
            if (targetWords.find(word) != targetWords.end()) {
                commonCount++;
            }
        }

        similarities.emplace_back(paragraph, commonCount);
    }

    // sort by # of common words/ similarity
    sort(similarities.begin(), similarities.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;    // sort
    });

    if (similarities.size() > 10) {
        similarities.resize(10); // keep only top 10
    }

    return similarities; // return sililar paragraphs
}

int main() {
    // start the timer
    auto start = chrono::high_resolution_clock::now();

    // read files
    string tomText = readFile("TomSawyer.txt")[0];
    string huckText = readFile("HuckleberryFinn.txt")[0];
    vector<string> longParagraphs = readParagraphs("LongParagraph.txt");

    // prepare to store phrase frequencies
    ofstream phrasesFile("TopPhrases.txt");     // output file
    if (!phrasesFile.is_open()) {
        cerr << "Error opening TopPhrases.txt for writing." << endl;        //safe case
        return 1;
    }

    // tokenize text into words
    vector<string> tomWords = tokenize(tomText);
    vector<string> huckWords = tokenize(huckText);

    for (int N = 1; N <= 10; ++N) {
        unordered_map<string, int> tomPhrases;
        unordered_map<string, int> huckPhrases;

        // find phrases for both novels
        findCommonPhrases(tomWords, N, tomPhrases);
        findCommonPhrases(huckWords, N, huckPhrases);

        // get top 10 phrases
        auto tomTop = getTopPhrases(tomPhrases);
        auto huckTop = getTopPhrases(huckPhrases);

        // output results to phrases file
        phrasesFile << "Top 10 Most Frequent Phrases of Length " << N << ":\n";
        phrasesFile << left << setw(30) << "Phrase" << setw(25) << "Frequency in Tom Sawyer" << "Frequency in Huckleberry Finn\n";

        for (size_t i = 0; i < 10; ++i) {
            string tomPhrase = (i < tomTop.size()) ? tomTop[i].first : "";   //get phrase
            string huckPhrase = (i < huckTop.size()) ? huckTop[i].first : "";
            int tomFreq = (i < tomTop.size()) ? tomTop[i].second : 0;   // get frequency
            int huckFreq = (i < huckTop.size()) ? huckTop[i].second : 0;
            phrasesFile << left << setw(30) << tomPhrase << setw(25) << tomFreq << huckFreq << "\n";   // write to file
        }
        phrasesFile << "\n";    //styling
    }

    // find similar paragraphs
    set<string> targetWords;
    for (const string& longParagraph : longParagraphs) {
        auto words = tokenize(longParagraph);
        targetWords.insert(words.begin(), words.end());
    }

    vector<string> tomParagraphs = readParagraphs("TomSawyer.txt");
    vector<string> huckParagraphs = readParagraphs("HuckleberryFinn.txt");

    vector<pair<string, int>> tomSimilar = findSimilarParagraphs(tomParagraphs, targetWords);
    vector<pair<string, int>> huckSimilar = findSimilarParagraphs(huckParagraphs, targetWords);

    // write similar paragraphs to TopParagraphs.txt
    ofstream paragraphsFile("TopParagraphs.txt");    // outut file
    if (!paragraphsFile.is_open()) {        
        cerr << "Error opening TopParagraphs.txt for writing." << endl;       // safe case
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

    // close files
    phrasesFile.close();
    paragraphsFile.close();

    // stop the timer
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // print the elapsed time in seconds
    cout << "Elapsed time: " << elapsed.count() << " seconds." << endl;

    return 0; 
}
