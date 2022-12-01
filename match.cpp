//
//  main.cpp
//  Project 5
//
//  Created by Angela Kan on 11/20/20.
//

#include <iostream>
#include <cassert>
#include <cstring>
#include <cctype>

using namespace std;

const int MAX_WORD_LENGTH = 20;
const int MAX_DOC_LENGTH = 250+2;   //length of doc (250) + space padding for beginning+end of doc

int cleanupRules(char wordin[][MAX_WORD_LENGTH+1],
                 char wordout[][MAX_WORD_LENGTH+1],
                 int nRules);

int determineScore(const char document[],
                   const char wordin[][MAX_WORD_LENGTH+1],
                   const char wordout[][MAX_WORD_LENGTH+1],
                   int nRules);

int main() {
    const int TEST1_NRULES = 12;
    char test1win[TEST1_NRULES][MAX_WORD_LENGTH+1] = {
                    "confusion", "FAMILY", "charm", "hearty", "house", "worn-out", "family", "charm", "ties", "", "charm", "FaMiLy"
                };
    char test1wout[TEST1_NRULES][MAX_WORD_LENGTH+1] = {
                    "", "TIES", "confusion", "hearty", "intrigue", "younger", "first", "", "family", "frightened", "", "tIeS"
                };
    assert(cleanupRules(test1win, test1wout, 12) == 6);

    const int TEST2_NRULES = 12;
    char test2win[TEST2_NRULES][MAX_WORD_LENGTH+1] = {
                    "hearty", "confusion", "charm", "FAMILY", "house", "worn-out", "family", "charm", "ties", "", "charm", "FaMiLy"
                };
    char test2wout[TEST2_NRULES][MAX_WORD_LENGTH+1] = {
                    "hear-ty", "", "confusion", "TIES", "intrigue", "younger", "first", "", "family", "frightened", "", "tIeS"
                };
    assert(cleanupRules(test2win, test2wout, 12) == 6);
    assert(cleanupRules(test2win, test2wout, -1) == 0);
    assert(cleanupRules(test2win, test2wout, 0) == 0);
    
    char test3win[7][MAX_WORD_LENGTH+1] = {
                    "hOW", "how", "how", "FAMILY", "house", "how", "house"
                };
    char test3wout[7][MAX_WORD_LENGTH+1] = {
                    "hearty", "confusion", "", "TIES", "intrigue", "hearty", ""
                };
    assert(cleanupRules(test3win, test3wout, 7) == 3);
    
    char test4win[6][MAX_WORD_LENGTH+1] = {
                    "who", "confusion", "charm", "FAMILY", "house", "charm"
                };
    char test4wout[6][MAX_WORD_LENGTH+1] = {
                    "who", ";h", "", "TIES", "intrigue", "younger"
                };
    assert(cleanupRules(test4win, test4wout, 6) == 3);
    
    char test5win[3][MAX_WORD_LENGTH+1] = {
                    "", "mzs:", "charm"
                };
    char test5wout[3][MAX_WORD_LENGTH+1] = {
                    "", "", "charm"
                };
    assert(cleanupRules(test5win, test5wout, 3) == 0);
    
    const int TESTa_NRULES = 3;
    char testawin[TESTa_NRULES][MAX_WORD_LENGTH+1] = {
                    "family", "unhappy", "horse",
                };
    char testawout[TESTa_NRULES][MAX_WORD_LENGTH+1] = {
                    "",       "horse",   "",
                };
    assert(determineScore("Happy families are all alike; every unhappy family is unhappy in its own way.",
                            testawin, testawout, TESTa_NRULES) == 2);
    assert(determineScore("Happy horses are all alike; every unhappy horse is unhappy in its own way.",
                            testawin, testawout, TESTa_NRULES-1) == 0);
    assert(determineScore("Happy horses are all alike; every unhappy horse is unhappy in its own way.",
                            testawin, testawout, TESTa_NRULES) == 1);
    assert(determineScore("A horse!  A horse!  My kingdom for a horse!",
                            testawin, testawout, TESTa_NRULES) == 1);
    assert(determineScore("horse:stable ratio is 10:1",
                            testawin, testawout, TESTa_NRULES) == 0);
    assert(determineScore("**** 2020 ****",
                            testawin, testawout, TESTa_NRULES) == 0);
    
    assert(determineScore("d", testawin, testawout, 0) == 0);
    assert(determineScore("d", testawin, testawout, -1) == 0);
    assert(determineScore("d", testawin, testawout, 0) == 0);
    
    char testbwin[4][MAX_WORD_LENGTH+1] = {
                    "family", "hi", "wow", "whoooooooooooooooooo"
                };
    char testbwout[4][MAX_WORD_LENGTH+1] = {
                    "",       "horse",   "", "",
                };
    assert(determineScore("FAMILy first", testbwin, testbwout, 4) == 1);
    assert(determineScore("family family family       first", testbwin, testbwout, 3) == 1);
    assert(determineScore("", testbwin, testbwout, 4) == 0);
    assert(determineScore("famiLya fir;;;;d wow;;;;;", testbwin, testbwout, 4) == 1);
    assert(determineScore("hi      man", testbwin, testbwout, 4) == 1);
    assert(determineScore("                ", testbwin, testbwout, 4) == 0);
    assert(determineScore("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------", testbwin, testbwout, 4) == 0);
    assert(determineScore("whoooooooooooooooooo", testbwin, testbwout, 4) == 1);

    cout << "All tests succeeded." << endl;
}

//shifts all elements to the left for BOTH win and wout arrays starting with the element one to the right of the given int position
void rotateLeft(char a[][MAX_WORD_LENGTH+1], char b[][MAX_WORD_LENGTH+1], int n, int pos){
    for (int i=pos; i<n - 1; i++){  //shifts all elements after specified pos one to the left
        strcpy(a[i], a[i+1]);
        strcpy(b[i], b[i+1]);
    } //if pos is 1 less than n, then it will skip the for loop, and  nRules-- in cleanupRules so n-1 gets "cut off"
}

//removes rules that aren't in clean form by using rotateLeft
int cleanupRules(char wordin[][MAX_WORD_LENGTH+1],
                 char wordout[][MAX_WORD_LENGTH+1],
                 int nRules){
    if (nRules <= 0){
        return 0;
    } else {
        //transform every upper case letter in the match rule words into lower case equivalent
        for (int i=0; i<nRules; i++){
            for (int j=0; wordin[i][j]!='\0'; j++){
                wordin[i][j] = tolower(wordin[i][j]);
            }
            for (int k=0; wordout[i][k]!='\0'; k++){
                wordout[i][k] = tolower(wordout[i][k]);
            }
        }
        //remove match rule if word in wordIN array contains a char that's not a letter
        for (int i=0; i<nRules; i++){
            for (int j=0; wordin[i][j]!='\0'; j++){     //iterates through each letter of each string in the array
                if (!isalpha(wordin[i][j])){
                    rotateLeft(wordin, wordout, nRules, i);
                    nRules--;       //updates nRules after removing a match rule
                    i--;    //updates i to re-check index (after rotating a new rule over into that same index)
                    break;     //ensures i++ to re-check the new word at the same index
                }
            }
        }
        //remove match rule if word in wordOUT array contains a char that's not a letter (same process as above loop)
        for (int i=0; i<nRules; i++){
            for (int k=0; wordout[i][k]!='\0'; k++){
                if (!isalpha(wordout[i][k])){
                    rotateLeft(wordin, wordout, nRules, i);
                    nRules--;
                    i--;
                    break;
                }
            }
        }
        //removes match rule if a w-in word is the empty string
        for (int i=0; i<nRules; i++){
            if (strcmp(wordin[i], "") == 0){    //checks if w-in word at index is equal to empty string
                rotateLeft(wordin, wordout, nRules, i);
                nRules--;
                i--; //i will automatically jump back to i++ in the for loop
            }
        }
        //removes match rule if w-in word equals w-out word
        for (int i=0; i<nRules; i++){
            if (strcmp(wordin[i], wordout[i]) == 0){    //compares word-in and word-out at given index
                rotateLeft(wordin, wordout, nRules, i);
                nRules--;
                i--;
            }
        }
        //removes match rule if duplicate w-in word (compared to one-word rule)
        for (int i=0; i<nRules; i++){
            for (int j=0; j<nRules; j++){
                //IF one word rule, check w/ all the OTHER rules in the array!
                if (strcmp(wordout[i], "") == 0 && strcmp(wordin[i], wordin[j]) == 0 && i != j){
                    rotateLeft(wordin, wordout, nRules, j);
                    nRules--;
                    j--;    //continues checking a NEW word at the same index against the one word w-in word specified
                    if (j<i){   //if the removed word is at an index less than the w-in word we're checking against:
                        i--; //update the index of the SAME w-in word we're checking against (i iterates forwards here (i++) after j finishes its loop, aka after we've checked all other words against the specified w-in word at i)
                    }
                }
            }
        }
        //removes duplicate two-word match rules if both w-in and w-out are identical to first two-word match rule
        for (int i=0; i<nRules; i++){
            for (int j=1; (i+j)<nRules; j++){
                //checks all two-word rule words AFTER the specified two-word rule w-in word at i
                if (strcmp(wordout[i], "") != 0 && strcmp(wordin[i], wordin[i+j]) == 0 && strcmp(wordout[i], wordout[i+j]) == 0){   //if both w-win and w-out respectively match for two match rules:
                    rotateLeft(wordin, wordout, nRules, (i+j));  //remove duplicate match rule at an index after the first one
                    nRules--;
                    j--;    //continues checking the new updated words at the same index against the two word rule words specified at i
                }
            }
        }
    }
    return nRules;
}

//returns score of the document based on specified clean match rules
int determineScore(const char document[],
                   const char wordin[][MAX_WORD_LENGTH+1],
                   const char wordout[][MAX_WORD_LENGTH+1],
                   int nRules){
    if (nRules <= 0){
        return 0;
    } else {
        int score = 0;
        
        //creates a copy of doc with two extra spaces, one at start and one at end of doc
        char doc[MAX_DOC_LENGTH+1] = " ";
        strcat(doc, document);
        strcat(doc, " ");
        
        //changes all uppercase letters to lowercase in doc and removes all non-alpha/space characters
        for (int i=0; doc[i]!='\0'; i++){
            doc[i] = tolower(doc[i]);
            if (!islower(doc[i]) && doc[i] != ' '){
                for (int j=i; doc[j]!='\0'; j++){
                    doc[j] = doc[j+1];  //shifts all char after the non-alpha/space char over one to left, replacing that non-alpha/space char
                }
                i--;    //updates index after shifting all characters over one to re-check the new updated char at index i
            }
        }
        
        //checks doc for empty string
        if (doc[2] == '\0'){
            return 0;
        }
        
        //checks doc against set of match rules to determine score
        for (int i=0; i<nRules; i++){
            //creates a temp string for current w-in word at specified index i with space at beginning and end of word
            char tempStrIn[MAX_WORD_LENGTH+2+1] = " ";
            strcat(tempStrIn, wordin[i]);
            strcat(tempStrIn, " ");
            
            //searches doc for temp string (w-in word with spaces)
            if (strstr(doc, tempStrIn) != NULL){
                if (strcmp(wordout[i], "") == 0){
                    score++;    //adds one to score if temp string (w-in word) is present and no corresponding w-out word
                } else { //if there is a w-out word for that rule:
                    //creates a temp string for current w-out word at specified index i with space at beginning and end of word
                    char tempStrOut[MAX_WORD_LENGTH+2+1] = " ";
                    strcat(tempStrOut, wordout[i]);
                    strcat(tempStrOut, " ");
                    
                    if (strstr(doc, tempStrOut) == NULL){
                        score++;    //adds one to score if temp string (w-out word) isn't present
                    }
                }
            }
        }
        return score;
    }
}
