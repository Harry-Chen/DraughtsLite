/*******************************************************************************
* Copyright (c) 2016-2017, Comrade Andrew A.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "network/network.h"
#include "utilities/gameconfig.h"

#include "logic/chessboard.h"
#include "gui/boardwidget.h"
#include "logic/move.h"

class Controller : public QObject {
    Q_OBJECT

    Network *network;
    ChessBoard  *board;
    BoardWidget *widget;
    SoundPlayer *player;
    bool soundEffectsEnabled = false;

public:
    Controller(BoardWidget *widget);
    ~Controller();

    void runServer(const GameConfig&);
    void connectIP(const QHostAddress& address, quint16 port);

public slots:
    void networkEvent(QByteArray data);
    void disconnected();
    void connected();

    void handleError(const QString &);
    void requestForDraw();
    void requestForResign(bool passive);
    void setSoundEffectsEnabled(bool enabled);

signals:
    void controllerError(const QString &); // #TODO: output error somewhere
    void moveFromNetwork(MovePath path);
    void opponentResigned(bool passive);
    void handleOpponentDraw(bool agreed);
    void drawAgreed();
    void drawDenied();
    void disconnectNetwork();
    void showStatusMessage(const QString&);

private:
    // flag to receive GameConfig on connect
    bool m_isConfigRecieved;

private slots:
    void sendMove(MovePath path);
};

#endif//CONTROLLER_H
