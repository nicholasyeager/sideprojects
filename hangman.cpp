#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;


void seedVectors(vector<string> &easyWords, vector<string> &mediumWords, vector<string> &expertWords){
    easyWords.at(0) = "park";
    easyWords.at(1) = "person";
    easyWords.at(2) = "rainy";
    easyWords.at(3) = "fire";
    easyWords.at(4) = "leaf";

    mediumWords.at(0) = "transistor";
    mediumWords.at(1) = "bashful";
    mediumWords.at(2) = "conditioner";
    mediumWords.at(3) = "pillage";
    mediumWords.at(4) = "menacing";

    expertWords.at(0) = "auspicious";
    expertWords.at(1) = "zephyr";
    expertWords.at(2) = "sphinx";
    expertWords.at(3) = "jazz";
    expertWords.at(4) = "supercalifragilisticexpialidocious";
}

void resetGuess(vector<char> &guess, string Word){
    guess.resize(Word.length());
    for (int i = 0; i < guess.size(); i++){
        guess.at(i) = '_';
    }
}

void printGuess(vector<char> guess){
    for (int i = 0; i < guess.size(); i++){
        cout << guess.at(i);
    }
    cout << endl;
}

void checkWord(vector<char> &guess, char input, int *guesses, string Word, int *missingLetters){
    bool found = false;
    for (int i = 0; i < guess.size(); i++){
        if (Word.at(i) == input){
            guess.at(i) = input;
            (*missingLetters)--;
            found = true;
        }
    }
    if (!found){
        (*guesses)--;
    }
}

int main(){
    string Word;
    char input;
    int missingLetters = 0;

    vector<string> easyWords(5);
    vector<string> mediumWords(5);
    vector <string> expertWords(5);
    vector <char> guess;

    string difficulty;
    int random = 0;
    srand((unsigned)time(0));

    seedVectors(easyWords, mediumWords, expertWords);

    while (1){
    int guesses = 5;
    cout << "Enter difficulty:" << endl << "easy, medium, expert" << endl;
    cin >> difficulty;
 
    random = (rand()%5);

    if (difficulty == "easy"){
        Word = easyWords.at(random);
        resetGuess(guess, Word);
        missingLetters = guess.size();
    }

    else if (difficulty == "medium"){
        Word = mediumWords.at(random);
        resetGuess(guess, Word);
        missingLetters = guess.size();
    }

    else if (difficulty == "expert"){
        Word = expertWords.at(random);
        resetGuess(guess, Word);
        missingLetters = guess.size();
    }
    
    else{
        cout << "Invalid Input" << endl;
        continue;
    }

    cout << endl;

    while(1){
        printGuess(guess);
        cout << "Guess a Letter:" << endl;
        cin >> input;
        checkWord(guess, input, &guesses, Word, &missingLetters);
        printGuess(guess);
        if (guesses == -1){
            cout << "Better luck next time" << endl;
            cout << "The word was: " << Word << endl;
            break;
        }
        cout << guesses << " lives remaining" << endl;
        cout << endl;

        if (missingLetters == 0){
            cout << "You did it!!!" << endl;
            cout << "The word was: " << Word << endl;
            break;
        }
    }
    cout << endl;

    cout << "Play again?" << endl << "y or n" << endl;
    cin >> input;
    if (input == 'y'){
        continue;
    }
    else if (input == 'n'){
        break;
    }
    else{
        break;
    }
    }
    return 0;

}