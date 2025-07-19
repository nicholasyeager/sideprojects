#include <iostream>
#include <vector>

using namespace std;

int main(){

    cout << "Hello World" << endl;

    vector<int> A = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 0; i < A.size(); i++){
        cout << A[i] << " ";
    }
    cout << endl;
  
    return 0;
}