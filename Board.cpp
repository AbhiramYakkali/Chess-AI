//
// Created by Abhiram on 3/23/2024.
//

#include <iostream>
#include "Board.h"

using namespace std;

Board::Board(const int b[8][8], const int turn) {
    this->turn = turn;
    enPassantCol = -10;
    inCheck = false;

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
Board::Board(const int b[8][8]) : Board(b, TURN_WHITE) {}

void Board::findAllMoves() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(const int piece = board[i][j]; (piece & PIECE_TYPE) != NONE && (piece & PIECE_COLOR) / 8 == turn) {
                findMovesForSquare(i, j);
            }
        }
    }
}

void Board::findMovesForSquare(const int row, const int col) {
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
            break;
        default:
            cout << "Error finding piece type: " << board[row][col] << endl;
            break;
    }
}

void Board::calculatePawnMoves(const int row, const int col) {
    //Determine direction of travel based on color; black: positive (downwards), white: negative (upwards)
    const int direction = (turn == 1) ? -1 : 1;

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

    //Check en passant
    if(abs(enPassantCol - col) == 1) {
        if((turn == TURN_WHITE && row == 3) || (turn == TURN_BLACK && row == 4)) {
            //En passant is possible
            checkMove(Move(row, col, row + direction, enPassantCol, EN_PASSANT));
        }
    }
}
void Board::calculateKnightMoves(const int row, const int col) {
    for(auto [rowDir, colDir] : knightDirections) {
        const int nextRow = row + rowDir;
        const int nextCol = col + colDir;

        if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) continue;

        if(board[nextRow][nextCol] == NONE || (board[nextRow][nextCol] & PIECE_COLOR) / 8 != turn) {
            checkMove(Move(row, col, nextRow, nextCol));
        }
    }
}
void Board::calculateKingMoves(int row, int col) {
    const int pieceColor = board[row][col] & PIECE_COLOR;

    for(auto [rowDir, colDir] : slidingDirections) {
        const int nextRow = row + rowDir;
        const int nextCol = col + colDir;

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
void Board::calculateSlidingMoves(const int row, const int col) {
    const int piece = board[row][col];
    const int pieceColor = piece & PIECE_COLOR;
    const int startIndex = ((piece & PIECE_TYPE) == BISHOP) ? 4 : 0;
    const int endIndex = ((piece & PIECE_TYPE) == ROOK) ? 4 : 8;

    for(int i = startIndex; i < endIndex; i++) {
        auto [rowDir, colDir] = slidingDirections[i];
        int nextRow = row;
        int nextCol = col;

        while(true) {
            nextRow += rowDir;
            nextCol += colDir;

            if(nextRow < 0 || nextRow > 7 || nextCol < 0 || nextCol > 7) break;

            const int nextSquareColor = board[nextRow][nextCol] & PIECE_COLOR;

            //Square being checked is occupied by a friendly piece
            if(nextSquareColor == pieceColor) break;

            checkMove(Move(row, col, nextRow, nextCol));

            //Square being checked is occupied by an enemy piece, current piece can go no farther
            if(nextSquareColor != NONE) break;
        }
    }
}

void Board::checkMove(const Move& move) {
    int tempBoard[8][8];
    int kingPos = -1;
    copy(&board[0][0], &board[7][8], &tempBoard[0][0]);

    tempBoard[move.endRow][move.endCol] = tempBoard[move.startRow][move.startCol];
    tempBoard[move.startRow][move.startCol] = 0;
    if(move.specialMove == EN_PASSANT) tempBoard[move.startRow][move.endCol] = 0;

    if((tempBoard[move.endRow][move.endCol] & PIECE_TYPE) == KING) {
        kingPos = move.startRow * 10 + move.startCol;
        kingPositions[turn - 1] = move.endRow * 10 + move.endCol;
    }

    if(!isKingInCheck(tempBoard)) allMoves.push_back(move);
    //else cout << "check blocking move: " << move.startRow << move.startCol << " " << move.endRow << move.endCol << endl;

    if(kingPos != -1) kingPositions[turn - 1] = kingPos;
}

bool Board::isEnemyPiece2(const int piece, const int color) {
    return piece != 0 && (piece & PIECE_COLOR) != color;
}
bool Board::isEnemyPiece(const int row, const int col) const {
    return isEnemyPiece2(board[row][col], turn * 8);
}
bool Board::isEnemyPiece(const int piece) const {
    return isEnemyPiece2(piece, turn * 8);
}

//Checks if the king is currently in check
bool Board::isKingInCheck(int (*boardToCheck)[8]) {
    const int kingPosition = kingPositions[turn - 1];
    const int kingRow = kingPosition / 10;
    const int kingCol = kingPosition % 10;

    //Check for enemy knights
    for(auto [rowDir, colDir] : knightDirections) {
        const int row = kingRow + rowDir;
        const int col = kingCol + colDir;

        if(row < 0 || row > 7 || col < 0 || col > 7) continue;

        if(isEnemyPiece(boardToCheck[row][col]) && (boardToCheck[row][col] & PIECE_TYPE) == KNIGHT) {
            return true;
        }
    }

    //Check for enemy sliding pieces
    for(int i = 0; i < 8; i++) {
        auto [rowDir, colDir] = slidingDirections[i];
        int row = kingRow + rowDir;
        int col = kingCol + colDir;

        //Check for enemy king (used to prevent king from moving next to the enemy king)
        if(row >= 0 && row < 8 && col >= 0 && col < 8 && (boardToCheck[row][col] & PIECE_TYPE) == KING) {
            return true;
        }

        while(row >= 0 && row < 8 && col >= 0 && col < 8) {
            const int type = boardToCheck[row][col] & PIECE_TYPE;

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

            row += rowDir;
            col += colDir;
        }
    }

    //Check for enemy pawns
    const int direction = (turn == TURN_WHITE) ? -1 : 1;
    const int newRow = kingRow + direction;
    if(newRow >= 0 && newRow < 8) {
        if(kingCol - 1 >= 0 && isEnemyPiece(boardToCheck[newRow][kingCol - 1]) && (boardToCheck[newRow][kingCol - 1] & PIECE_TYPE) == PAWN) {
            return true;
        }
        if(kingCol + 1 < 8 && isEnemyPiece(boardToCheck[newRow][kingCol + 1]) && (boardToCheck[newRow][kingCol + 1] & PIECE_TYPE) == PAWN) {
            return true;
        }
    }

    return false;
}
bool Board::isKingInCheck() {
    return isKingInCheck(board);
}

bool Board::click(const int row, const int col) {
    for(const auto & move : moves) {
        if(move.endRow == row && move.endCol == col) {
            makeMove(move);

            return true;
        }
    }

    isolateMovesForSquare(row, col);
    return false;
}

void Board::makeMove(const Move& move) {
    enPassantCol = -10;

    const int start = move.startRow * 10 + move.startCol;
    const int end = move.endRow * 10 + move.endCol;

    //Update king position if king moves
    //Castling is no longer possible if king moves
    if((board[move.startRow][move.startCol] & PIECE_TYPE) == KING) {
        kingPositions[turn - 1] = end;
        canCastle[turn - 1] = make_pair(false, false);
    }

    if(move.specialMove == NORMAL_MOVE) {
        board[move.endRow][move.endCol] = board[move.startRow][move.startCol];
        board[move.startRow][move.startCol] = 0;

        if((board[move.endRow][move.endCol] & PIECE_TYPE) == PAWN && abs(move.endRow - move.startRow) == 2) {
            //Pawn moved forward two squares, keep track to check for en passant
            enPassantCol = move.endCol;
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
        const int row = move.startRow;
        board[row][6] = board[row][4];
        board[row][5] = board[row][7];
        board[row][4] = 0;
        board[row][7] = 0;
    } else if(move.specialMove == QUEENS_CASTLE) {
        const int row = move.startRow;
        board[row][2] = board[row][4];
        board[row][3] = board[row][0];
        board[row][4] = 0;
        board[row][0] = 0;
    } else if(move.specialMove == EN_PASSANT) {
        board[move.endRow][move.endCol] = board[move.startRow][move.startCol];
        board[move.startRow][move.startCol] = 0;
        //Capture pawn with en passant
        board[move.startRow][move.endCol] = 0;
    }
}

Board Board::makeBoardForMove(const Move& move) const {
    auto output = Board(board, turn);
    output.makeMove(move);
    return output;
}

//Returns current board state
//0: normal, 1: white victory, 2: black victory, 3: stalemate, 4: current player is in check
int Board::endTurn() {
    int state = NORMAL_STATE;

    moves.clear();
    allMoves.clear();
    turn = (turn == 1) ? 2 : 1;

    inCheck = isKingInCheck();
    if(inCheck) state = IN_CHECK;

    findAllMoves();

    if(allMoves.empty()) {
        //No moves can be made, current outcome is stalemate or checkmate
        //Current player is in check, therefore player is checkmated (lost)
        if(inCheck) state = (turn == 1) ? BLACK_MATE : WHITE_MATE;
        //Current player is not in check, therefore result is stalemate (draw)
        else state = STALEMATE;
    }

    return state;
}

void Board::isolateMovesForSquare(const int row, const int col) {
    moves.clear();

    int piece = board[row][col];
    if(piece != NONE && (piece & PIECE_COLOR) / 8 == turn) {
        for(auto & move : allMoves) {
            if(move.startRow == row && move.startCol == col) moves.push_back(move);
        }
    }
}

string Board::getFENString() const {
    string FEN;

    for(const auto & row : board) {
        int emptyCols = 0;
        for(const auto & square : row) {
            if(square == NONE) {
                emptyCols++;
            } else {
                //There is a piece on this square
                if(emptyCols != 0) FEN.append(to_string(emptyCols));

                emptyCols = 0;
                const int piece = square;
                char letter;

                switch(piece & PIECE_TYPE) {
                    case ROOK:
                        letter = 'R';
                        break;
                    case KNIGHT:
                        letter = 'N';
                        break;
                    case BISHOP:
                        letter = 'B';
                        break;
                    case QUEEN:
                        letter = 'Q';
                        break;
                    case KING:
                        letter = 'K';
                        break;
                    case PAWN:
                        letter = 'P';
                        break;
                    default:
                        letter = ' ';
                        break;
                }

                if((piece & PIECE_COLOR) == BLACK) letter = tolower(letter);
                FEN += letter;
            }
        }
        if(emptyCols != 0) FEN.append(to_string(emptyCols));
        FEN += '/';
    }

    return FEN;
}

vector<Move> Board::getAllMoves() {
    return allMoves;
}
vector<Move> Board::getMoves() {
    return moves;
}

int (*Board::getBoard())[8] {
    return board;
}