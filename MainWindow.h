//
// Created by Abhiram on 3/23/2024.
//

#ifndef CHESS_AI_MAINWINDOW_H
#define CHESS_AI_MAINWINDOW_H


#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <string>

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);
    void setBoard(int board[][8]);
    void setSelectedSquare(int row, int col);
    void setGameState(int state);
    void setFEN(std::string FEN);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int gameState;
    QString FEN;

    void loadPixmaps();

    QVector<QPixmap> pieceImages;
};


#endif //CHESS_AI_MAINWINDOW_H
