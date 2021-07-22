#ifndef TCPSERVERMONITOR_H
#define TCPSERVERMONITOR_H

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNetwork/QTcpServer>

class TcpServerMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServerMonitor(QWidget *parent = nullptr);
    ~TcpServerMonitor();

signals:

private:
    QLabel *m_labelSocketStatus;
    QLabel *m_labelServerIP;
    QLabel *m_labelServerPort;
    QLabel *m_labelClientIP;
    QLabel *m_labelClientPort;
    QTcpServer *m_tcpServer;
};

#endif  // TCPSERVERMONITOR_H
