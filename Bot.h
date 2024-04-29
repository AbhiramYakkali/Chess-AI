#ifndef CHESS_AI_BOT_H
#define CHESS_AI_BOT_H

#include "main.h"
#include "board.h"


class Bot {
public:
    static Move makeMove(Board board, int color);
};


#endif //CHESS_AI_BOT_H
