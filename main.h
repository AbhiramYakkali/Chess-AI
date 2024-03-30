//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_MAIN_H
#define CHESS_AI_MAIN_H

#include <vector>

//Assign pieces to squares using bitwise or (ex: WHITE | QUEEN to represent a white queen) or addition (WHITE + QUEEN)
//Check values of squares using bitwise and
//To get color: value & 24
//To get piece type: value & 7
const int WHITE = 8, BLACK = 16;
const int NONE = 0, KING = 1, PAWN = 2, KNIGHT = 3, BISHOP = 4, ROOK = 5, QUEEN = 6;
const int PIECE_COLOR = 24, PIECE_TYPE = 7;
//Special moves (moves that require more than moving a single piece)
const int NORMAL_MOVE = 0, KINGS_CASTLE = 1, QUEENS_CASTLE = 2, EN_PASSANT = 3;

struct Move {
    int startRow, startCol, endRow, endCol;
    int specialMove;

    Move(int sR, int sC, int eR, int eC) {
        startRow = sR;
        startCol = sC;
        endRow = eR;
        endCol = eC;
        specialMove = NORMAL_MOVE;
    }
    Move(int sR, int sC, int eR, int eC, int special) {
        startRow = sR;
        startCol = sC;
        endRow = eR;
        endCol = eC;
        specialMove = special;
    }
};

class main {
public:
    static void mouseClick(int row, int col);
    static std::vector<Move> getMoves();

private:
    static void endTurn();
};

#endif //CHESS_AI_MAIN_H
