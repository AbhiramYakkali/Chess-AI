#include <iostream>
#include <QApplication>
#include <QDebug>
#include "MainWindow.h"
#include "main.h"
#include "Board.h"

MainWindow* mainWindow;
Board* board;

int startingBoard[64];

bool playing = false;
//1: white, 2: black
int turn = 1;

void initializeBoard() {
    startingBoard[0] = BLACK | ROOK;
    startingBoard[1] = BLACK | KNIGHT;
    startingBoard[2] = BLACK | BISHOP;
    startingBoard[3] = BLACK | QUEEN;
    startingBoard[4] = BLACK | KING;
    startingBoard[5] = BLACK | BISHOP;
    startingBoard[6] = BLACK | KNIGHT;
    startingBoard[7] = BLACK | ROOK;
    startingBoard[8] = BLACK | PAWN;
    startingBoard[9] = BLACK | PAWN;
    startingBoard[10] = BLACK | PAWN;
    startingBoard[11] = BLACK | PAWN;
    startingBoard[12] = BLACK | PAWN;
    startingBoard[13] = BLACK | PAWN;
    startingBoard[14] = BLACK | PAWN;
    startingBoard[15] = BLACK | PAWN;

    startingBoard[56] = WHITE | ROOK;
    startingBoard[57] = WHITE | KNIGHT;
    startingBoard[58] = WHITE | BISHOP;
    startingBoard[59] = WHITE | QUEEN;
    startingBoard[60] = WHITE | KING;
    startingBoard[61] = WHITE | BISHOP;
    startingBoard[62] = WHITE | KNIGHT;
    startingBoard[63] = WHITE | ROOK;
    startingBoard[48] = WHITE | PAWN;
    startingBoard[49] = WHITE | PAWN;
    startingBoard[50] = WHITE | PAWN;
    startingBoard[51] = WHITE | PAWN;
    startingBoard[52] = WHITE | PAWN;
    startingBoard[53] = WHITE | PAWN;
    startingBoard[54] = WHITE | PAWN;
    startingBoard[55] = WHITE | PAWN;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    initializeBoard();

    mainWindow = new MainWindow();
    mainWindow->setBoard(startingBoard);
    mainWindow->show();

    board = new Board(startingBoard);

    playing = true;

    return QApplication::exec();
}

void main::mouseClick(int row, int col) {
    if(playing) {
        if(board->click(row * 8 + col)) {
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

    board->endTurn();
}

std::vector<Move> main::getMoves() {
    return board->getMoves();
}
