//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_BOARD_H
#define CHESS_AI_BOARD_H


#include <vector>
#include <string>
#include "main.h"

constexpr int TURN_WHITE = 1, TURN_BLACK = 2;
constexpr int NORMAL_STATE = 0, WHITE_MATE = 1, BLACK_MATE = 2, STALEMATE = 3, IN_CHECK = 4;

class Board {
public:
    explicit Board(const int b[8][8]);
    Board(const int b[8][8], int turn);

    //Returns possible moves that the currently selected piece can make
    std::vector<Move> getMoves();
    //Returns all possible moves in the position
    std::vector<Move> getAllMoves();

    //Generates a FEN string corresponding to the current board state
    std::string getFENString() const;

    bool click(int row, int col);
    void makeMove(const Move& move);
    Board makeBoardForMove(const Move& move) const;
    int (*getBoard())[8];
    int endTurn();

private:
    int board[8][8]{};
    int turn;
    //0: white king, 1: black king
    //Find row using / 10, column using % 10
    int kingPositions[2]{};
    //Keeps track of the column of a pawn that has moved two squares in the previous move (set to -2 if N/A)
    //Used to check if en passant is possible
    int enPassantCol;
    //Checks if the player whose turn it is in check (used to prevent castling while in check)
    bool inCheck;
    std::pair<int, int> knightDirections[8];
    std::pair<int, int> slidingDirections[8];
    std::vector<Move> allMoves, moves;

    //First: Queen's side, second: king's side
    std::pair<int, int> canCastle[2];

    void findMovesForSquare(int row, int col);
    void findAllMoves();
    void isolateMovesForSquare(int row, int col);
    void checkMove(const Move& move);

    void calculatePawnMoves(int row, int col);
    void calculateKnightMoves(int row, int col);
    void calculateSlidingMoves(int row, int col);
    void calculateKingMoves(int row, int col);

    bool isKingInCheck(int boardToCheck[][8]);
    bool isKingInCheck();

    static bool isEnemyPiece2(int piece, int color);
    bool isEnemyPiece(int row, int col) const;
    bool isEnemyPiece(int piece) const;
};


#endif //CHESS_AI_BOARD_H
