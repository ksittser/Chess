/* FIX / TO DO:
 * handle two of same piece able to move to same square
 * "Nb6" for knight on a8 not accepted
 *   ^ apparently, but can't reproduce
 * implement en passant
 * add castling to possibleMoves possibilities
 * inputs like "Nae5" aren't processed properly
 * check/checkmate (moving into check??)
 * draws??
 */


#include "chess-new.h"

bool Board::moveInput(string s) {
    // check if input string is valid notation; call makeMove to check/make the move if so
    char pieceChar = 'X';
    int xf = -1, yf = -1;
    int xi = -1, yi = -1;
    
    bool isPawn = false;
    if (s.size() <= 1) {
        cout << "  BAD FORMAT" << endl;
        return false;
    }
    // find what kind of command, and process initial "K/Q/etc."
    if (s.at(0)=='K' || s.at(0)=='Q' || s.at(0)=='R' || s.at(0)=='B' || s.at(0)=='N') {
        pieceChar = s.at(0);
        s.erase(0,1);
    }
    else if (s=="0-0" || s=="O-O")  // kingside castling
        return makeMove('K',-1,-1,-1,-1,'C');
    else if (s=="0-0-0" || s=="O-O-O")  // queenside castling
        return makeMove('K',-1,-1,-1,-1,'c');
    else if (s.at(0)=='x') {  // first character is x
        cout << "  BAD FORMAT" << endl;
        return false;
    }
    else {
        isPawn = true;
        pieceChar = 'P';
    }
    // for pawn captures, process initial "a/b/etc."
    if (isPawn && s.at(1)=='x' && (s.at(0)=='a' || s.at(0)=='b' || s.at(0)=='c' || s.at(0)=='d' || s.at(0)=='e' || s.at(0)=='f' || s.at(0)=='g' || s.at(0)=='h')) {  // e.g., axb3
        xi = s.at(0)-'a'+1;
        s.erase(0,1);
    }
    // ignore "x" for captures
    if (s.at(0) == 'x')
        s.erase(0,1);
    // find file
    if (!s.size()) {
        cout << "  BAD FORMAT" << endl;
        return false;
    }
    switch (s.at(0)) {
        case 'a':
            xf = 1;
            break;
        case 'b':
            xf = 2;
            break;
        case 'c':
            xf = 3;
            break;
        case 'd':
            xf = 4;
            break;
        case 'e':
            xf = 5;
            break;
        case 'f':
            xf = 6;
            break;
        case 'g':
            xf = 7;
            break;
        case 'h':
            xf = 8;
            break;
        default:
            cout << "  BAD FORMAT" << endl;
            return false;
    }
    s.erase(0,1);
    // find rank
    if (!s.size()) {
        cout << "  BAD FORMAT" << endl;
        return false;
    }
    stringstream ss;
    ss << s.at(0);
    ss >> yf;
    if (yf<1 || yf>8) {
        cout << "  BAD FORMAT" << endl;;
        return false;
    }
    s.erase(0,1);
    // pawn promotion notation
    char specialChar = '*';  // this will be passed to makeMove to indicate special move like castling or promotion
    if (s.size() > 0) {
        // extraneous characters
        if (s.at(0)=='K' || s.at(0)=='Q' || s.at(0)=='R' || s.at(0)=='B' || s.at(0)=='N')
            specialChar = s.at(0);
        else {
            cout << "  BAD FORMAT" << endl;
            return false;
        }
        s.erase(0,1);
        if (s.size() > 0) {
            cout << "  BAD FORMAT" << endl;
            return false;
        }
    }
    return makeMove(pieceChar,yi-1,xi-1,yf-1,xf-1,specialChar);
}

bool Board::makeMove(char pieceChar,int yi,int xi,int yf,int xf,char specialChar) {
    // check if move is legal, then move it if so
    // input: CAPITAL letter pieceChar, coords of destination
    // any origin coords that can be found from command notation are input as yi/xi (e.g., "exd5," "Na3b5"); otherwise yi/xi are -1
    bool castling = tolower(specialChar) == 'c';
    bool validSpace = false;
    if (mv == BLACK)
        pieceChar = tolower(pieceChar);
    if (castling) {
        // handle special case of castling
        xi = 4;
        if (mv == WHITE) {
            yi = 0;
            yf = 0;
        }
        else {
            yi = 7;
            yf = 7;
        }
        if (specialChar == 'C')  // kingside
            xf = 6;
        else
            xf = 1;
        if (mv == WHITE) {
            if (board.at(yi).at(xi) != 'K') {
                cout << "  KING UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
            if (specialChar == 'C' && board.at(0).at(7) != 'R') {
                cout << "  ROOK UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
            if (specialChar == 'c' && board.at(0).at(0) != 'R') {
                cout << "  ROOK UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
        }
        else {
            if (board.at(yi).at(xi) != 'k') {
                cout << "  KING UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
            if (specialChar == 'C' && board.at(7).at(7) != 'r') {
                cout << "  ROOK UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
            if (specialChar == 'c' && board.at(7).at(0) != 'r') {
                cout << "  ROOK UNAVAILABLE FOR CASTLING" << endl;
                return false;
            }
        }
    }
    else {
        // everything besides castling
        // find origin space
        for (int y=0; y<8; y++) {
            if (yi > -1 && y != yi)
                continue;
            for (int x=0; x<8; x++) {
                if (xi > -1 && x != xi)
                    continue;
                char piece = board.at(y).at(x);
                if (getColorByPiece(piece) == mv && piece == pieceChar && canMoveTo(y,x,yf,xf)) {
                    validSpace = true;
                    yi = y;
                    xi = x;
                    break;
                }
            }
            if (validSpace)
                break;
        }
        
        if (!validSpace) {
            cout << "  ILLEGAL DESTINATION" << endl;
            return false;
        }
    }
    // check if space has own color piece on it
    if (spaceOccupied(yf,xf) == mv) {
        cout << "  SPACE IS OCCUPIED" << endl;
        return false;
    }
    
    // check if any pieces in between origin and destination
    int yCtr = yi, xCtr = xi;
    if (toupper(pieceChar) != 'N')
        while (yCtr != yf || xCtr != xf) {
            if (!(yCtr==yi && xCtr==xi)) {
                if (board.at(yCtr).at(xCtr)!=' ') {
                    cout << "PIECE IN THE WAY" << endl;
                    return false;
                }
            }
            yCtr += (yf-yi > 0) - (yf-yi < 0);  // i.e., add sign of yf-yi
            xCtr += (xf-xi > 0) - (xf-xi < 0);  // i.e., add sign of xf-xi
        }
    if (specialChar != '*')
        if (!(pieceChar=='P' && yf==0) && !(pieceChar=='p' && yf==7)) {
            cout << "  ILLEGAL PROMOTION" << endl;
            return false;
        }
    if (toupper(board.at(yf).at(xf)) == 'K') {
        cout << "  ILLEGAL DESTINATION" << endl;
        return false;
    }
    // move
    movePiece(yi,xi,yf,xf);
    if (castling) {
        if (mv == WHITE) {
            if (specialChar == 'C')
                movePiece(0,7,0,5);
            else if (specialChar == 'c')
                movePiece(0,0,0,2);
        }
        else {
            if (specialChar == 'C')
                movePiece(7,7,7,5);
            else if (specialChar == 'c')
                movePiece(7,0,7,2);
        }
    }
    // pawn promotion
    if (pieceChar == 'P' && yf == 7) {
        if (specialChar == '*')
            board.at(yf).at(xf) = 'Q';
        else
            board.at(yf).at(xf) = specialChar;
    }
    if (pieceChar == 'p' && yf == 0) {
        if (specialChar == '*')
            board.at(yf).at(xf) = 'q';
        else
            board.at(yf).at(xf) = tolower(specialChar);
    }
    // switch sides
    if (MOVESWITCH) {
        if (mv == WHITE)
            mv = BLACK;
        else
            mv = WHITE;
    }
    return true;
}

bool Board::canMoveTo(int yi,int xi,int yf,int xf) {
    // return whether piece at input origin can, according to only its movement rules, move to input destination
    if (yf < 0 || yf >= 8 || xf < 0 || xf >= 8)
        return false;
    if (!abs(yf-yi) && !abs(xf-xi))
        return false;
    if (toupper(board.at(yi).at(xi)) == 'K')
        return kingMoveTo(yi,xi,yf,xf);
    else if (toupper(board.at(yi).at(xi)) == 'Q')
        return queenMoveTo(yi,xi,yf,xf);
    else if (toupper(board.at(yi).at(xi)) == 'R')
        return rookMoveTo(yi,xi,yf,xf);
    else if (toupper(board.at(yi).at(xi)) == 'B')
        return bishopMoveTo(yi,xi,yf,xf);
    else if (toupper(board.at(yi).at(xi)) == 'N')
        return knightMoveTo(yi,xi,yf,xf);
    else if (board.at(yi).at(xi) == 'P')
        return pawnWhiteMoveTo(yi,xi,yf,xf);
    else if (board.at(yi).at(xi) == 'p')
        return pawnBlackMoveTo(yi,xi,yf,xf);
}

bool Board::kingMoveTo(int yi,int xi,int yf,int xf) {
    // return whether this piece can, according to only its movement rules, move from input origin to input destination
    return abs(yf-yi) <= 1 && abs(xf-xi) <= 1;
}
bool Board::queenMoveTo(int yi,int xi,int yf,int xf) {
    return yf == yi || xf == xi || abs(yf-yi) == abs(xf-xi);
}
bool Board::rookMoveTo(int yi,int xi,int yf,int xf) {
    return yf == yi || xf == xi;
}
bool Board::bishopMoveTo(int yi,int xi,int yf,int xf) {
    return abs(yf-yi) == abs(xf-xi);
}
bool Board::knightMoveTo(int yi,int xi,int yf,int xf) {
    return (abs(yf-yi)==1 && abs(xf-xi)==2) || (abs(yf-yi)==2 && abs(xf-xi)==1);
}
bool Board::pawnWhiteMoveTo(int yi,int xi,int yf,int xf) {
    // space 32, A-Z 65-90, a-z 97-122
    if (board.at(yf).at(xf) == ' ')
        // non-capture
        return xf == xi && (yf == yi+1 || (yf == yi+2 && yi == 1));
    else if (board.at(yf).at(xf) >= 'a')
        // capture
        return yf == yi+1 && abs(xf-xi) == 1;
    return false;
}
bool Board::pawnBlackMoveTo(int yi,int xi,int yf,int xf) {
    if (yi == 5 && xi == 5)
        cout << "f6: " << board.at(yi).at(xi) << endl;
    // space 32, A-Z 65-90, a-z 97-122
    if (board.at(yf).at(xf) == ' ')
        // non-capture
        return xf == xi && (yf == yi-1 || (yf == yi-2 && yi == 6));
    else if (board.at(yf).at(xf) <= 'Z')
        // capture
        return yf == yi-1 && abs(xf-xi) == 1;
    return false;
}

Color Board::spaceOccupied(int y,int x) {
    // return color of piece occupying space (NOCOLOR if empty)
    return getColorByPiece(board.at(y).at(x));
}

int Board::score() {
    // calculate score (based solely on what pieces remain on board, regardless of position)
    int sc = 0;
    bool wKing = false, bKing = false;
    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++)
            if (board.at(i).at(j) != ' ') {
                bool side = board.at(i).at(j) <= 'Z';
                int add = 0;
                switch (toupper(board.at(i).at(j))) {
                    case 'Q':
                        add = 9;
                        break;
                    case 'R':
                        add = 5;
                        break;
                    case 'B':
                        add = 3;
                        break;
                    case 'N':
                        add = 3;
                        break;
                    case 'P':
                        add = 1;
                        break;
                    case 'K':
                        if (side)
                            wKing = false;
                        else
                            bKing = false;
                }
                if (!side)
                    add = -add;
                sc += add;
            }
    if (!wKing)
        sc -= 10000;
    if (!bKing)
        sc += 10000;
    return sc;
}

vector<pair<int,int>> Board::possibleMovesFrom(int y,int x) {
    // return vector of all possible spaces that the piece at input location can move to
    Color pieceColor = getColorByPiece(board.at(y).at(x));
    vector<pair<int,int>> moves;
    switch (toupper(board.at(y).at(x))) {
        case 'K':
            for (int i=y-1; i<=y+1; i++) {
                if (i<0 || i>=8)
                    continue;
                for (int j=x-1; j<=x+1; j++) {
                    if (i==y && j==x)
                        continue;
                    if (j<0 || j>=8)
                        continue;
                    if (getColorByPiece(board.at(i).at(j)) != pieceColor)
                        moves.push_back(make_pair(i,j));
                }
            }
            break;
        case 'Q':
            for (int i=-1; i<=1; i++)
                for (int j=-1; j<=1; j++) {
                    if (i==0 && j==0)
                        continue;
                    pair<int,int> p = make_pair(y,x);
                    while (p.first>=0 && p.first<8 && p.second>=0 && p.second<8) {
                        if (p.first==y && p.second==x) {
                            p.first += i;
                            p.second += j;
                            continue;
                        }
                        if (getColorByPiece(board.at(p.first).at(p.second)) == pieceColor)
                            break;
                        moves.push_back(p);
                        if (getColorByPiece(board.at(p.first).at(p.second)) != pieceColor && getColorByPiece(board.at(p.first).at(p.second)) != NOCOLOR)
                            break;
                        p.first += i;
                        p.second += j;
                    }
                }
            break;
        case 'R':
            for (int i=-1; i<=1; i++)
                for (int j=-1; j<=1; j++) {
                    if (i==0 && j==0)
                        continue;
                    if (i!=0 && j!=0)
                        continue;
                    pair<int,int> p = make_pair(y,x);
                    while (p.first>=0 && p.first<8 && p.second>=0 && p.second<8) {
                        if (p.first==y && p.second==x) {
                            p.first += i;
                            p.second += j;
                            continue;
                        }
                        if (getColorByPiece(board.at(p.first).at(p.second)) == pieceColor)
                            break;
                        moves.push_back(p);
                        if (getColorByPiece(board.at(p.first).at(p.second)) != pieceColor && getColorByPiece(board.at(p.first).at(p.second)) != NOCOLOR)
                            break;
                        p.first += i;
                        p.second += j;
                    }
                }
            break;
        case 'B':
            for (int i=-1; i<=1; i++)
                for (int j=-1; j<=1; j++) {
                    if (i==0 || j==0)
                        continue;
                    pair<int,int> p = make_pair(y,x);
                    while (p.first>=0 && p.first<8 && p.second>=0 && p.second<8) {
                        if (p.first==y && p.second==x) {
                            p.first += i;
                            p.second += j;
                            continue;
                        }
                        if (getColorByPiece(board.at(p.first).at(p.second)) == pieceColor)
                            break;
                        moves.push_back(p);
                        if (getColorByPiece(board.at(p.first).at(p.second)) != pieceColor && getColorByPiece(board.at(p.first).at(p.second)) != NOCOLOR)
                            break;
                        p.first += i;
                        p.second += j;
                    }
                }
            break;
        case 'N':
            for (int i=y-2; i<=y+2; i++) {
                if (i<0 || i>=8)
                    continue;
                for (int j=x-2; j<=x+2; j++) {
                    if (j<0 || j>=8)
                        continue;
                    if (abs(abs(i-y)-abs(j-x)) != 1 || i==y || j==x)
                        continue;
                    if (getColorByPiece(board.at(i).at(j)) != pieceColor)
                        moves.push_back(make_pair(i,j));
                }
            }
            break;
        case 'P':
            if (board.at(y).at(x) == 'P') {
                if (getColorByPiece(board.at(y+1).at(x)) == NOCOLOR)
                    moves.push_back(make_pair(y+1,x));
                if (y==1 && getColorByPiece(board.at(y+2).at(x)) == NOCOLOR)
                    moves.push_back(make_pair(y+2,x));
            }
            else {
                if (getColorByPiece(board.at(y-1).at(x)) == NOCOLOR)
                    moves.push_back(make_pair(y-1,x));
                if (y==6 && getColorByPiece(board.at(y-2).at(x)) == NOCOLOR)
                    moves.push_back(make_pair(y-2,x));
            }
    }
    return moves;
}
vector<pair<pair<int,int>,pair<int,int>>> Board::possibleMoves() {
    // return vector of all possible moves that can be made on board (format is origin coords, then destination coords)
    vector<pair<pair<int,int>,pair<int,int>>> moves;
    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++)
            if (board.at(i).at(j) != ' ') {
                vector<pair<int,int>> movesPiece = possibleMovesFrom(i,j);
                for (pair<int,int> p : movesPiece)
                    moves.push_back(make_pair(make_pair(i,j),p));
            }
    return moves;
}

int main() {
    string input;
    Board b;
    while (true) {
        cout << "SCORE: " << b.score() << endl;
        b.draw();
        if (b.move() == WHITE)
            cout << "White's move: ";
        else
            cout << "Black's move: ";
        cin >> input;
        while (!b.moveInput(input)) {
            if (b.move() == WHITE)
                cout << "  White's move: ";
            else
                cout << "  Black's move: ";
            cin >> input;
        }
        b.recordMove(input);
        cout << b.movestream() << endl;
    }
    return 0;
}