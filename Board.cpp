//
// Created by Abhiram on 3/23/2024.
//

#include <iostream>
#include "Board.h"

using namespace std;

Board::Board(const int b[8][8]) {
    //1: white, 2: black
    turn = 1;

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            board[i][j] = b[i][j];
        }
    }

    findAllMoves();
}

void Board::findAllMoves() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            int piece = board[i][j];

            if((piece & PIECE_TYPE) != NONE && (piece & PIECE_COLOR) / 8 == turn) {
                findMovesForSquare(i, j);
            }
        }
    }
}

void Board::findMovesForSquare(int row, int col) {
    switch(board[row][col] & PIECE_TYPE) {
        case PAWN:
            calculatePawnMoves(row, col);
            break;
        case KNIGHT:
            calculateKnightMoves(row, col);
            break;
        case BISHOP:
        case ROOK:
        case QUEEN:
            calculateSlidingMoves(row, col);
            break;
        case KING:
            calculateKingMoves(row, col);
    }
}

void Board::calculatePawnMoves(int row, int col) {
    //Determine direction of travel based on color; black: positive (downwards), white: negative (upwards)
    int direction = (turn == 1) ? -1 : 1;

    //Check forward moves
    int nextRow = row + direction;
    if(board[nextRow][col] == NONE) {
        allMoves.emplace_back(row, col, nextRow, col);

        //Check for double Move
        nextRow += direction;
        if((row == 1 || row == 6) && board[nextRow][col] == NONE) {
            allMoves.emplace_back(row, col, nextRow, col);
        }
    }

    //Check captures
    nextRow = row + direction;
    int nextCol = col - 1;
    if(nextCol >= 0 && board[nextRow][nextCol] != NONE && (board[nextRow][nextCol] & PIECE_COLOR) != (board[row][col] & PIECE_COLOR)) {
        allMoves.emplace_back(row, col, nextRow, nextCol);
    }

    nextCol += 2;
    if(nextCol < 8 && board[nextRow][nextCol] != NONE && (board[nextRow][nextCol] & PIECE_COLOR) != (board[row][col] & PIECE_COLOR)) {
        allMoves.emplace_back(row, col, nextRow, nextCol);
    }

    //TODO: Check en passant
}
void Board::calculateKnightMoves(int row, int col) {
    pair<int, int> directions[8];
    directions[0] = make_pair(-2, -1);
    directions[1] = make_pair(-2, 1);
    directions[2] = make_pair(-1, -2);
    directions[3] = make_pair(-1, 2);
    directions[4] = make_pair(2, -1);
    directions[5] = make_pair(2, 1);
    directions[6] = make_pair(1, -2);
    directions[7] = make_pair(1, 2);

    for(pair<int, int> direction : directions) {
        int newRow = row + direction.first;
        int newCol = col + direction.second;

        if(newRow < 0 || newRow > 7 || newCol < 0 || newCol > 7) continue;

        if(board[newRow][newCol] == NONE || (board[newRow][newCol] & PIECE_COLOR) / 8 != turn) {
            allMoves.emplace_back(row, col, newRow, newCol);
        }
    }
}
void Board::calculateKingMoves(int row, int col) {
    pair<int, int> directions[8];
    directions[0] = make_pair(1, 0);
    directions[1] = make_pair(-1, 0);
    directions[2] = make_pair(0, 1);
    directions[3] = make_pair(0, -1);
    directions[4] = make_pair(1, 1);
    directions[5] = make_pair(-1, 1);
    directions[6] = make_pair(1, -1);
    directions[7] = make_pair(-1, -1);

    int pieceColor = board[row][col] & PIECE_COLOR;

    for(auto direction : directions) {
        int nextRow = row + direction.first;
        int nextCol = col + direction.second;

        if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) continue;

        if((board[nextRow][nextCol] & PIECE_COLOR) != pieceColor) allMoves.emplace_back(row, col, nextRow, nextCol);
    }
}
void Board::calculateSlidingMoves(int row, int col) {
    pair<int, int> directions[8];
    directions[0] = make_pair(1, 0);
    directions[1] = make_pair(-1, 0);
    directions[2] = make_pair(0, 1);
    directions[3] = make_pair(0, -1);
    directions[4] = make_pair(1, 1);
    directions[5] = make_pair(-1, 1);
    directions[6] = make_pair(1, -1);
    directions[7] = make_pair(-1, -1);

    int piece = board[row][col];
    int pieceColor = piece & PIECE_COLOR;
    int startIndex = ((piece & PIECE_TYPE) == BISHOP) ? 4 : 0;
    int endIndex = ((piece & PIECE_TYPE) == ROOK) ? 4 : 8;

    for(int i = startIndex; i < endIndex; i++) {
        pair<int, int> direction = directions[i];
        int nextRow = row;
        int nextCol = col;

        while(true) {
            nextRow += direction.first;
            nextCol += direction.second;

            if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) break;

            int nextSquareColor = board[nextRow][nextCol] & PIECE_COLOR;

            //Square being checked is occupied by a friendly piece
            if(nextSquareColor == pieceColor) break;

            allMoves.emplace_back(row, col, nextRow, nextCol);

            //Square being checked is occupied by an enemy piece, current piece can go no farther
            if(nextSquareColor != NONE) break;
        }
    }
}

bool Board::click(int row, int col) {
    for(auto & move : moves) {
        if(move.endRow == row && move.endCol == col) {
            board[move.endRow][move.endCol] = board[move.startRow][move.startCol];
            board[move.startRow][move.startCol] = 0;
            return true;
        }
    }

    isolateMovesForSquare(row, col);
    return false;
}

void Board::endTurn() {
    moves.clear();
    allMoves.clear();
    turn = (turn == 1) ? 2 : 1;

    findAllMoves();
}

void Board::isolateMovesForSquare(int row, int col) {
    moves.clear();

    int piece = board[row][col];
    if(piece != NONE && (piece & PIECE_COLOR) / 8 == turn) {
        for(auto & move : allMoves) {
            if(move.startRow == row && move.startCol == col) moves.push_back(move);
        }
    }
}

std::vector<Move> Board::getAllMoves() {
    return allMoves;
}
std::vector<Move> Board::getMoves() {
    return moves;
}

int (*Board::getBoard())[8] {
    return board;
}