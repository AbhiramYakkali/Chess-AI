#include <iostream>
#include <QApplication>
#include <QDebug>
#include "MainWindow.h"
#include "main.h"
#include "Board.h"
#include "Bot.h"

using namespace std;

MainWindow* mainWindow;
Board* board;

int startingBoard[8][8];
//FEN string corresponding to the starting board
string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/";

bool playing = false;
//1: white, 2: black
int turn = 1;
//Set this to the color the AI should play as
//-1 to play without AI
int aiColor = -1;

void generateBoardFromFEN(string FEN) {
    int c = 0, row = 0, col = 0;
    string uppercase = FEN;

    transform(FEN.begin(), FEN.end(), uppercase.begin(), ::toupper);

    while(row < 8) {
        char character = uppercase[c];

        if(character == '/') {
            row++;
            c++;
            col = 0;
            continue;
        }

        if(isdigit(character)) {
            c++;
            col += character - '0';
            continue;
        }

        //The current character indicates the existence of a piece
        int piece = 0;

        switch(character) {
            case 'R':
                piece = ROOK;
                break;
            case 'N':
                piece = KNIGHT;
                break;
            case 'B':
                piece = BISHOP;
                break;
            case 'K':
                piece = KING;
                break;
            case 'Q':
                piece = QUEEN;
                break;
            case 'P':
                piece = PAWN;
                break;
        }
        //Lowercase characters represent black pieces, uppercase represent white
        if(isupper(FEN[c])) piece |= WHITE;
        else piece |= BLACK;

        startingBoard[row][col] = piece;

        col++;
        c++;
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    generateBoardFromFEN(startingFEN);

    mainWindow = new MainWindow();
    mainWindow->setBoard(startingBoard);
    mainWindow->setGameState(0);
    mainWindow->setFEN(startingFEN);
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

    int state = board->endTurn();

    mainWindow->setBoard(board->getBoard());
    mainWindow->setSelectedSquare(-1, -1);
    mainWindow->setFEN(board->getFENString());
    mainWindow->setGameState(state);

    //If state is not 0 (normal) or 4 (check) then the game has ended
    if(state != 0 && state != 4) {
        playing = false;
        return;
    }

    //Check if it's the AI's turn to make a move
    if(aiColor == turn) {
        board->makeMove(Bot::makeMove(*board, aiColor));
        endTurn();
    }
}

std::vector<Move> main::getMoves() {
    return board->getMoves();
}
