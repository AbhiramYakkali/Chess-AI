//
// Created by Abhiram on 3/23/2024.
//

#include <iostream>
#include "Board.h"

using namespace std;

Board::Board(const int b[8][8]) {
    turn = TURN_WHITE;

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            board[i][j] = b[i][j];

            if((board[i][j] & PIECE_TYPE) == KING) {
                kingPositions[(board[i][j] & PIECE_COLOR) / 8 - 1] = i * 10 + j;
            }
        }
    }

    //Populate possible knight moves
    knightDirections[0] = make_pair(-2, -1);
    knightDirections[1] = make_pair(-2, 1);
    knightDirections[2] = make_pair(-1, -2);
    knightDirections[3] = make_pair(-1, 2);
    knightDirections[4] = make_pair(2, -1);
    knightDirections[5] = make_pair(2, 1);
    knightDirections[6] = make_pair(1, -2);
    knightDirections[7] = make_pair(1, 2);

    //Populate possible sliding moves
    slidingDirections[0] = make_pair(1, 0);
    slidingDirections[1] = make_pair(-1, 0);
    slidingDirections[2] = make_pair(0, 1);
    slidingDirections[3] = make_pair(0, -1);
    slidingDirections[4] = make_pair(1, 1);
    slidingDirections[5] = make_pair(-1, 1);
    slidingDirections[6] = make_pair(1, -1);
    slidingDirections[7] = make_pair(-1, -1);

    canCastle[0] = make_pair(true, true);
    canCastle[1] = make_pair(true, true);

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
        checkMove(Move(row, col, nextRow, col));

        //Check for double Move
        nextRow += direction;
        if((row == 1 || row == 6) && board[nextRow][col] == NONE) {
            checkMove(Move(row, col, nextRow, col));
        }
    }

    //Check captures
    nextRow = row + direction;
    int nextCol = col - 1;
    if(nextCol >= 0 && board[nextRow][nextCol] != NONE && (board[nextRow][nextCol] & PIECE_COLOR) != (board[row][col] & PIECE_COLOR)) {
        checkMove(Move(row, col, nextRow, nextCol));
    }

    nextCol += 2;
    if(nextCol < 8 && board[nextRow][nextCol] != NONE && (board[nextRow][nextCol] & PIECE_COLOR) != (board[row][col] & PIECE_COLOR)) {
        checkMove(Move(row, col, nextRow, nextCol));
    }

    //TODO: Check en passant
}
void Board::calculateKnightMoves(int row, int col) {
    for(pair<int, int> direction : knightDirections) {
        int nextRow = row + direction.first;
        int nextCol = col + direction.second;

        if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) continue;

        if(board[nextRow][nextCol] == NONE || (board[nextRow][nextCol] & PIECE_COLOR) / 8 != turn) {
            checkMove(Move(row, col, nextRow, nextCol));
        }
    }
}
void Board::calculateKingMoves(int row, int col) {
    int pieceColor = board[row][col] & PIECE_COLOR;

    for(auto direction : slidingDirections) {
        int nextRow = row + direction.first;
        int nextCol = col + direction.second;

        if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) continue;

        if((board[nextRow][nextCol] & PIECE_COLOR) != pieceColor) checkMove(Move(row, col, nextRow, nextCol));
    }

    //Check if king can castle
    //Check queen's side castling
    if(canCastle[turn - 1].first && (board[row][0] & PIECE_TYPE) == ROOK && !isEnemyPiece(row, 0)) {
        if(board[row][1] + board[row][2] + board[row][3] == 0) allMoves.emplace_back(row, col, row, 2, QUEENS_CASTLE);
    }
    //Check king's side castling
    if(canCastle[turn - 1].second && (board[row][7] & PIECE_TYPE) == ROOK && !isEnemyPiece(row, 7)) {
        if(board[row][5] + board[row][6] == 0) allMoves.emplace_back(row, col, row, 6, KINGS_CASTLE);
    }
}
void Board::calculateSlidingMoves(int row, int col) {
    int piece = board[row][col];
    int pieceColor = piece & PIECE_COLOR;
    int startIndex = ((piece & PIECE_TYPE) == BISHOP) ? 4 : 0;
    int endIndex = ((piece & PIECE_TYPE) == ROOK) ? 4 : 8;

    for(int i = startIndex; i < endIndex; i++) {
        pair<int, int> direction = slidingDirections[i];
        int nextRow = row;
        int nextCol = col;

        while(true) {
            nextRow += direction.first;
            nextCol += direction.second;

            if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) break;

            int nextSquareColor = board[nextRow][nextCol] & PIECE_COLOR;

            //Square being checked is occupied by a friendly piece
            if(nextSquareColor == pieceColor) break;

            checkMove(Move(row, col, nextRow, nextCol));

            //Square being checked is occupied by an enemy piece, current piece can go no farther
            if(nextSquareColor != NONE) break;
        }
    }
}

void Board::checkMove(Move move) {
    int tempBoard[8][8];
    int kingPos = -1;
    copy(&board[0][0], &board[7][8], &tempBoard[0][0]);

    tempBoard[move.endRow][move.endCol] = tempBoard[move.startRow][move.startCol];
    tempBoard[move.startRow][move.startCol] = 0;

    if((tempBoard[move.endRow][move.endCol] & PIECE_TYPE) == KING) {
        kingPos = move.startRow * 10 + move.startCol;
        kingPositions[turn - 1] = move.endRow * 10 + move.endCol;
    }

    if(!isKingInCheck(tempBoard)) allMoves.push_back(move);

    if(kingPos != -1) kingPositions[turn - 1] = kingPos;
}

bool Board::isEnemyPiece2(int piece, int color) {
    return piece != 0 && (piece & PIECE_COLOR) != color;
}
bool Board::isEnemyPiece(int row, int col) {
    return isEnemyPiece2(board[row][col], turn * 8);
}
bool Board::isEnemyPiece(int piece) {
    return isEnemyPiece2(piece, turn * 8);
}

//Checks if the king is currently in check
bool Board::isKingInCheck(int (*boardToCheck)[8]) {
    int kingPosition = kingPositions[turn - 1];
    int kingRow = kingPosition / 10;
    int kingCol = kingPosition % 10;

    //Check for enemy knights
    for(auto direction : knightDirections) {
        int row = kingRow + direction.first;
        int col = kingCol + direction.second;

        if(row < 0 || row > 7 || col < 0 || col > 7) continue;

        if(isEnemyPiece(boardToCheck[row][col]) && (boardToCheck[row][col] & PIECE_TYPE) == KNIGHT) {
            return true;
        }
    }

    //Check for enemy sliding pieces
    for(int i = 0; i < 8; i++) {
        pair<int, int> direction = slidingDirections[i];
        int row = kingRow + direction.first;
        int col = kingCol + direction.second;

        //Check for enemy king (used to prevent king from moving next to the enemy king)
        if((boardToCheck[row][col] & PIECE_TYPE) == KING) return true;

        while(row >= 0 && row < 8 && col >= 0 && col < 8) {
            int type = boardToCheck[row][col] & PIECE_TYPE;

            if(boardToCheck[row][col] != NONE) {
                if(isEnemyPiece(boardToCheck[row][col])) {
                    //Check for straight-line sliding pieces
                    if(i < 4 && (type == ROOK || type == QUEEN)) {
                        return true;
                    }

                    //Check for diagonal sliding pieces
                    if(i >= 4 && (type == BISHOP || type == QUEEN)) {
                        return true;
                    }
                }

                break;
            }

            row += direction.first;
            col += direction.second;
        }
    }

    //Check for enemy pawns
    int direction = (turn == TURN_WHITE) ? -1 : 1;
    int newRow = kingRow + direction;
    if(newRow >= 0 && newRow < 8) {
        if(kingCol - 1 >= 0 && isEnemyPiece(boardToCheck[newRow][kingCol - 1]) && (boardToCheck[newRow][kingCol - 1] & PIECE_TYPE) == PAWN) return true;
        if(kingCol + 1 < 8 && isEnemyPiece(boardToCheck[newRow][kingCol + 1]) && (boardToCheck[newRow][kingCol + 1] & PIECE_TYPE) == PAWN) return true;
    }

    return false;
}
bool Board::isKingInCheck() {
    return isKingInCheck(board);
}

bool Board::click(int row, int col) {
    for(auto & move : moves) {
        if(move.endRow == row && move.endCol == col) {
            makeMove(move);

            return true;
        }
    }

    isolateMovesForSquare(row, col);
    return false;
}

void Board::makeMove(Move move) {
    int start = move.startRow * 10 + move.startCol;
    int end = move.endRow * 10 + move.endCol;

    if(move.specialMove == NORMAL_MOVE) {
        board[move.endRow][move.endCol] = board[move.startRow][move.startCol];
        board[move.startRow][move.startCol] = 0;

        //Update king position if king moved
        if((board[move.endRow][move.endCol] & PIECE_TYPE) == KING) {
            kingPositions[turn - 1] = end;

            //Castling is no longer possible if king moved
            canCastle[turn - 1] = make_pair(false, false);
        }

        //Check if rooks moved or were captured
        if(turn == TURN_WHITE) {
            //Queen's side rook moved
            if(start == 70) canCastle[0].first = false;
            //King's side rook moved
            if(start == 77) canCastle[0].second = false;
        } else if(turn == TURN_BLACK) {
            //Queen's side rook moved
            if(start == 0) canCastle[1].first = false;
            //King's side rook moved
            if(start == 7) canCastle[1].second = false;
        }
    } else if(move.specialMove == KINGS_CASTLE) {
        int row = move.startRow;
        board[row][6] = board[row][4];
        board[row][5] = board[row][7];
        board[row][4] = 0;
        board[row][7] = 0;
    } else if(move.specialMove == QUEENS_CASTLE) {
        int row = move.startRow;
        board[row][2] = board[row][4];
        board[row][3] = board[row][0];
        board[row][4] = 0;
        board[row][0] = 0;
    }
}

//Returns current board state
//0: normal, 1: white victory, 2: black victory, 3: stalemate, 4: current player is in check
int Board::endTurn() {
    int state = 0;

    moves.clear();
    allMoves.clear();
    turn = (turn == 1) ? 2 : 1;

    if(isKingInCheck()) state = 4;

    findAllMoves();

    return state;
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