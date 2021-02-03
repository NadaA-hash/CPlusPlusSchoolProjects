/*===================================================================
Name        : Programming Assignment 2
Description : A lexical analyzer test main program
===================================================================*/
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "lex.h"
#include "parse.h"
#include <cxxabi.h>

string demangle(const char* name) {
    int status;
    char* demangledName = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    string retVal = string(demangledName);
    free(demangledName);
    return retVal;
}

// Prints out the parse tree if debug is enabled.
void debugPrint(Pt* tree, int level = 0) {
    if (!tree) {
        cout << "tree = null" << endl;
        return;
    }
    const string className = demangle(typeid(*tree).name());
    Pt* currTree = tree->left;
    cout << string(level, '\t') << className << '\n';
    if (currTree)
        debugPrint(currTree, level + 1);
    currTree = tree->right;
    if (currTree)
        // Fragile code, but it's for debugging anyway.
        debugPrint(currTree, level + (1));
}

using namespace std;

bool compareAlpha(string a, string b){ return a<b; }//1 if a is less than b in the context of alphabetic order

vector<string> alphabaticallySort(vector<string> a){
        sort(a.begin(),a.end(),compareAlpha);//recursively calls compareAlpha until array is sorted
        return a;
}

int main(int argc, char *argv[ ])
{
    istream*in = &cin;// default: read stdin
    ifstream file;
    string filename = "";

    vector <string> identsInAlpha;

    //bool printedWhenSeen = false;// for -v
    //bool intConstants = false;// for -iconsts
    //bool stringConstants = false;// for -sconts
    //bool ids = false;//-ids
    Tok t;

    int lineCount = 0;
    int tokenCount = 0;

    for( int i= 1; i< argc; i++ ) {
            if( argv[i][0] != '-' ){//&& filename != "") {
                if( filename != "" ) {//if the previous file name is stored in here, it will return error
                    cerr<< "ONLY ONE FILE NAME ALLOWED" << endl;
                    return 1;
                }
                filename = argv[i];
                continue; //go to the next item in this list
            }

            string flag(argv[i]);

            if( flag == "-v") {//if present, every token is printed when it is seen
                    //printedWhenSeen = true;
            }
            else if (flag == "-iconsts"){
                    //intConstants = true;
            }
            else if (flag == "-sconsts"){
                    //stringConstants = true;
            }
            else if (flag == "-ids"){
                    //ids = true;
            }
            else{
                cerr<< "UNRECOGNIZED FLAG " << flag << endl;
                return 1;
            }
    }
    if( filename != "" ) {
        file.open(filename);
        if( !file.is_open() ) {
            cerr<< "CANNOT OPEN " << filename << endl;
            return 1;
        }
        in = &file;
    }
/*
    while( (t = getNextToken(*in, lineCount)) != DONE && t != ERR ) {
            tokenCount++;
            lineCount = t.GetLinenum();
            if (printedWhenSeen == true){
                cout << t << endl;
            }
            if (ids && t == IDENT){
                identsInAlpha.push_back(t.GetLexeme());
            }
            if (intConstants && stringConstants)
            {
                return 0;
            }
            if (file.eof()){
               break;
            }
    }*/
//============================================================ASSIGNMENT 3
    Pt *tree = Prog(*in, lineCount);
	debugPrint(tree);
    if(tree!=0){
        cout<<"PLUS COUNT: "<<tree->plusCount()<<endl;
        cout<<"EQ COUNT: "<<tree->eqCount()<<endl;
        cout<<"MAX DEPTH: "<<tree->MaxDepth()<<endl;
    }
    file.close();

    /*if (t == ERR){
        cout << t << endl;
        return 0;
    }*/
    //cout << "Lines: " << lineCount << endl;
    if (tokenCount != 0){
        cout << "Tokens: " << tokenCount << endl;
    }

    /*if (ids && identsInAlpha.size() != 0){
        identsInAlpha = alphabaticallySort(identsInAlpha);

        cout <<"IDENTIFIERS: " << identsInAlpha[0];
        for (int i = 1; (unsigned)i < identsInAlpha.size(); i++) {
            cout << ", " << identsInAlpha[i];
        }
        cout << endl;
    }*/

    return 0;
}
