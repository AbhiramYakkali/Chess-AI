//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_BOARD_H
#define CHESS_AI_BOARD_H


#include <vector>
#include "main.h"

class Board {
public:
    Board(const int board[64]);

    //Returns possible moves that the currently selected piece can make
    std::vector<Move> getMoves();
    //Returns all possible moves in the position
    std::vector<Move> getAllMoves();

    bool click(int square);
    int* getBoard();
    void endTurn();

private:
    int board[64];
    int turn;
    std::vector<Move> allMoves, moves;

    void findMovesForSquare(int square);
    void findAllMoves();
    void isolateMovesForSquare(int square);

    void calculatePawnMoves(int square);
};


#endif //CHESS_AI_BOARD_H
