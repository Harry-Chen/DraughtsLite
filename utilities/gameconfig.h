#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QMetaType>
#include <QDataStream>

#include "enumerates.h"
#include "logic/chesscell.h"

struct GameConfig {
    quint16 port;
    eColor userColor;
    eColor firstColor;
    ChessCell cells[51];


    GameConfig() {}

    // Stream overload
    friend QDataStream & operator << (QDataStream &arch, const GameConfig &object)
    {
        arch << static_cast<quint8>(object.userColor);
        arch << static_cast<quint8>(object.firstColor);
        for(int i=1;i<=50;++i){
            arch << static_cast<quint8>(object.cells[i].type);
            arch << static_cast<quint8>(object.cells[i].color);
        }
        return arch;
    }
    friend QDataStream & operator >> (QDataStream &arch, GameConfig &object)
    {
        quint8 userColor, firstColor;
        arch >> userColor >> firstColor;
        object.userColor = static_cast<eColor>(userColor);
        object.firstColor = static_cast<eColor>(firstColor);
        for(int i=1;i<=50;++i){
            quint8 type, color;
            arch >> type >> color;
            object.cells[i].type = static_cast<ChessCellType>(type);
            object.cells[i].color = static_cast<eColor>(color);
        }

        return arch;
    }

};
Q_DECLARE_METATYPE(GameConfig)
#endif // GAMECONFIG_H
