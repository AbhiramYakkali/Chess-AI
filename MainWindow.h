//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_MAINWINDOW_H
#define CHESS_AI_MAINWINDOW_H


#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <string>

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr, int height = 1080, int width = 1920);
    static void setBoard(int board[][8]);
    void setSelectedSquare(int row, int col);
    void setGameState(int state);
    void setFEN(const std::string& fen);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int gameState;
    QString FEN;
    int HEIGHT, WIDTH, vertOffset, sqLength, pieceLength, pieceCenteringOffset, horizStart;

    void loadPixmaps();

    QVector<QPixmap> pieceImages;
};

#endif //CHESS_AI_MAINWINDOW_H
