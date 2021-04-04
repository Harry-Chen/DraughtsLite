#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QPair>
#include <functional>

#include "utilities/gameconfig.h"
#include "chesscell.h"
#include "move.h"

class ChessBoard : public QObject
{
    Q_OBJECT
public:
    ChessBoard(GameConfig _config);
    GameConfig config;
    ChessCell* getCell(int index);
    QList<int> getSelectableIndecies();
    QList<MovePath> getPossibleMoveForCell(int index);
    friend class Controller;

public slots:
    void recalculateMoves();

signals:
    void calculateMovesDone();

private:
    QVector<ChessCell> cells;
    QMap<int, MovePath> possibleMoves;
    int searchInfo[51];
    int eatInfo[51];
    bool mark[51];

    static int frontLeftIndex(int index);
    static int frontRightIndex(int index);
    static int backLeftIndex(int index);
    static int backRightIndex(int index);

    QPair<int, QVector<MovePath>> getAvailableMoveForIndex(int index);

    bool searchEatingJumpForIndex(int step, int index, bool isKing, eColor color,
                                  std::function<bool(int)> callback);

    std::function<int(int)> directionsForWhite[4] = {
        frontLeftIndex,
        frontRightIndex,
        backLeftIndex,
        backRightIndex
    };

    std::function<int(int)> directionsForBlack[4] = {
        backLeftIndex,
        backRightIndex,
        frontLeftIndex,
        frontRightIndex
    };

};

#endif // CHESSBOARD_H
