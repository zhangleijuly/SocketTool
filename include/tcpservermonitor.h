#ifndef TCPSERVERMONITOR_H
#define TCPSERVERMONITOR_H

#include <QWidget>

#include "stdafx.h"

class TcpServerMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServerMonitor(QWidget *parent = nullptr);
    ~TcpServerMonitor();
    void setTcpServer(QTcpServer *tcpServer);

public slots:
    void on_tcpSocket_receiveData(QString info, QString data);

signals:
    void tcpSocketConnected(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                            quint16 port);

private:
    QLabel *m_labelServerStatus;
    QLabel *m_labelServerIP;
    QLabel *m_labelServerPort;
    QPushButton *m_pushButtonStart;
    QPushButton *m_pushButtonStop;
    QTextBrowser *m_textBrowserReceivedData;
    QTcpServer *m_tcpServer;
    QHostAddress m_hostAddress;
    quint16 m_port;

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_tcpServer_newConnection();
    void on_tcpServer_acceptError();
};

#endif  // TCPSERVERMONITOR_H
