#include <QJsonDocument>
#include <QHostInfo>
#include <QHostAddress>
#include <ctime>
#include <QMessageBox>

#include "controller.h"
#include "utilities/gameconfig.h"
#include "gui/soundplayer.h"

Controller::Controller(BoardWidget * _widget)
{
    network = new Network(this);

    board = nullptr;
    widget = _widget;

    player = new SoundPlayer();

    m_isConfigRecieved = false;

    connect(network, &Network::eventRecieved, this, &Controller::networkEvent);
    connect(network, &Network::networkConnected, this, &Controller::connected);
    connect(network, &Network::networkDisconnected, this, &Controller::disconnected);
    connect(network, &Network::networkError, this, &Controller::handleError);

    connect(widget, &BoardWidget::performMove, this, &Controller::sendMove);

    connect(this, &Controller::disconnectNetwork, network, &Network::disconnectNetwork);

    connect(widget, &BoardWidget::playSound, [=](SoundPlayer::Sounds sound){
        if(soundEffectsEnabled){
            player->play(sound);
        }
    });

    connect(this, &Controller::controllerError, widget, &BoardWidget::enableWaiting);
}

Controller::~Controller()
{
    if ( board ) { // if board has already been created
        widget->setBoard(nullptr); // safely remove
        board->disconnect(); // signal & slot
        delete board;
        board = nullptr;
    }
}

void Controller::runServer(const GameConfig &_config)
{
    network->run(Network::SERVER, _config.port);

    if (board) { // if board has already been created
        widget->setBoard(nullptr); // safely remove
        board->disconnect(); // signal & slot
        delete board;
        board = nullptr;
    }

    GameConfig config = _config;

    if(config.userColor == RANDOM){
        qsrand(time(NULL));
        if(qrand() % 2 == 0){
            config.userColor = BLACK;
        }
        else{
            config.userColor = WHITE;
        }
    }

    board = new ChessBoard(config);
    widget->setBoard(board);

    QString notify = tr("等待客户端连接...");

    auto info = QHostInfo::fromName(QHostInfo::localHostName());
    for(auto address : info.addresses()){
        notify = notify % tr("\nIP地址: ") % address.toString();
    }
    notify = notify % tr("\n端口: ") % QString::number(config.port);
    widget->enableWaiting(notify);

}

void Controller::connectIP(const QHostAddress &address, quint16 port)
{
    network->run(Network::CLIENT);
    network->connectTo(address, port);
    widget->enableWaiting(tr("正在连接到服务器..."));
}

void Controller::networkEvent(QByteArray data)
{
    auto jsonDocument = QJsonDocument::fromBinaryData(data);
    qDebug() << "Received: " << jsonDocument.toJson();

    if(jsonDocument.isEmpty()) return;
    auto objectsMap = jsonDocument.toVariant().toMap();

    auto operation = static_cast<OperationType>(objectsMap["operation"].toInt());
    switch(operation) {
    case INITIALIZE:{
        auto configString = objectsMap["config"].toString();
        auto block = QByteArray::fromBase64(configString.toLatin1());
        QDataStream in(&block, QIODevice::ReadOnly);
        GameConfig config;
        in >> config;
        if (board) {
            widget->setBoard(nullptr);
            board->disconnect();
            delete board;
            board = nullptr;
        }
        board = new ChessBoard(config);
        widget->setBoard(board);
        break;
    }
    case MOVE:{
        MovePath movePath;
        auto path = objectsMap["path"].toList();
        for(auto &_move : path){
            auto map = _move.toMap();
            Move move = {map["startIndex"].toInt(),
                         map["endIndex"].toInt(),
                         map["takeIndex"].toInt()};
            movePath.push_back(move);
        }
        emit moveFromNetwork(movePath);
        break;
    }
    case DRAWREQUEST:{
        auto action = objectsMap["action"].toString();
        if(action == "request"){
            auto result = QMessageBox::question(widget, tr("询问"), tr("对手请求和棋，是否同意？"),
                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            bool agreed = result == QMessageBox::Yes;
            QJsonDocument document;
            QJsonObject object;
            object.insert("operation", OperationType::DRAWREQUEST);
            object.insert("action", agreed ? "agreed" : "denied");
            document.setObject(object);
            network->sendData(document.toBinaryData());
            emit handleOpponentDraw(agreed);
        }
        else if(action == "agreed"){
            emit drawAgreed();
        }
        else if(action == "denied"){
            emit drawDenied();
        }

        break;
    }
    case RESIGN:{
        auto passive = objectsMap["passive"].toBool();
        emit opponentResigned(passive);
        break;
    }
    }
}

void Controller::disconnected()
{
    auto cfg = board->config;

    widget->setBoard(nullptr);
    board->disconnect();
    delete board;
    board = nullptr;

    widget->enableWaiting(tr("连接已断开"));

    if (network->netType == Network::SERVER) {
        runServer(cfg);
    }
}

void Controller::connected()
{
    if (network->netType == Network::SERVER) {
        // Change userColor parameter for opponent in config file
        // Swapping colors
        GameConfig cfg = board->config;
        if (cfg.userColor == WHITE)
            cfg.userColor = BLACK;
        else if(cfg.userColor == BLACK)
            cfg.userColor = WHITE;

        QJsonDocument document;
        QJsonObject object;
        object.insert("operation", OperationType::INITIALIZE);

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out << cfg;
        object.insert("config", QString(block.toBase64()));

        document.setObject(object);
        network->sendData(document.toBinaryData());
        widget->disableWaiting();
    }
    if (network->netType == Network::CLIENT) {
        widget->disableWaiting();
    }
}

void Controller::handleError(const QString &str)
{
    emit controllerError(tr("Socket 连接发生异常：\n") % str);
}

void Controller::requestForDraw()
{
    QJsonDocument document;
    QJsonObject object;
    object.insert("operation", OperationType::DRAWREQUEST);
    object.insert("action", "request");
    document.setObject(object);
    network->sendData(document.toBinaryData());
}

void Controller::requestForResign(bool passive)
{
    QJsonDocument document;
    QJsonObject object;
    object.insert("operation", OperationType::RESIGN);
    object.insert("passive", passive);
    document.setObject(object);
    network->sendData(document.toBinaryData());
}

void Controller::setSoundEffectsEnabled(bool enabled)
{
    soundEffectsEnabled = enabled;
}

void Controller::sendMove(MovePath path)
{
    QJsonDocument document;
    QJsonObject action;
    action.insert("operation", OperationType::MOVE);
    QJsonArray array;
    int i = 0;
    for(auto &move : path){
        QJsonObject object;
        object.insert("startIndex", move.startIndex);
        object.insert("endIndex", move.endIndex);
        object.insert("takeIndex", move.takeIndex);
        array.insert(i, object);
        ++i;
    }
    action.insert("path", array);
    document.setObject(action);
    qDebug() << "Move to send: " << document.toJson();
    network->sendData(document.toBinaryData());
}
