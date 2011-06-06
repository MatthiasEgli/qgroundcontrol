#ifndef MESHSTATUS_H
#define MESHSTATUS_H

#include <QWidget>
#include <QTimer>
#include "qttelnet.h"

namespace Ui {
    class MeshStatus;
}

class MeshStatus : public QWidget
{
    Q_OBJECT

public:
    explicit MeshStatus(QWidget *parent = 0);
    ~MeshStatus();

private:
    Ui::MeshStatus *ui;
    QtTelnet *telnetConnection;
    QTimer *autoUpdateTimer;

public slots:
    void update();

private slots:
    void telnetMessage(const QString &msg);
    void autoUpdate(int state);
};

#endif // MESHSTATUS_H
