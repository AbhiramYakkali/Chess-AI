//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_BOARD_H
#define CHESS_AI_BOARD_H


#include <vector>
#include "main.h"

const int TURN_WHITE = 1, TURN_BLACK = 2;

class Board {
public:
    Board(const int board[8][8]);

    //Returns possible moves that the currently selected piece can make
    std::vector<Move> getMoves();
    //Returns all possible moves in the position
    std::vector<Move> getAllMoves();

    bool click(int row, int col);
    int (*getBoard())[8];
    int endTurn();

private:
    int board[8][8]{};
    int turn;
    //0: white king, 1: black king
    //Find row using / 10, column using % 10
    int kingPositions[2]{};
    std::pair<int, int> knightDirections[8];
    std::pair<int, int> slidingDirections[8];
    std::vector<Move> allMoves, moves;

    //First: Queen's side, second: king's side
    std::pair<int, int> canCastle[2];

    void findMovesForSquare(int row, int col);
    void findAllMoves();
    void isolateMovesForSquare(int row, int col);
    void checkMove(Move move);

    void calculatePawnMoves(int row, int col);
    void calculateKnightMoves(int row, int col);
    void calculateSlidingMoves(int row, int col);
    void calculateKingMoves(int row, int col);

    bool isKingInCheck(int boardToCheck[][8]);
    bool isKingInCheck();

    static bool isEnemyPiece2(int piece, int color);
    bool isEnemyPiece(int row, int col);
    bool isEnemyPiece(int piece);

    void makeMove(Move move);
};


#endif //CHESS_AI_BOARD_H
