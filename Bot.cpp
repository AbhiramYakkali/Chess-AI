#include "Bot.h"
#include <chrono>
#include <iostream>

using namespace std;

#define DEFAULT_DEPTH 3

//Piece-square tables
int pawn_table[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};
int knight_table[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
int bishop_table[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
int rook_table[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};
int queen_table[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
int king_table[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int* piece_tables[6] = {king_table, pawn_table, knight_table, bishop_table, rook_table, queen_table};

int positions = 0;

//Evaluates a given board state and returns an integer
//Positive numbers: white advantage; negative numbers: black advantage
//Always returns a positive number (greater numbers are better for the AI)
int evaluate(int board[][8], const int color) {
    positions++;
    int eval = 0;

    //Iterate through the board and add up the pieces on each side
    //Piece values: pawn: 1, knight/bishop: 3, rook: 5, queen: 9
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(board[i][j] != NONE) {
                //How much the current eval should be changed by due to this piece
                int evalMod = 100;
                const int pieceType = board[i][j] & PIECE_TYPE;

                switch(pieceType) {
                    case BISHOP:
                    case KNIGHT:
                        evalMod = 300;
                        break;
                    case ROOK:
                        evalMod = 500;
                        break;
                    case QUEEN:
                        evalMod = 900;
                        break;
                    default:
                        break;
                }

                //Consider piece position into evaluation using piece tables
                //Table should be flipped if current color is black
                const int row = color == BLACK ? 7 - i : i;
                evalMod += piece_tables[pieceType - 1][row * 8 + j];

                //White pieces contribute positively, black pieces contribute negatively
                eval += (board[i][j] & PIECE_COLOR) == WHITE ? evalMod : -evalMod;
            }
        }
    }

    return color == TURN_WHITE ? eval : eval * -1;
}

int search(Board board, const int depth, int alpha, int beta, const bool max, const int color) {
    if(const int outcome = board.endTurn(); outcome != NORMAL_STATE && outcome != IN_CHECK) {
        if(outcome == color) {
            //Current player win
            return 100000;
        }

        if(outcome == STALEMATE) {
            return 0;
        }

        //Other player win
        return -100000;
    }

    for(auto moves = board.getAllMoves(); Move move : moves) {
        if(depth == 0) {
            //If depth is 0, evaluate the current board state and return the eval
            return evaluate(board.makeBoardForMove(move).getBoard(), color);
        }

        const int eval = search(board.makeBoardForMove(move), depth - 1, alpha, beta, !max, color);

        //Update alpha and beta if better moves are found
        if(max && eval > alpha) alpha = eval;
        if(!max && eval < beta) beta = eval;

        //Alpha-beta pruning
        if(beta <= alpha) break;
    }

    if(max) return alpha;
    return beta;
}

Move Bot::makeMove(Board board, const int color) {
    const auto startTime = chrono::high_resolution_clock::now();
    positions = 0;

    const vector<Move> moves = board.getAllMoves();

    //Keep track of the best move and eval
    Move bestMove = moves[0];
    int bestEval = search(board.makeBoardForMove(moves[0]), DEFAULT_DEPTH, -10000, 10000, false, color);

    //Iterate through all possible moves to find the best one
    for(int i = 1; i < moves.size(); i++) {
        Move move = moves[i];
        int eval = search(board.makeBoardForMove(move), DEFAULT_DEPTH, -10000, 10000, false, color);

        if(eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }

    const auto elapsedTime = chrono::high_resolution_clock::now() - startTime;
    cout << "Evaluated " << positions << " positions in " << elapsedTime / chrono::milliseconds(1) <<
            "ms, best position found: " << static_cast<float>(bestEval) / 100.0f << endl;

    return bestMove;
}