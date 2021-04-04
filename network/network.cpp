#include "network.h"

Network::Network(QObject * parent) : QObject(parent)
{
    netType = EMPTY;
    m_tcpserver = new QTcpServer(this);
    m_tcpsocket = nullptr;
    m_isConnected = false;

    connect(this, &Network::disconnectNetwork, &Network::m_resetConnections);
}

void Network::sendData(QByteArray rawData)
{
    rawData = qCompress(rawData);
    if (m_isConnected) {
        QByteArray block;
        QDataStream sendStream(&block, QIODevice::ReadWrite);

        sendStream << quint16(0) << rawData;
        // Size of data
        sendStream.device()->seek(0);
        sendStream << (quint16)(block.size() - sizeof(quint16));

        m_tcpsocket->write(block);
    }
}

void Network::run(NetType _netType, quint16 port)
{
    m_resetConnections();

    netType = _netType;
    if (netType == SERVER) {
        if (m_tcpserver->listen(QHostAddress::Any, port) == false) return;
        connect(m_tcpserver, SIGNAL(newConnection()), this, SLOT(connected()), Qt::UniqueConnection);
    }
    if (netType == CLIENT) {
        if (m_tcpsocket == nullptr)
            m_tcpsocket = new QTcpSocket(this);
        connect(m_tcpsocket, SIGNAL(connected()), this, SLOT(connected()), Qt::UniqueConnection);
        connect(m_tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(handleSocketError(QAbstractSocket::SocketError)), Qt::UniqueConnection);
    }
}

void Network::disconnected()
{
    m_resetConnections();
    emit networkDisconnected();
}

void Network::connected()
{
    if (netType == EMPTY) return;
    if (netType == SERVER) {
        if (m_tcpsocket) {
            m_tcpsocket->disconnect(); // signal & slot disconnect
            m_tcpsocket->close();
            m_tcpsocket->deleteLater();
        }
        m_tcpsocket = m_tcpserver->nextPendingConnection();
        connect(m_tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(handleSocketError(QAbstractSocket::SocketError)), Qt::UniqueConnection);
    }

    connect(m_tcpsocket, SIGNAL(readyRead()), this, SLOT(recieveEvent()), Qt::UniqueConnection);
    connect(m_tcpsocket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::UniqueConnection);

    m_isConnected = true;
    emit networkConnected();
}

void Network::connectTo(QHostAddress address, quint16 port)
{
    if (m_tcpsocket == nullptr || m_isConnected || netType != CLIENT)
    {
        m_resetConnections();
        run(CLIENT, port);
    }
    m_tcpsocket->connectToHost(address, port);
}

void Network::recieveEvent()
{
    QDataStream receiveStream(m_tcpsocket);
    quint16     blockSize;
    QByteArray  data;

    receiveStream >> blockSize;
    if (m_tcpsocket->bytesAvailable() < blockSize) { // Server returned incorrect data
        emit networkError(tr(u8"Network::recieveEvent(): error on data exchange"));
        return;
    }

    receiveStream >> data;
    data = qUncompress(data);

    emit eventRecieved(data);
}

void Network::handleSocketError(QAbstractSocket::SocketError)
{
    QString str = m_tcpsocket->errorString();
    m_resetConnections();
    emit networkError(str);
}

void Network::m_resetConnections()
{
    if (m_tcpsocket != nullptr) {
        if (m_tcpsocket->state() == QTcpSocket::ConnectedState ||
            m_tcpsocket->state() == QTcpSocket::ConnectingState)
        {
            m_tcpsocket->close();
        }
        m_tcpsocket->disconnect();
        m_tcpsocket->deleteLater();
        m_tcpsocket = nullptr;
    }
    

    if (m_tcpserver->isListening())
    {
        m_tcpserver->close();
    }

    m_isConnected = false;
}
