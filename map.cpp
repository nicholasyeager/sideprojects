#include <iostream>
#include <vector>
#include <string>

using namespace std;

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

void instructions(){
    cout << "The objective of this game is to reach the square of your opponent. You both are allowed to move once per turn." << endl;
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
    board[rows-1][cols-1].setStatus(opponent);
    for (int i = 0; i < board.size(); i++){
        for (int j = 0; j < board[i].size(); j++){
            board[i][j].setNumber((i+j));
        }
    }
    displayBoard(board);
    instructions();


    while(1){
        cout << "Enter command:" << endl;
        if (input == "q"){
            break;
        }
        else if (input == "right"){
            
        }
    }

    return 0;
}