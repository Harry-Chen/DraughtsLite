#include "createdialog.h"

#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

CreateDialog::CreateDialog(QWidget * parent) : QDialog(parent) {
    ui.setupUi(this);

    //connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    //connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(onRejected()));

    connect(ui.buttonBox, QOverload<QAbstractButton *>::of(&QDialogButtonBox::clicked),
            [=](QAbstractButton *button){
        switch(ui.buttonBox->buttonRole(button)){
        case QDialogButtonBox::AcceptRole:
            if(onOkay()){
                accept();
            }
            break;
        case QDialogButtonBox::RejectRole:
            reject();
            break;
        }
    });

    // set default side to random
    config.userColor = RANDOM;
    ui.randomKingButton->setChecked(true);
    connect(ui.blackKingButton,  SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));
    connect(ui.randomKingButton, SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));
    connect(ui.whiteKingButton,  SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));

}

CreateDialog::~CreateDialog() {

}

GameConfig CreateDialog::getConfig()
{
    config.port = static_cast<quint16>(ui.spinPort->value());
    if(ui.comboFirstPlayer->currentIndex() == 0){
        config.firstColor = eColor::BLACK;
    }
    else{
        config.firstColor = eColor::WHITE;
    }
    return config;
}

bool CreateDialog::onOkay()
{
    auto text = ui.textGame->document()->toPlainText().simplified().replace(' ', "");
    if(text.isEmpty()){
        for(auto i = 1;i<=20;++i){
            config.cells[i] = ChessCell(eColor::BLACK);
        }
        for(auto i = 31;i<=50;++i){
            config.cells[i] = ChessCell(eColor::WHITE);
        }
        return true;
    }
    else if(text.length() != 100){
        QMessageBox::warning(this, tr("错误"), tr("字符长度不足100"), QMessageBox::Ok);
        return false;
    }
    else{
        for(auto i=0;i<100;++i){
            auto c = text.at(i).toLatin1();
            auto row = i / 10 + 1;
            auto column = (i + 1) % 10;
            if(column == 0) column = 10;
            auto index = -1;
            if(row % 2 == 1 && column % 2 == 0){
                index = (row - 1) * 5 + column / 2;
            }
            else if(row % 2 == 0 && column % 2 == 1){
                index = (row - 1) * 5 + (column + 1) / 2;
            }
            if((row % 2) == (column % 2)) {
                if(c != '0'){
                    showRejectReason(row, column);
                    return false;
                }
            }
            else{
                switch(c){
                case 'w':
                    config.cells[index] = ChessCell(eColor::WHITE, ChessCellType::NORMAL);
                    break;
                case 'W':
                    config.cells[index] = ChessCell(eColor::WHITE, ChessCellType::KING);
                    break;
                case 'b':
                    config.cells[index] = ChessCell(eColor::BLACK, ChessCellType::NORMAL);
                    break;
                case 'B':
                    config.cells[index] = ChessCell(eColor::BLACK, ChessCellType::KING);
                    break;
                case '0':
                    break;
                default:
                    showRejectReason(row, column);
                    return false;
                }
            }
        }
    }
    return true;
}

void CreateDialog::onRejected()
{
    //reject();
}

void CreateDialog::onSideChosen()
{
    bool isAllUnchecked = !(ui.blackKingButton->isChecked()  ||
                            ui.randomKingButton->isChecked() ||
                            ui.whiteKingButton->isChecked()  );

    if (sender() == ui.blackKingButton) {
        if (isAllUnchecked) ui.blackKingButton->setChecked(true);
        ui.randomKingButton->setChecked(false);
        ui.whiteKingButton->setChecked(false);
        config.userColor = BLACK;
    }
    if (sender() == ui.randomKingButton) {
        if (isAllUnchecked) ui.randomKingButton->setChecked(true);
        ui.blackKingButton->setChecked(false);
        ui.whiteKingButton->setChecked(false);
        config.userColor = RANDOM;
    }
    if (sender() == ui.whiteKingButton) {
        if (isAllUnchecked) ui.whiteKingButton->setChecked(true);
        ui.blackKingButton->setChecked(false);
        ui.randomKingButton->setChecked(false);
        config.userColor = WHITE;
    }
}

void CreateDialog::showRejectReason(int row, int column)
{
    QString format = tr("第%1行第%2列字符格式错误！");
    QMessageBox::warning(this, tr("错误"), format.arg(row).arg(column), QMessageBox::Ok);
}
