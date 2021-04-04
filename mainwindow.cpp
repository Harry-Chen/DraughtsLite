#include "mainwindow.h"
#include "gui\createdialog.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QHostAddress>
#include <QStringRef>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/resources/icons/chess_material.ico"));

    board_widget = new BoardWidget(this);
    controller = new Controller(board_widget);
    this->setCentralWidget(board_widget);

    int menuHeight = menuBar()->height();
    int statusHeight = statusBar()->height();
    setFixedSize(410, 400 + menuHeight + statusHeight + 10);

    connect(ui->actionAbout, &QAction::triggered, [=](){
        QMessageBox::about(this, tr("关于 Draughts Lite"), tr("Copyright © Harry Chen 2017.\nAll Rights Reserved.\n感谢 miskcoo 在算法上提供的帮助"));
    });

    connect(ui->actionNewGame, &QAction::triggered, [=](){
        CreateDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            controller->runServer(dialog.getConfig());
        }
    });

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectClicked);

    connect(ui->actionDisconnect, &QAction::triggered, controller, &Controller::disconnectNetwork);

    connect(ui->actionSound, &QAction::toggled, controller, &Controller::setSoundEffectsEnabled);

    connect(ui->actionDraw, &QAction::triggered, [=](){
        if(!board_widget->playing) return;
        if(askForConfirmation(tr("确定向对手要求和吗？")) == true){
            board_widget->enableWaiting(tr("已求和，等待对手答复..."));
            controller->requestForDraw();
        }
    });

    connect(ui->actionResign, &QAction::triggered, [=](){
        if(!board_widget->playing) return;
        if(askForConfirmation(tr("确定要认输吗？")) == true){
            emit resigned(false);
        }
    });

    connect(board_widget, &BoardWidget::gameLost, [=](){
        emit resigned(true);
    });

    connect(this, &MainWindow::resigned, [=](bool passive){
        QString notify = passive ? tr("您输了，游戏结束") : tr("您主动认输，游戏结束");
        board_widget->enableWaiting(notify);
        controller->requestForResign(passive);
        board_widget->terminateGame(BoardWidget::LOSE);
    });

    connect(controller, &Controller::opponentResigned, [=](bool passive){
        QString notify = passive ? tr("您赢了，游戏结束") : tr("对手主动认输，游戏结束");
        board_widget->enableWaiting(notify);
        board_widget->terminateGame(BoardWidget::WIN);
    });

    connect(controller, &Controller::moveFromNetwork, board_widget, &BoardWidget::handleMoveFromNetwork);

    connect(controller, &Controller::drawAgreed, [=](){
        board_widget->disableWaiting();
        QString notify = tr("对手同意求和，游戏结束");
        board_widget->enableWaiting(notify);
        board_widget->terminateGame(BoardWidget::DRAW);
    });

    connect(controller, &Controller::drawDenied, [=](){
        board_widget->disableWaiting();
        QString notify = tr("对手不同意求和，游戏继续");
        QMessageBox::information(this, tr("提示"), notify, QMessageBox::Ok);
        statusBar()->showMessage(notify);
    });

    connect(controller, &Controller::handleOpponentDraw, [=](bool agreed){
        if(agreed){
            board_widget->enableWaiting(tr("您同意了对手的求和，游戏结束"));
            board_widget->terminateGame(BoardWidget::DRAW);
        }
        else{
            statusBar()->showMessage(tr("您拒绝了对手的求和，游戏继续"));
        }
    });

    // Qt saving type
    qRegisterMetaTypeStreamOperators<GameConfig>("GameConfig");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectClicked()
{
    bool ok;
    QString ip_address = QInputDialog::getText(this, tr("输入服务器信息"),
                                               tr("格式： IP:Port，如 127.0.0.1:9999，或[::1]:9999"), QLineEdit::Normal,
                                               "127.0.0.1:9999", &ok);
    if (ok) {
        QHostAddress IP;
        QString address;
        quint16 port;
        if(ip_address.contains('[')){ // handle IPv6
            auto start = ip_address.indexOf('[');
            auto end = ip_address.indexOf(']');
            address = QStringRef(&ip_address, start + 1, end - start - 1).toString();
            port = QStringRef(&ip_address, end + 2, ip_address.size() - end - 2).toString().toInt();
            qDebug() << address << port;
        }
        else{
            auto config = ip_address.split(":");
            address = config.at(0);
            port = config.at(1).toInt();
        }
        if (IP.setAddress(address) && port >= 1 && port <= 65535) // here we have correct IP
            controller->connectIP(IP, port);
        else
            statusBar()->showMessage(tr("IP 地址不正确"));
    } else {
        statusBar()->showMessage(tr("请求被取消"));
    }

}


bool MainWindow::askForConfirmation(const QString &content)
{
    auto result =  QMessageBox::question(this, tr("请求确认"), content, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return result == QMessageBox::Yes;
}
