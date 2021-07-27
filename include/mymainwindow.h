#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QHBoxLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStackedWidget>
#include <QTreeWidget>

#include "tcpservermonitor.h"
#include "tcpserverwidget.h"
#include "tcpsocketmonitor.h"
#include "tcpsocketwidget.h"

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget *parent = nullptr);
    ~MyMainWindow();

private:
    QListWidget *m_listWidget;
    QStackedWidget *m_stackedWidget;

private slots:
    void on_actionTcpServer_triggered();
    void on_actionTcpClient_triggered();
    void on_actionUdpSocket_triggered();
    void on_actionUdpGroup_triggered();
    void on_actionDelete_triggered();
    void on_listWidget_currentItemChanged(QListWidgetItem *current);
    void on_tcpServerCreated(QTcpServer *tcpServer);
    void on_tcpSocketCreated(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                             quint16 port);
};
#endif  // MYMAINWINDOW_H
