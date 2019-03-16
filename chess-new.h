#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <utility>
using namespace std;

enum Color{BLACK,WHITE,NOCOLOR};

bool MOVESWITCH = true;  // turn off to only play white moves (for debugging)
Color STARTCOLOR = WHITE;  // switch to BLACK for black to start game (for debugging)

class Board {
public:
    Board() {
        board = vector<string>(8);
        board.at(0) = "RNBQKBNR";
        board.at(1) = "PPPPPPPP";
        
        for (int i=2; i<=5; i++)
            board.at(i) = "        ";
        
        board.at(6) = "pppppppp";
        board.at(7) = "rnbqkbnr";
        
        mv = STARTCOLOR;
    }
    Board(vector<string> v,Color c) : board(v),mv(c) {}
    Board(Color c) : board(vector<string>()),mv(c) {}
    Board(Board& b) : board(b.board),mv(b.mv)/*,ss(b.ss)*/ {}
    char pieceAt(int y,int x) { return board.at(y).at(x); }
    Color move() { return mv; }
    void movePiece(int yi,int xi,int yf,int xf) {
        // do actual piece movement
        board.at(yf).at(xf) = board.at(yi).at(xi);
        board.at(yi).at(xi) = ' ';
    }
    void recordMove(string s) {
        // add move to output stringstream
        while (s.size() < 5)
            s += " ";
        if (mv == BLACK)  // move var is switched before recordMove is called, so check for black when you would expect white
            ss << endl << s;
        else
            ss << " " << s;
    }
    string movestream() { return ss.str(); }
    bool moveInput(string);
    bool makeMove(char,int,int,int,int,char);
    Color spaceOccupied(int,int);
    bool canMoveTo(int,int,int,int);
    bool kingMoveTo(int,int,int,int);
    bool queenMoveTo(int,int,int,int);
    bool rookMoveTo(int,int,int,int);
    bool bishopMoveTo(int,int,int,int);
    bool knightMoveTo(int,int,int,int);
    bool pawnWhiteMoveTo(int,int,int,int);
    bool pawnBlackMoveTo(int,int,int,int);
    vector<pair<int,int>> possibleMovesFrom(int,int);
    vector<pair<pair<int,int>,pair<int,int>>> possibleMoves(Color c);
    Color getColorByPiece(char p) {
        // return color of input piece (based on capitalization)
        // space 32, A-Z 65-90, a-z 97-122
        if (p == ' ')
            return NOCOLOR;
        else if (p <= 'Z')
            return WHITE;
        else if (p >= 'a')
            return BLACK;
        return NOCOLOR;
    }
    
    void draw() {
        // print board state to terminal
        cout << endl;
        cout << "   abcdefgh" << endl;
        cout << "   --------" << endl;
        for (int i=7; i>=0; i--) {
            cout << i+1 << " |";
            for (int j=0; j<8; j++)
                if (board.at(i).at(j) == ' ') {
                    if ((i+j) % 2)
                        cout << ' ';
                    else
                        cout << '+';
                }
                else
                    cout << board.at(i).at(j);
            cout << "| " << i+1;
            cout << endl;
        }
        cout << "   --------" << endl;
        cout << "   abcdefgh" << endl;
        cout << endl;
    }
    int score();
    string aiMove();
private:
    vector<string> board;
    Color mv;
    stringstream ss;  // to output move series
};

struct Node {
    Board* board;
    vector<Node*> children;
    Node(Board* b,pair<pair<int,int>,pair<int,int>> mv,vector<Node*> cs) : board(b),move(mv),children(cs) {}
    Node(Board* b,pair<pair<int,int>,pair<int,int>> mv) : board(b),move(mv),children(vector<Node*>()) {}
    Node(Board* b) : board(b) {}
    Color col;
    pair<pair<int,int>,pair<int,int>> move;
    pair<int,Node*> maxChild(Color,int,int alpha=-20000,int beta=20000);
    pair<int,Node*> minChild(Color,int,int alpha=-20000,int beta=20000);
};
