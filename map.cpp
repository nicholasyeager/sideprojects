#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Cell{
public:
    // Constructor to initialize the number and status
    Cell(int num = 0, bool stat = false) : number(num), status(stat) {}

    // Getter and setter for number
    int getNumber() const { return number; }
    void setNumber(int num) { number = num; }

    // Getter and setter for status
    bool getStatus() const { return status; }
    void setStatus(bool stat) { status = stat; }

private:
    int number;
    bool status;
};

void displayBoard(const vector<vector<Cell>> &a){
    
}

int main(){

    vector<vector<Cell>> board(4, vector<Cell>(4));
    board[0][0].setStatus(true);
    for (int i = 0; i < board.size(); i++){
        for (int j = 0; j <board[i].size(); j++){
            board[i][j].setNumber((i+j));
        }
    }
    
}