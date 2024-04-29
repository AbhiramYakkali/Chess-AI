#include <iostream>
#include <QApplication>
#include <QDebug>
#include "MainWindow.h"
#include "main.h"
#include "Board.h"
#include "Bot.h"

MainWindow* mainWindow;
Board* board;

int startingBoard[8][8];

bool playing = false;
//1: white, 2: black
int turn = 1;
//Set this to the color the AI should play as
//-1 to play without AI
int aiColor = 2;

void initializeBoard() {
    startingBoard[0][0] = BLACK | ROOK;
    startingBoard[0][1] = BLACK | KNIGHT;
    startingBoard[0][2] = BLACK | BISHOP;
    startingBoard[0][3] = BLACK | QUEEN;
    startingBoard[0][4] = BLACK | KING;
    startingBoard[0][5] = BLACK | BISHOP;
    startingBoard[0][6] = BLACK | KNIGHT;
    startingBoard[0][7] = BLACK | ROOK;
    startingBoard[1][0] = BLACK | PAWN;
    startingBoard[1][1] = BLACK | PAWN;
    startingBoard[1][2] = BLACK | PAWN;
    startingBoard[1][3] = BLACK | PAWN;
    startingBoard[1][4] = BLACK | PAWN;
    startingBoard[1][5] = BLACK | PAWN;
    startingBoard[1][6] = BLACK | PAWN;
    startingBoard[1][7] = BLACK | PAWN;

    startingBoard[7][0] = WHITE | ROOK;
    startingBoard[7][1] = WHITE | KNIGHT;
    startingBoard[7][2] = WHITE | BISHOP;
    startingBoard[7][3] = WHITE | QUEEN;
    startingBoard[7][4] = WHITE | KING;
    startingBoard[7][5] = WHITE | BISHOP;
    startingBoard[7][6] = WHITE | KNIGHT;
    startingBoard[7][7] = WHITE | ROOK;
    startingBoard[6][0] = WHITE | PAWN;
    startingBoard[6][1] = WHITE | PAWN;
    startingBoard[6][2] = WHITE | PAWN;
    startingBoard[6][3] = WHITE | PAWN;
    startingBoard[6][4] = WHITE | PAWN;
    startingBoard[6][5] = WHITE | PAWN;
    startingBoard[6][6] = WHITE | PAWN;
    startingBoard[6][7] = WHITE | PAWN;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    initializeBoard();

    mainWindow = new MainWindow();
    mainWindow->setBoard(startingBoard);
    mainWindow->setGameState(0);
    mainWindow->show();

    board = new Board(startingBoard);

    playing = true;

    return QApplication::exec();
}

void main::mouseClick(int row, int col) {
    if(playing) {
        if(board->click(row, col)) {
            endTurn();
        } else {
            mainWindow->setSelectedSquare(row, col);
        }
    }
}

void main::endTurn() {
    turn = (turn == 1) ? 2 : 1;

    mainWindow->setBoard(board->getBoard());
    mainWindow->setSelectedSquare(-1, -1);

    int state = board->endTurn();
    mainWindow->setGameState(state);

    //If state is not 0 (normal) or 4 (check) then the game has ended
    if(state != 0 && state != 4) {
        playing = false;
        return;
    }

    //Check if it's the AI's turn to make a move
    if(aiColor == turn) {
        board->makeMove(Bot::makeMove(*board));
        endTurn();
    }
}

std::vector<Move> main::getMoves() {
    return board->getMoves();
}
