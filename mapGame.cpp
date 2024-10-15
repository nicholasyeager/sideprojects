#include <iostream>
#include <vector>
#include <string>

using namespace std;

//Need to find some intuitive way to use the number, in general take the game to another level of playability

enum Status{player, passive, opponent};

class Cell{
public:
    // Constructor to initialize the number and status
    Cell(int num = 0, Status stat = passive) : number(num), status(stat) {}

    // Getter and setter for number
    int getNumber() const { return number; }
    void setNumber(int num) { number = num; }

    // Getter and setter for status
    Status getStatus() const { return status; }
    void setStatus(Status stat) { status = stat; }

private:
    int number;
    Status status;
};

void displayBoard(const vector<vector<Cell>> &a){
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[i].size(); j++){
            if (a[i][j].getStatus() == passive){
                cout << "X "; 
            }
            else if (a[i][j].getStatus() == player){
                cout << "P ";
            }
            else{
                cout << "O ";
            }
        }
        cout << endl;
    }
}

void shiftRight(vector<vector<Cell>> &a, bool &opponent1){
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[i].size(); j++){
            if (a[i][j].getStatus() == player){
                a[i][j].setStatus(passive);
                if (a[i][j+1].getStatus() == opponent){
                    opponent1 = false;
                }
                a[i][j+1].setStatus(player);
                return;
            }
        }
    }
}

void shiftLeft(vector<vector<Cell>> &a, bool &opponent1){
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[i].size(); j++){
            if (a[i][j].getStatus() == player){
                a[i][j].setStatus(passive);
                if (a[i][j-1].getStatus() == opponent){
                    opponent1 = false;
                }
                a[i][j-1].setStatus(player);
                return;
            }
        }
    }
}

void shiftUp(vector<vector<Cell>> &a, bool &opponent1){
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[i].size(); j++){
            if (a[i][j].getStatus() == player){
                a[i][j].setStatus(passive);
                if (a[i-1][j].getStatus() == opponent){
                    opponent1 = false;
                }
                a[i-1][j].setStatus(player);
                return;
            }
        }
    }
}

void shiftDown(vector<vector<Cell>> &a, bool &opponent1){
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[i].size(); j++){
            if (a[i][j].getStatus() == player){
                a[i][j].setStatus(passive);
                if (a[i+1][j].getStatus() == opponent){
                    opponent1 = false;
                }
                a[i+1][j].setStatus(player);
                return;
            }
        }
    }
}

void moveOpponent(vector<vector<Cell>> &a, int rows, int cols){
    int playerRow = -1, playerCol = -1;
    int opponentRow = -1, opponentCol = -1;
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (a[i][j].getStatus() == player) {
                playerRow = i;
                playerCol = j;
            }
            if (a[i][j].getStatus() == opponent) {
                opponentRow = i;
                opponentCol = j;
            }
        }
    }

    a[opponentRow][opponentCol].setStatus(passive);

    if (playerRow < opponentRow && opponentRow < rows - 1) {
        opponentRow++;
    } else if (playerRow >= opponentRow && opponentRow > 0) {
        opponentRow--;
    }

    if (playerCol < opponentCol && opponentCol < cols - 1) {
        opponentCol++;
    } else if (playerCol >= opponentCol && opponentCol > 0) {
        opponentCol--;
    }

    a[opponentRow][opponentCol].setStatus(opponent);
}

void instructions(){
    cout << "The objective of this game is to reach the square of your opponent. You both are allowed to move once per turn." << endl;
    cout << "You will earn points based on grid size and how many moves you used" << endl;
    cout << "To quit enter 'q', else enter either 'up', 'down', 'left', or 'right'" << endl;
}

int main(){

    int cols, rows;
    string input;

    cout << "Generate grid size" << endl;
    cout << "Enter rows:" << endl;
    cin >> rows;
    cout << "Enter cols:" << endl;
    cin >> cols;

    vector<vector<Cell>> board(rows, vector<Cell>(cols));
    board[0][0].setStatus(player);
    board[rows-2][cols-2].setStatus(opponent);
    for (int i = 0; i < board.size(); i++){
        for (int j = 0; j < board[i].size(); j++){
            board[i][j].setNumber((i+j));
        }
    }
    displayBoard(board);
    instructions();
    bool opponent = true;
    int moveCount = 0;

    while(1){
        moveCount++;
        cout << "Enter command:" << endl;
        cin >> input;
        if (input == "q"){
            return 0;
        }
        else if (input == "right"){
            shiftRight(board, opponent);
        }
        else if (input == "left"){
            shiftLeft(board, opponent);
        }
        else if (input == "up"){
            shiftUp(board, opponent);
        }
        else if (input == "down"){
            shiftDown(board, opponent);
        }
        
        if (opponent == false){
            break;
        }
        moveOpponent(board, rows, cols);

        displayBoard(board);
        cout << endl;
    }
    cout << "You did it!" << endl;
    int score = (cols * rows * 10) - (moveCount*10);
    cout << "It took you: " << moveCount << " moves to win" << endl;
    cout << "Final Score: " << score << endl;

    return 0;
}