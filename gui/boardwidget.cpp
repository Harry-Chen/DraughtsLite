#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <QtEvents>
#include <QImage>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>

#include <algorithm>

#include "boardwidget.h"
#include "utilities/gameconfig.h"

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent)
{
    p = new QPainter();
    wait = new WaitingWidget(this);
    wait->setGeometry(5, 5, 400, 400);
    wait->enableWidget("游戏尚未开始");
    connect(this, &BoardWidget::performMove, [=](MovePath path){
        this->deselectAll();
        this->moveOnBoard(path);
        playing = false;
    });
}

void BoardWidget::setBoard(ChessBoard *_board)
{
    if(_board == nullptr){
        for(auto i = 1;i <= 50;++i){
            delete board->getCell(i)->label;
        }
        board = nullptr;
        return;
    }
    board = _board;

    rotation = board->config.userColor == BLACK;

    for(auto i = 1;i <= 50;++i){
        auto *cell = board->getCell(i);
        auto position = getPositionForCellIndex(i);
        position.translate(DRAW_OFFSET);
        auto *label = new QLabel(this);
        label->setGeometry(position);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if(cell->type != EMPTY){
            label->setPixmap(getPixmapForCell(*cell));
        }else{
        }
        cell->label = label;
        label->show();
    }
    connect(this, &BoardWidget::opponentMoveDone, board, &ChessBoard::recalculateMoves);
    connect(board, &ChessBoard::calculateMovesDone, this, &BoardWidget::updateData);

    if(board->config.userColor == board->config.firstColor){
        emit opponentMoveDone();
        playing = true;
    }
}

void BoardWidget::paintEvent(QPaintEvent *p)
{
    Q_UNUSED(p);
    drawBackground();
    this->p->begin(this);
    this->p->translate(DRAW_OFFSET);
    this->p->setPen(QColor(0, 0, 255));
    if(inSelectionMode){
        for(auto &path : nowPossibleMoves){
            for(auto &move : path){
                this->p->drawLine(getPositionForCellIndex(move.startIndex).center(), getPositionForCellIndex(move.endIndex).center());
            }
        }
    }
    this->p->end();
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    if(!playing) return;
    auto pos = event->pos();
    int index = getCellIndexByPosition(pos);
    if(index == -1){
        return;
    }
    if(inSelectionMode){
        for(auto &move : nowPossibleMoves){
            if(move.last().endIndex == index){
                emit(performMove(move));
                playing = false;
                return;
            }
        }
    }

    if(board->getCell(index)->type == EMPTY && inSelectionMode){
        deselectAll();
        drawSelectableCells();
    }

    else if(selectableIndecies.contains(index)){
        select(index);
    }

}

void BoardWidget::enableWaiting(const QString &text)
{
    wait->raise();
    wait->enableWidget(text);
    playing = false;
}

void BoardWidget::disableWaiting()
{
    wait->disableWidget();
    playing = true;
}

void BoardWidget::handleMoveFromNetwork(MovePath path)
{
    moveOnBoard(path);
    emit opponentMoveDone();
}

void BoardWidget::drawBackground()
{
    p->begin(this);
    p->translate(DRAW_OFFSET);
    QImage blackTile = QImage(":/resources/dark_oak.jpg").scaled(CELL_WIDTH, CELL_WIDTH);
    QImage whiteTile = QImage(":/resources/white_oak.jpg").scaled(CELL_WIDTH, CELL_WIDTH);
    for(int i=0;i<10;++i){
        for(int j=0;j<10;++j){
            auto rect = getRectForCell(i, j);
            if((i + j) % 2 == 0){
                p->drawImage(rect.topLeft(), whiteTile);
            }
            else{
                p->drawImage(rect.topLeft(), blackTile);
            }
        }
    }
    p->end();
}

void BoardWidget::deselectAll()
{
    update();
    if(board == nullptr) return;
    for(int i=1;i<=50;++i){
        auto *cell = board->getCell(i);
        cell->label->setStyleSheet("");
    }
    inSelectionMode = false;
    nowSelectedIndex = -1;
    nowPossibleMoves.clear();
}

void BoardWidget::select(int index)
{
    deselectAll();
    drawSelectableCells();
    auto *cell = board->getCell(index);
    cell->label->setStyleSheet("border-width: 1px; border-style: solid; border-color: rgb(0, 255, 0);");
    nowSelectedIndex = index;
    inSelectionMode = true;
    nowPossibleMoves = board->getPossibleMoveForCell(index);
    for(auto &path : nowPossibleMoves){
        qDebug() << "Possible Move End: " << path.at(0).endIndex;
        board->getCell(path.last().endIndex)->label->setStyleSheet("border-width: 1px; border-style: solid; border-color: rgb(0, 255, 0);");
    }
    update();
}

QRect BoardWidget::getRectForCell(int row, int column)
{
    return QRect(QPoint(column * CELL_WIDTH, row * CELL_WIDTH), QSize(CELL_WIDTH, CELL_WIDTH));
}

QRect BoardWidget::getPositionForCellIndex(int index)
{
    if(rotation){
        index = 51 - index;
    }
    int row = (index + 4) / 5;
    int rawColumn = index % 5 == 0 ? 5 : index % 5;
    int column = rawColumn * 2 - 1;
    if(row & 1) column++;

    return getRectForCell(row - 1, column - 1);
}

int BoardWidget::getCellIndexByPosition(QPoint position)
{
    int x = position.x() - 5;
    int y = position.y() - 5;
    int column = x / 40 + 1;
    int row = y / 40 + 1;
    int index = -1;
    if(row % 2 == 1 && column % 2 == 0){
        index = (row - 1) * 5 + column / 2;
    }
    else if(row % 2 == 0 && column % 2 == 1){
        index = (row - 1) * 5 + (column + 1) / 2;
    }
    if(index != -1 && rotation){
        index = 51 - index;
    }
    return index;
}

void BoardWidget::drawSelectableCells()
{
    for(int index : selectableIndecies){
        board->getCell(index)->label->setStyleSheet("border-width: 1px; border-style: solid; border-color: rgb(255, 255, 0);");
    }
}

void BoardWidget::moveOnBoard(MovePath path)
{
    deselectAll();
    QSequentialAnimationGroup *groupMove = new QSequentialAnimationGroup();
    QSequentialAnimationGroup *groupEat = new QSequentialAnimationGroup();

    for(auto iter = path.begin(); iter != path.end() ; ++iter){
        auto move = *iter;
        auto *startCell = board->getCell(move.startIndex);
        auto *endCell = board->getCell(move.endIndex);
        SoundPlayer::Sounds soundToPlay;

        QPropertyAnimation *animationChess = new QPropertyAnimation(startCell->label, "geometry");
        QPropertyAnimation *animationEat = nullptr;
        animationChess->setDuration(500);
        animationChess->setStartValue(getPositionForCellIndex(move.startIndex) + RECT_MARGIN);
        animationChess->setEndValue(getPositionForCellIndex(move.endIndex) + RECT_MARGIN);

        if(move.takeIndex == -1){
            switch(board->config.userColor){
            case eColor::WHITE:
                soundToPlay = SoundPlayer::MOVE_WHITE;
                break;
            case eColor::BLACK:
                soundToPlay = SoundPlayer::MOVE_BLACK;
                break;
            case eColor::RANDOM:
                break; //WTF?
            }
        }
        else{
            auto eatCell = board->getCell(move.takeIndex);
            eatCell->type = EMPTY;
            soundToPlay = SoundPlayer::BREAK_CELL;
            animationChess->setKeyValueAt(0.5, getPositionForCellIndex(move.takeIndex) + RECT_MARGIN);

            QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
            eatCell->label->setGraphicsEffect(eff);
            animationEat = new QPropertyAnimation(eff, "opacity");
            animationEat->setStartValue(1);
            animationEat->setEndValue(0);
            animationEat->setDuration(500);
            animationEat->setKeyValueAt(0.5, 0.9);

            connect(animationEat, &QPropertyAnimation::finished, [=](){
                eff->setOpacity(1);
                eatCell->label->clear();
            });
        }

        groupMove->addAnimation(animationChess);
        if(animationEat != nullptr){
            groupEat->addAnimation(animationEat);
        }

        endCell->label->setGeometry(getPositionForCellIndex(move.startIndex) + RECT_MARGIN);

        std::swap(startCell->type, endCell->type);
        std::swap(startCell->color, endCell->color);
        std::swap(startCell->label, endCell->label);

        emit playSound(soundToPlay);

    }

    // king detection
    auto lastMove = path.last();
    auto lastReachedCell = board->getCell(lastMove.endIndex); // already swapped above
    if(lastReachedCell->type != ChessCellType::KING){
        if((lastMove.endIndex <= 5 && lastReachedCell->color == eColor::WHITE)
                || (lastMove.endIndex >= 46 && lastReachedCell->color == eColor::BLACK)){

            lastReachedCell->type = ChessCellType::KING;

            QPropertyAnimation *promoteToKingAnimation = new QPropertyAnimation();
            QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
            lastReachedCell->label->setGraphicsEffect(eff);
            promoteToKingAnimation = new QPropertyAnimation(eff, "opacity");
            promoteToKingAnimation->setStartValue(1);
            promoteToKingAnimation->setEndValue(1);
            promoteToKingAnimation->setDuration(300);

            groupMove->addAnimation(promoteToKingAnimation);
            connect(promoteToKingAnimation, &QPropertyAnimation::stateChanged,
                    [=](QAbstractAnimation::State state){
                if(state == QAbstractAnimation::Running){
                    lastReachedCell->label->setPixmap(getPixmapForCell(*lastReachedCell));
                }
            });
        }
    }

    groupMove->start(QAbstractAnimation::DeleteWhenStopped);
    groupEat->start(QAbstractAnimation::DeleteWhenStopped);

}

void BoardWidget::updateData()
{
    selectableIndecies = board->getSelectableIndecies();
    if(selectableIndecies.size() == 0){
        emit gameLost();
    }
    drawSelectableCells();
    playing = true;
}

const QPixmap &BoardWidget::getPixmapForCell(const ChessCell &cell)
{
    if(cell.color == eColor::WHITE){
        if(cell.type == ChessCellType::NORMAL){
            return WHITE_NORMAL;
        }
        else{
            return WHITE_KING;
        }
    }
    else if(cell.color == eColor::BLACK){
        if(cell.type == ChessCellType::NORMAL){
            return BLACK_NORMAL;
        }
        else{
            return BLACK_KING;
        }
    }
}

void BoardWidget::terminateGame(TerminateType type)
{
    //deselectAll();
    playing = false;
    SoundPlayer::Sounds sound;
    switch(type){
    case WIN:
        sound = SoundPlayer::WIN_GAME;
        break;
    case LOSE:
        sound = SoundPlayer::LOSE_GAME;
        break;
    case DRAW:
        sound = SoundPlayer::DRAW_GAME;
        break;
    }
    emit playSound(sound);
}
