//
// Created by Abhiram on 3/23/2024.
//

#include <iostream>
#include "Board.h"

Board::Board(const int b[64]) {
    //1: white, 2: black
    turn = 1;

    for(int i = 0; i < 64; i++) {
        board[i] = b[i];
    }

    findAllMoves();
}

void Board::findAllMoves() {
    for(int i = 0; i < 64; i++) {
        int piece = board[i];

        if((piece & PIECE_TYPE) != NONE && (piece & PIECE_COLOR) / 8 == turn) {
            findMovesForSquare(i);
        }
    }
}

void Board::findMovesForSquare(int square) {
    if((board[square] & PIECE_TYPE) == PAWN) {
        //Calculate pawn moves
        calculatePawnMoves(square);
    }
}

void Board::calculatePawnMoves(int square) {
    //Determine direction of travel based on color; black: positive (downwards), white: negative (upwards)
    int direction = (turn == 1) ? -1 : 1;
    //Multiply direction by 8 since each row is equal to 8 squares
    direction *= 8;

    //Check forward moves
    int nextSquare = square + direction;
    if((board[nextSquare] & PIECE_TYPE) == NONE) {
        allMoves.emplace_back(square, nextSquare);

        //Check for double Move
        nextSquare += direction;
        if(((square / 8 == 1 && turn == 2) || (square / 8 == 6 && turn == 1)) && (board[nextSquare] & PIECE_TYPE) == NONE) {
            allMoves.emplace_back(square, nextSquare);
        }
    }

    //Check captures
    nextSquare = square + direction;
    if(nextSquare % 8 != 0) {
        nextSquare--;
        int color = board[nextSquare] & PIECE_COLOR;
        if(color != NONE && color != (board[square] & PIECE_COLOR)) allMoves.emplace_back(square, nextSquare);
    }

    nextSquare = square + direction;
    if(nextSquare % 8 != 7) {
        nextSquare++;
        int color = board[nextSquare] & PIECE_COLOR;
        if(color != NONE && color != (board[square] & PIECE_COLOR)) allMoves.emplace_back(square, nextSquare);
    }

    //TODO: Check en passant
}

bool Board::click(int square) {
    for(auto & move : moves) {
        if(move.endSquare == square) {
            board[move.endSquare] = board[move.startSquare];
            board[move.startSquare] = 0;
            return true;
        }
    }

    isolateMovesForSquare(square);
    return false;
}

void Board::endTurn() {
    moves.clear();
    allMoves.clear();
    turn = (turn == 1) ? 2 : 1;

    findAllMoves();
}

void Board::isolateMovesForSquare(int square) {
    moves.clear();

    int piece = board[square];
    if((piece & PIECE_TYPE) != NONE && (piece & PIECE_COLOR) / 8 == turn) {
        for(auto & move : allMoves) {
            if(move.startSquare == square) moves.push_back(move);
        }
    }
}

std::vector<Move> Board::getAllMoves() {
    return allMoves;
}
std::vector<Move> Board::getMoves() {
    return moves;
}

int *Board::getBoard() {
    return board;
}