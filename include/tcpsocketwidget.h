#ifndef TCPSOCKETWIDGET_H
#define TCPSOCKETWIDGET_H

#include <QWidget>

#include "stdafx.h"

class TcpSocketWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpSocketWidget(QWidget *parent = nullptr);

signals:
    void tcpSocketCreated(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                          quint16 port);

private:
    QLineEdit *m_lineEditPeerIp;
    QSpinBox *m_spinBoxPeerPort;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
};

#endif  // TCPSOCKETWIDGET_H
