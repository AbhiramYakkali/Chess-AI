#include "Bot.h"
#include <chrono>
#include <iostream>

using namespace std;

#define DEFAULT_DEPTH 3

int positions = 0;

//Evaluates a given board state and returns an integer
//Positive numbers: white advantage; negative numbers: black advantage
//Always returns a positive number (greater numbers are better for the AI)
int evaluate(int board[][8], int color) {
    positions++;
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

    return (color == TURN_WHITE) ? eval : eval * (-1);
}

int search(Board board, int depth, int alpha, int beta, bool max, int color) {
    int outcome = board.endTurn();
    if(outcome != NORMAL_STATE) {
        //TODO: Handle non-normal outcomes
    }

    vector<Move> moves = board.getAllMoves();

    for(Move move : moves) {
        int eval;
        if(depth == 0) {
            //If depth is 0, evaluate the current board state and return the eval
            return evaluate(board.makeBoardForMove(move).getBoard(), color);
        } else {
            eval = search(board.makeBoardForMove(move), depth - 1, alpha, beta, !max, color);

            //Update alpha and beta if better moves are found
            if(max && eval > alpha) alpha = eval;
            if(!max && eval < beta) beta = eval;

            //Alpha-beta pruning
            if(beta <= alpha) break;
        }
    }

    if(max) return alpha;
    else return beta;
}

Move Bot::makeMove(Board board, int color) {
    auto startTime = chrono::high_resolution_clock::now();
    positions = 0;

    vector<Move> moves = board.getAllMoves();

    //Keep track of the best move and eval
    Move bestMove = moves[0];
    int bestEval = search(board.makeBoardForMove(moves[0]), DEFAULT_DEPTH, -1000, 1000, false, color);

    //Iterate through all possible moves to find the best one
    for(int i = 1; i < moves.size(); i++) {
        Move move = moves[i];
        int eval = search(board.makeBoardForMove(move), DEFAULT_DEPTH, -1000, 1000, false, color);

        if(eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }

    auto elapsedTime = chrono::high_resolution_clock::now() - startTime;
    cout << "Evaluated " << positions << " positions in " << elapsedTime/chrono::milliseconds(1) << "ms, best position found: " << bestEval << endl;

    return bestMove;
}