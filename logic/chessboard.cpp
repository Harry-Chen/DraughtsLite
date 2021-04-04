#include <QDebug>
#include <cstring>
#include "chessboard.h"

ChessBoard::ChessBoard(GameConfig _config)
{
    config = _config;
    cells.fill(ChessCell(), 51);
    /*for(auto i = 1;i<=20;++i){
        cells[i] = ChessCell(eColor::BLACK);
    }
    for(auto i = 31;i<=50;++i){
        cells[i] = ChessCell(eColor::WHITE);
    }*/
    //cells[7] = ChessCell(eColor::WHITE);
    //cells[44] = ChessCell(eColor::BLACK);
    for(auto i = 1;i<=50;++i){
        cells[i] = config.cells[i];
    }
}

ChessCell* ChessBoard::getCell(int index)
{
    return &(cells.data()[index]);
}

QList<int> ChessBoard::getSelectableIndecies()
{
    return possibleMoves.uniqueKeys();
}

QList<MovePath> ChessBoard::getPossibleMoveForCell(int index)
{
    return possibleMoves.values(index);
}

void ChessBoard::recalculateMoves()
{
    possibleMoves.clear();
    int longestStep = 0;

    for(int i=1;i<=50;++i){
        if(cells.at(i).type == EMPTY || cells.at(i).color != config.userColor){
            continue;
        }
        auto availableMoveInfo = getAvailableMoveForIndex(i);
        if(longestStep == availableMoveInfo.first){
            for(const auto &path : availableMoveInfo.second){
                possibleMoves.insertMulti(i, path);
            }
        }else if(longestStep < availableMoveInfo.first){
            longestStep = availableMoveInfo.first;
            possibleMoves.clear();
            for(const auto &path : availableMoveInfo.second){
                possibleMoves.insertMulti(i, path);
            }
        }
    }

    emit calculateMovesDone();
}

QPair<int, QVector<MovePath>> ChessBoard::getAvailableMoveForIndex(int index) {
    int longestStep = 0;
    QVector<MovePath> move;

    auto isKing = cells.at(index).type == KING;
    auto color = cells.at(index).color;

    //non-eating move
    if(!isKing){
        auto directions = config.userColor == WHITE? directionsForWhite : directionsForBlack;
        for(int i = 0; i < 2; ++i){
            int newIndex = directions[i](index);
            if(newIndex != -1 && cells.at(newIndex).type == EMPTY){
                longestStep = 1;
                move.push_back({{index, newIndex, -1}});
            }
        }
    }else{
        for(int i = 0;i < 4; ++i){
            int newIndex = directionsForWhite[i](index);
            while(newIndex != -1 && cells.at(newIndex).type == EMPTY){
                longestStep = 1;
                move.push_back({{index, newIndex, -1}});
                newIndex = directionsForWhite[i](newIndex);
            }
        }
    }

    //eating move
    memset(mark, false, sizeof(mark));
    searchEatingJumpForIndex(0, index, isKing, color, [&](int step) -> bool {
        int totalStep = step * 2;
        qDebug() << "Total step" << totalStep;
        if(totalStep <= 1 || longestStep > totalStep){
            return false;
        }
        MovePath path;
        for(int i = 0;i<step;++i){
            path.push_back({searchInfo[i], searchInfo[i + 1], eatInfo[i]});
        }
        if(longestStep == totalStep){
            move.push_back(path);
        }
        else if(longestStep < totalStep){
            longestStep = totalStep;
            move.clear();
            move.push_back(path);
        }

        return false;
    });

    return {longestStep, move};
}

bool ChessBoard::searchEatingJumpForIndex(int step, int index, bool isKing, eColor color,
                                          std::function<bool (int)> callback) {
    bool isFinal = true;
    searchInfo[step] = index;
    for(int i = 0;i < 4;++i){
        if(!isKing){
            int step1 = directionsForWhite[i](index);
            int step2 = directionsForWhite[i](step1);
            if(step2 == -1 || mark[step1]){
                continue;
            }
            if(cells.at(step2).type == EMPTY && cells.at(step1).type != EMPTY && cells.at(step1).color != color){
                qDebug() << "Can eat" << step1;
                mark[step1] = true;
                eatInfo[step] = step1;
                if(searchEatingJumpForIndex(step + 1, step2, false, color, callback)){
                    return true;
                }
                mark[step1] = false;
                isFinal = false;
            }
        }
        else{
            eColor firstMeet = eColor::RANDOM;
            int nextChessIndex = index;
            do{
                nextChessIndex = directionsForWhite[i](nextChessIndex);
                if(nextChessIndex != -1 && cells.at(nextChessIndex).type != ChessCellType::EMPTY){
                    firstMeet = cells.at(nextChessIndex).color;
                    break;
                }
            }
            while(nextChessIndex != -1);

            if(firstMeet != color && !mark[nextChessIndex]){
                mark[nextChessIndex] = true;
                eatInfo[step] = nextChessIndex;
                int nextIndex = directionsForWhite[i](nextChessIndex);

                while(nextIndex != -1 && cells.at(nextIndex).type == ChessCellType::EMPTY){
                    if(searchEatingJumpForIndex(step + 1, nextIndex, true, color, callback)){
                        return true;
                    }
                    nextIndex = directionsForWhite[i](nextIndex);
                    isFinal = false;
                }

                mark[nextChessIndex] = false;
            }

        }
    }
    if(isFinal){
        if(callback(step)){
            return true;
        }
    }
    return false;
}


int ChessBoard::frontLeftIndex(int index){
    if(index <= 5 || index % 10 == 6 || index == -1){
        return -1;
    }
    else if(((index - 1) / 5) % 2 == 1){
        return index - 6;
    }
    else{
        return index - 5;
    }
}

int ChessBoard::frontRightIndex(int index){
    if(index <= 5 || index % 10 == 5 || index == -1){
        return -1;
    }
    else if(((index - 1) / 5) % 2 == 1){
        return index - 5;
    }
    else{
        return index - 4;
    }
}

int ChessBoard::backLeftIndex(int index){
    if(index >= 46 || index % 10 == 6 || index == -1){
        return -1;
    }
    else if(((index - 1) / 5) % 2 == 1){
        return index + 4;
    }
    else{
        return index + 5;
    }
}

int ChessBoard::backRightIndex(int index){
    if(index >= 46 || index % 10 == 5 || index == -1){
        return -1;
    }
    else if(((index - 1) / 5) % 2 == 1){
        return index + 5;
    }
    else{
        return index + 6;
    }
}
