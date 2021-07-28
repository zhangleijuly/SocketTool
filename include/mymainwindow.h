#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>

#include "stdafx.h"

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget *parent = nullptr);
    ~MyMainWindow();

private:
    QTreeWidget *m_treeWidget;
    QStackedWidget *m_stackedWidget;

private slots:
    void on_actionTcpServer_triggered();
    void on_actionTcpClient_triggered();
    void on_actionUdpSocket_triggered();
    void on_actionUdpGroup_triggered();
    void on_actionDelete_triggered();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current);
    void on_tcpServerCreated(QTcpServer *tcpServer);
    void on_tcpSocketCreated(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                             quint16 port);
    void on_tcpSocketConnected(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                               quint16 port);
    void on_udpSocketCreated(QUdpSocket *udpSocket);
};
#endif  // MYMAINWINDOW_H
