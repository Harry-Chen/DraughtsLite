#ifndef CREATEDIALOG_HPP
#define CREATEDIALOG_HPP
#include <QDialog>

#include "utilities/gameconfig.h"
#include "ui_createdialog.h"

class CreateDialog : public QDialog {
	Q_OBJECT

public:
	CreateDialog(QWidget * parent = Q_NULLPTR);
	~CreateDialog();

    GameConfig getConfig();

public slots:
    bool onOkay();
    void onRejected();
    void onSideChosen();

private:
	Ui::CreateDialog ui;
    GameConfig config;
    void showRejectReason(int row, int column);
};

#endif // CREATEDIALOG_HPP
