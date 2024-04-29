#include "Bot.h"

using namespace std;

Move Bot::makeMove(Board board) {
    vector<Move> moves = board.getAllMoves();

    return moves[0];
}