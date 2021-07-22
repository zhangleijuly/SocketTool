#include "tcpservermonitor.h"

TcpServerMonitor::TcpServerMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpServer = nullptr;

    QGroupBox *groupBoxSocketStatus = new QGroupBox("Socket Status");

    QGridLayout *layoutSocketStatus = new QGridLayout;

    QPushButton *pushButtonStart = new QPushButton;
    QPushButton *pushButtonStop = new QPushButton;

    m_labelSocketStatus = new QLabel;
    m_labelServerIP = new QLabel;
    m_labelServerPort = new QLabel;
    m_labelClientIP = new QLabel;
    m_labelClientPort = new QLabel;

    layoutSocketStatus->addWidget(m_labelSocketStatus, 0, 0);
    layoutSocketStatus->addWidget(m_labelClientIP, 0, 2);
    layoutSocketStatus->addWidget(m_labelClientPort, 0, 3);
    layoutSocketStatus->addWidget(pushButtonStart, 1, 0);
    layoutSocketStatus->addWidget(pushButtonStop, 1, 1);
    layoutSocketStatus->addWidget(m_labelServerIP, 1, 2);
    layoutSocketStatus->addWidget(m_labelServerPort, 1, 3);

    groupBoxSocketStatus->setLayout(layoutSocketStatus);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxSocketStatus);

    setLayout(vBoxLayout);
}

TcpServerMonitor::~TcpServerMonitor()
{
    if (m_tcpServer != nullptr)
    {
        m_tcpServer->close();
        m_tcpServer->deleteLater();
    }
}

void TcpServerMonitor::setTcpServer(QTcpServer *tcpServer) {}
