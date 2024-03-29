#ifndef TCPSERVERWIDGET_H
#define TCPSERVERWIDGET_H

#include <QWidget>

#include "stdafx.h"

class TcpServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServerWidget(QWidget *parent = nullptr);

signals:
    void tcpServerCreated(QTcpServer *tcpServer);

private:
    QComboBox *m_comboBoxIP;
    QSpinBox *m_spinBoxPort;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
};

#endif  // TCPSERVERWIDGET_H
