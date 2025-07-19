#include <iostream>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
    if (argc != 3){
        cerr << "Usage: ./a.exe followingFile followerFile" << endl;
        return 1;
    }

    unordered_set<string> followers;
    vector<string> notFollowingBack;
    ifstream followingFile(argv[1]);
    ifstream followerFile(argv[2]);
    string line;

    while (getline(followerFile, line)){
        followers.insert(line);
    }

    while (getline(followingFile, line)){
        if (followers.find(line) == followers.end()){
            notFollowingBack.push_back(line);
        }
    }

    for (const string& person : notFollowingBack){
        cout << person << endl;
    }

    followingFile.close();
    followerFile.close();

    return 0;
}
