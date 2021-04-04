#ifndef ENUMERATES_H
#define ENUMERATES_H

enum eColor {
    WHITE,
    BLACK,
    RANDOM
};

enum OperationType {
    INITIALIZE,
    MOVE,
    DRAWREQUEST,
    RESIGN
};

enum ChessCellType {
    EMPTY,
    NORMAL,
    KING
};

#endif // ENUMERATES_H
