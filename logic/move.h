#ifndef MOVE_H
#define MOVE_H

#include <QVector>

struct Move{
    int startIndex;
    int endIndex;
    int takeIndex;
};

using MovePath = QVector<Move>;

#endif // MOVE_H
