//
// Created by Abhiram on 3/23/2024.
//

using namespace std;

#include "MainWindow.h"
#include "main.h"
#include <QPixmap>

const int HEIGHT = 1080, WIDTH = 1920;
const int vertOffset = 8;
const int sqLength = (HEIGHT - vertOffset * 2) / 8;
const int pieceLength = sqLength * 7 / 8;
const int pieceCenteringOffset = (sqLength - pieceLength) / 2;
const int horizStart = (WIDTH - (sqLength * 8)) / 2;

int gameBoard[8][8];

pair<int, int> selectedSquare(-1, -1);

//Colors
const QColor white("#EEEED2");
const QColor black("#769656");
const QColor red("#ff5252");
const QColor bg("#312E2B");

MainWindow::MainWindow(QWidget *parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setWindowState(Qt::WindowMaximized);

    loadPixmaps();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setBrush(bg);
    painter.setPen(bg);
    painter.drawRect(0, 0, WIDTH, HEIGHT);

    //Draw the startingBoard
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            if(selectedSquare.first == row && selectedSquare.second == col) {
                painter.setBrush(red);
                painter.setPen(red);
            } else if((row + col) % 2 == 0) {
                painter.setBrush(white);
                painter.setPen(white);
            } else {
                painter.setBrush(black);
                painter.setPen(black);
            }

            painter.drawRect(horizStart + sqLength * col, vertOffset + sqLength * row, sqLength, sqLength);

            if((gameBoard[row][col] & PIECE_TYPE) != NONE) {
                //Piece exists on this square, draw it
                painter.drawPixmap(horizStart + sqLength * col + pieceCenteringOffset,
                                   vertOffset + sqLength * row + pieceCenteringOffset, pieceImages[gameBoard[row][col] - 9].
                                scaled(pieceLength, pieceLength, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }

    //Fill in possible moves (if any)
    painter.setPen(red);
    std::vector<Move> moves = main::getMoves();
    for(auto & move : moves) {
        int row = move.endRow;
        int col = move.endCol;

        if(gameBoard[row][col] != 0) {
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(QPoint(horizStart + sqLength * col + sqLength / 2, vertOffset + sqLength * row + sqLength / 2), sqLength / 2, sqLength / 2);
        }
        else {
            painter.setBrush(red);
            painter.drawEllipse(QPoint(horizStart + sqLength * col + sqLength / 2, vertOffset + sqLength * row + sqLength / 2), sqLength / 5, sqLength / 5);
        }
    }
}

void MainWindow::setBoard(int board[][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            gameBoard[i][j] = board[i][j];
        }
    }
}

//Pixmaps are loaded in order: king, pawn, knight, bishop, rook, queen
//2 blank pixmaps after white pieces, then black pieces are loaded in same order
//Access images: pieceImages[piece - 9]
void MainWindow::loadPixmaps() {
    pieceImages.append(QPixmap("images/white_king.png"));
    pieceImages.append(QPixmap("images/white_pawn.png"));
    pieceImages.append(QPixmap("images/white_knight.png"));
    pieceImages.append(QPixmap("images/white_bishop.png"));
    pieceImages.append(QPixmap("images/white_rook.png"));
    pieceImages.append(QPixmap("images/white_queen.png"));

    pieceImages.append(QPixmap(""));
    pieceImages.append(QPixmap(""));

    pieceImages.append(QPixmap("images/black_king.png"));
    pieceImages.append(QPixmap("images/black_pawn.png"));
    pieceImages.append(QPixmap("images/black_knight.png"));
    pieceImages.append(QPixmap("images/black_bishop.png"));
    pieceImages.append(QPixmap("images/black_rook.png"));
    pieceImages.append(QPixmap("images/black_queen.png"));
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    int x = event->position().x();
    int y = event->position().y();

    int col = (x - horizStart) / sqLength;
    int row = (y - vertOffset) / sqLength;

    //Check if mouse press was within game bounds
    if(col > -1 && col < 8 && row > -1 && row < 8) {
        main::mouseClick(row, col);
    } else {
        setSelectedSquare(-1, -1);
    }

    repaint();

    QMainWindow::mousePressEvent(event);
}

void MainWindow::setSelectedSquare(int row, int col) {
    selectedSquare = make_pair(row, col);
}
