// References: https://en.cppreference.com/w/cpp/regex, https://www.softwaretestinghelp.com/regex-in-cpp/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <regex>
using namespace std;

// List of C++ keywords, operators, and seperators
vector<string> keywords = { "int", "void", "return", "if", "else", "while", "for" };
vector<string> operators = { "=", ">", "<<", ">>", "+", "-", "*", "/", "==", "!=", "<=", ">=", "&&", "||", "!", "++", "--" };
vector<char> separators = { '{', '}', '(', ')', ';', ',' };

// Function to read, open, and close the input file
vector<string> readFile(const string& filename) {
    vector<string> lines;
    string line;
    ifstream file(filename);

    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }
    return lines;
}

// Function to clean the comments from the code
vector<string> removeComments(const vector<string>& lines, vector<string>& comments) {
    vector<string> cleanLines;
    bool insideMultiLineComment = false;

    // For single line comments, "//" matches with literal character //, .* is to match characters that follow until the end of the line
    regex slCommentPattern ("//.*");
    // For multiline comments, "/" is for the literal character /, "\\*" is for the * character for the beginning of the line 
    regex multiLineStartPattern ("/\\*");
    // This is for the end of the line
    regex multiLineEndPattern ("\\*/");

    for (string line : lines) {
        // Search for matches found by regular expression search
        smatch match;
        // Searches for expression that match single-line comments 
        if (regex_search(line, match, slCommentPattern)) {
            comments.push_back(match.str());
        // Removes single-line comments
            line = regex_replace(line, slCommentPattern, "");
        }

        // Searches for parsers inside if not found it will be executed
        if (insideMultiLineComment) {
            if (regex_search(line, match, multiLineEndPattern)) {
                insideMultiLineComment = false;
                // Removes */ from the line
                line = regex_replace(line, multiLineEndPattern, ""); 
            }
            // Skips remainder of current loop and moves onto the next
            continue; 
        }   
        // Makes sure the output of the code does not contain coments 
        else if (regex_search(line, match, multiLineStartPattern)) {
            insideMultiLineComment = true; 
            comments.push_back(match.str());
            line = regex_replace(line, multiLineStartPattern, "");
        }
        // Removes comments or whitespaces
        if (!line.empty()) {
            cleanLines.push_back(line); 
        }
    }
    // Returns without comments
    return cleanLines;
}

// Reference: https://www.geeksforgeeks.org/write-regular-expressions/#
// Function to classify tokens and store into seperate vectors for output
void classifyTokens(const vector<string>& lines, const vector<string>& comments) {
    vector<string> keywordTokens, identifierTokens, literalTokens, operatorTokens, separatorTokens;

    // Searches for lowercase letters or underscore and any following characters with lowercase and numbers
    regex identifierPattern("[a-z_][a-z0-9_]*");
    // Matches string litersl in double quotes, "\\b[0-9]+\\.?[0-9]*\" indicates literals, integers and floating-point numbers, \\b makes sure the number is not part of a different string
    regex literalPattern("\".*?\"|\\b[0-9]+\\.?[0-9]*\\b");
    // Matches relational, arithmetic, and assignment operators
    regex operatorPattern("==|!=|<=|>=|[-+*/=<>]+");
    // Matches seperators
    regex separatorPattern("[{}()\\[\\];,]");

// Reference: https://stackoverflow.com/questions/34229328/writing-a-very-simple-lexical-analyser-in-c
    // Creates copy of current line to remove or replace matched literals
    for (const string& line : lines) {
        string processedLine = line; 

        auto literal_begin = sregex_iterator(line.begin(), line.end(), literalPattern);
        auto words_end = sregex_iterator();

        // Searches current line for matches in the literalPattern
        for (sregex_iterator i = literal_begin; i != words_end; ++i) {
            // Extracts matched literls to literalToken
            string literalToken = (*i).str();
            // Keeps track of literals
            literalTokens.push_back(literalToken);
            // Finds start of literalToken
            size_t startPos = processedLine.find(literalToken);
            // Ensures literals are classified correctly 
            if (startPos != string::npos) {
                processedLine.replace(startPos, literalToken.length(), string(literalToken.length(), ' '));
            }
        }

        auto words_begin = sregex_iterator(processedLine.begin(), processedLine.end(), identifierPattern);
        // Searches for identifiers or keywords 
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            string token = (*i).str();
            // Makes sure the token is either a keyword or identifier in the lists
            if (find(keywords.begin(), keywords.end(), token) != keywords.end()) {
                keywordTokens.push_back(token);
            }
            else {
                identifierTokens.push_back(token);
            }
        }

// old code that did not work because it would count words inside " " as identifiers as well and not just literals. 
// this only found key words and identifiers, it did took into account the words within the "  ", which caused my output to be a larger number than the expected output
/* for (sregex_iterator i = words_begin; i != words_end; ++i) {
        string token = (*i).str();
        if (find(keywords.begin(),keywords.end(), token) != keywords.end()) {
            keywordTokens.push_back(token);
        } else {
            identifierTokens.push_back(token);
        }
   }*/
// auto literals_begin = sregex_iterator(line.begin(), line.end(), literalPattern);
// for (sregex_iterator i = literals_begin; i != words_end; ++i) {
//     literalTokens.push_back((*i).str());
// }

        // Iterator to check for operators from the beginning to end of the line
        auto operators_begin = sregex_iterator(line.begin(), line.end(), operatorPattern);
        // Loops through the line to find any matches of operators and converts it into a string stored in operatorTokens
        for (sregex_iterator i = operators_begin; i != words_end; ++i) {
            operatorTokens.push_back((*i).str());
        }

        // Iterator to check for separators from the beginning to end of the line 
        auto separators_begin = sregex_iterator(line.begin(), line.end(), separatorPattern);
        // Loops through the line to find any matches of seperators and converts it into a string stored in seperatorTokens
        for (sregex_iterator i = separators_begin; i != words_end; ++i) {
            separatorTokens.push_back((*i).str());
        }
    }

    // Prints Tokens
    cout << "Tokens:" << endl;

    // Prints Keywords
    cout << "Keywords: ";
    for (const string& token : keywordTokens) {
        cout << token << ' ';
    }
    cout << endl;

    // Prints Identifiers
    cout << "Identifiers: ";
    for (const string& token : identifierTokens) {
        cout << token << ' ';
    }
    cout << endl;

    // Prints Literals
    cout << "Literals: ";
    for (const string& token : literalTokens) {
        cout << token << ' ';
    }
    cout << endl;

    // Prints Operators
    cout << "Operators: ";
    for (const string& token : operatorTokens) {
        cout << token << ' ';
    }
    cout << endl;

    // Prints Separators
    cout << "Separators: ";
    for (const string& token : separatorTokens) {
        cout << token << ' ';
    }
    cout << endl;

    // Prints Comments
    cout << "Comments: ";
    for (const string& comment : comments) {
        cout << comment << ' ';
    }
    cout << endl;

    // Adds total of each token and prints output
    int totalTokens = keywordTokens.size() + identifierTokens.size() + literalTokens.size() + operatorTokens.size() + separatorTokens.size();
    cout << "Total : " << totalTokens << endl;
}

int main() {
    // Varaible for file name
    string inputFile1 = "input_code1.txt";
    string inputFile2 = "input_code2.txt";

    // Reads the input file for the first input text 
    vector<string> codeLines1 = readFile(inputFile1);

    // Remove comments to store into comments1 and return without comments, only classifies keywords, identifiers, literals, and operators 
    vector<string> comments1;
    vector<string> cleanCode1 = removeComments(codeLines1, comments1);

    // Classifies the tokens and catergorizes them 
    classifyTokens(cleanCode1, comments1);

    cout << '\n' << endl;

    // For the second text file: reads input file, removes comments, and classifies tokens 
    vector<string> codeLines2 = readFile(inputFile2);
    vector<string> comments2;
    vector<string> cleanCode2 = removeComments(codeLines2, comments2);
    classifyTokens(cleanCode2, comments2);

    return 0;
}