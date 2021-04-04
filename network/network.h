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
#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork>

class Network : public QObject {
    Q_OBJECT
public:
    enum NetType {
        EMPTY,
        SERVER,
        CLIENT
    } netType;
    
    explicit Network(QObject *parent = 0);

public slots:
    // Slots for Controller
    // Common slots
    void sendData(QByteArray);
    void run(NetType type, quint16 port = 0);
    void disconnected();
    
    // Server slots
    void connected();

    // Client slots
    void connectTo(QHostAddress address, quint16 port);
    
    // socket slots
    void recieveEvent();

    // error slot
    void handleSocketError(QAbstractSocket::SocketError);

signals:
    void networkConnected();
    void networkDisconnected();
    void eventRecieved(QByteArray);

    void networkError(const QString &);
    void disconnectNetwork();

private:
    // reset all connections
    void m_resetConnections();

    // Server variables
    QTcpServer *m_tcpserver;

    // Common variables
    QTcpSocket *m_tcpsocket;
    bool        m_isConnected;
};


#endif//NETWORK_H
