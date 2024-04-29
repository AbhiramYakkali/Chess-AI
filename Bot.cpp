#include "Bot.h"

using namespace std;

//Evaluates a given board state and returns an integer
//Positive numbers: white advantage; negative numbers: black advantage
int evaluate(int board[][8]) {
    int eval = 0;

    //Iterate through the board and add up the pieces on each side
    //Piece values: pawn: 1, knight/bishop: 3, rook: 5, queen: 9
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(board[i][j] != NONE) {
                //How much the current eval should be changed by due to this piece
                int evalMod;
                //White pieces contribute positively, black pieces contribute negatively
                if((board[i][j] & PIECE_COLOR) == WHITE) {
                    evalMod = 1;
                } else {
                    evalMod = -1;
                }

                switch((board[i][j] & PIECE_TYPE)) {
                    case BISHOP:
                    case KNIGHT:
                        evalMod *= 3;
                        break;
                    case ROOK:
                        evalMod *= 5;
                        break;
                    case QUEEN:
                        evalMod *= 9;
                        break;
                }

                eval += evalMod;
            }
        }
    }

    return eval;
}

Move Bot::makeMove(Board board, int color) {
    vector<Move> moves = board.getAllMoves();

    //Keep track of the best move
    //Pair: eval of best move found, best move found
    pair<int, Move> bestMove(-1, Move(0, 0, 0, 0));

    //Iterate through all possible moves to find the best one
    for(Move move : moves) {
        Board tempBoard = board.makeBoardForMove(move);
        tempBoard.endTurn();
        int eval = evaluate(tempBoard.getBoard());
        if(color == TURN_BLACK) eval *= -1;

        if(bestMove.first == -1 || eval > bestMove.first) {
            bestMove = make_pair(eval, move);
        }
    }

    return bestMove.second;
}