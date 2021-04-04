#ifndef CHESSCELL_H
#define CHESSCELL_H

#include <QLabel>

#include "utilities/enumerates.h"

class ChessCell
{
public:
    ChessCell();
    ChessCell(eColor _color, ChessCellType _type = NORMAL);
    eColor color;
    ChessCellType type;
    QLabel *label;
};

#endif // CHESSCELL_H
