#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include "gui/boardwidget.h"
#include "logic/chessboard.h"
#include "logic/controller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
   void connectClicked();

signals:
   void resigned(bool passive);

private:
    Ui::MainWindow *ui;
    BoardWidget *board_widget;
    Controller  *controller;
    bool askForConfirmation(const QString &content);
};

#endif // MAINWINDOW_H
