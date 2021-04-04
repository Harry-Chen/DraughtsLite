#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QObject>
#include <QWidget>

#include "logic/chessboard.h"
#include "gui/waitingwidget.h"
#include "logic/move.h"
#include "gui/soundplayer.h"

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent = nullptr);
    void setBoard(ChessBoard *_board);
    void paintEvent(QPaintEvent *p) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enableWaiting(const QString& text);
    void disableWaiting();
    bool playing = false;

    enum TerminateType{
        WIN,
        LOSE,
        DRAW
    };

signals:
    void performMove(MovePath path);
    void playSound(SoundPlayer::Sounds sound);
    void opponentMoveDone();
    void gameLost();

public slots:
    void handleMoveFromNetwork(MovePath path);
    void terminateGame(TerminateType type);

private:
    const int CELL_WIDTH = 40;
    const QPoint DRAW_OFFSET = QPoint(5, 5);
    const QMargins RECT_MARGIN = QMargins(-5, -5, 5, 5);
    int nowSelectedIndex = -1;
    QPainter *p;
    ChessBoard *board;
    QList<int> selectableIndecies;
    QList<MovePath> nowPossibleMoves;
    bool inSelectionMode = false;
    bool rotation = false;
    WaitingWidget *wait;

    void drawBackground();
    void deselectAll();
    void select(int index);
    QRect getRectForCell(int row, int column);
    QRect getPositionForCellIndex(int index);
    int getCellIndexByPosition(QPoint position);
    void drawSelectableCells();
    void moveOnBoard(MovePath path);
    void updateData();

    const QPixmap WHITE_NORMAL = QPixmap(":/resources/normal_white.png").scaled(30, 30);
    const QPixmap BLACK_NORMAL = QPixmap(":/resources/normal_black.png").scaled(30, 30);
    const QPixmap WHITE_KING = QPixmap(":/resources/king_white.png").scaled(30, 30);
    const QPixmap BLACK_KING = QPixmap(":/resources/king_black.png").scaled(30, 30);
    const QPixmap &getPixmapForCell(const ChessCell& cell);
};

#endif // BOARDWIDGET_H
