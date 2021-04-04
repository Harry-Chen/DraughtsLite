#include "chesscell.h"

ChessCell::ChessCell(): type(EMPTY), color(RANDOM){

}

ChessCell::ChessCell(eColor _color, ChessCellType _type):
    color(_color), type(_type)
{

}
