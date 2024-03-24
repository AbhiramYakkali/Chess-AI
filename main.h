//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_MAIN_H
#define CHESS_AI_MAIN_H

//Assign pieces to squares using bitwise or (ex: WHITE | QUEEN to represent a white queen) or addition (WHITE + QUEEN)
//Check values of squares using bitwise and
//To get color: value & 24
//To get piece type: value & 7
const int WHITE = 8, BLACK = 16;
const int NONE = 0, KING = 1, PAWN = 2, KNIGHT = 3, BISHOP = 4, ROOK = 5, QUEEN = 6;

class main {
public:
    static void mouseClick(int row, int col);
};

#endif //CHESS_AI_MAIN_H
