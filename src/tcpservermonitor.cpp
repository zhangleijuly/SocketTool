#include "tcpservermonitor.h"

TcpServerMonitor::TcpServerMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpServer = nullptr;

    // Server status
    QGroupBox *groupBoxServerStatus = new QGroupBox(tr("Server Status"));

    m_pushButtonStart = new QPushButton(tr("Start listening"));
    m_pushButtonStop = new QPushButton(tr("Stop listening"));
    connect(m_pushButtonStart, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStart_clicked);
    connect(m_pushButtonStop, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStop_clicked);

    m_labelServerStatus = new QLabel;
    m_labelServerIP = new QLabel;
    m_labelServerPort = new QLabel;

    QVBoxLayout *layoutSocketStatus = new QVBoxLayout;
    QHBoxLayout *layoutSocketStatusR1 = new QHBoxLayout;
    QHBoxLayout *layoutSocketStatusR2 = new QHBoxLayout;

    layoutSocketStatusR1->addWidget(m_labelServerStatus);
    m_labelServerStatus->setAlignment(Qt::AlignHCenter);
    m_labelServerStatus->setMinimumWidth(200);
    layoutSocketStatusR1->addStretch();

    layoutSocketStatusR2->addWidget(m_pushButtonStart);
    m_pushButtonStart->setMinimumWidth(200);
    layoutSocketStatusR2->addSpacing(5);
    layoutSocketStatusR2->addWidget(m_pushButtonStop);
    m_pushButtonStop->setMinimumWidth(200);
    layoutSocketStatusR2->addStretch();
    layoutSocketStatusR2->addWidget(m_labelServerIP);
    layoutSocketStatusR2->addSpacing(5);
    layoutSocketStatusR2->addWidget(m_labelServerPort);

    layoutSocketStatus->addLayout(layoutSocketStatusR1);
    layoutSocketStatus->addLayout(layoutSocketStatusR2);

    groupBoxServerStatus->setLayout(layoutSocketStatus);

    // Receive data
    QGroupBox *groupBoxReceiveData =
        new QGroupBox(tr("Received data and notification"));

    m_textBrowserReceivedData = new QTextBrowser;

    groupBoxReceiveData->setLayout(new QVBoxLayout);
    groupBoxReceiveData->layout()->addWidget(m_textBrowserReceivedData);

    // Layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxServerStatus);
    vBoxLayout->addWidget(groupBoxReceiveData);

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

void TcpServerMonitor::setTcpServer(QTcpServer *tcpServer)
{
    Q_ASSERT(m_tcpServer == nullptr);
    if (tcpServer == nullptr)
    {
        return;
    }

    m_tcpServer = tcpServer;
    m_hostAddress = m_tcpServer->serverAddress();
    m_port = m_tcpServer->serverPort();

    connect(m_tcpServer, &QTcpServer::newConnection, this,
            &TcpServerMonitor::on_tcpServer_newConnection);
    connect(m_tcpServer, &QTcpServer::acceptError, this,
            &TcpServerMonitor::on_tcpServer_acceptError);
    m_labelServerIP->setText(tr("Server IP: %1").arg(m_hostAddress.toString()));
    m_labelServerPort->setText(
        tr("Server port: %1").arg(QString::number(m_port)));

    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelServerStatus->setText(tr("Listening"));
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelServerStatus->setText(tr("Stop"));
    }
}

void TcpServerMonitor::on_pushButtonStart_clicked()
{
    if (!m_tcpServer->listen(m_hostAddress, m_port))
    {
        QString estr =
            tr("Start TCP server failed! %1").arg(m_tcpServer->errorString());
        QMessageBox::critical(this, tr("Error"), estr);
    }
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelServerStatus->setText(tr("Listening"));
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelServerStatus->setText(tr("Stop"));
    }
}

void TcpServerMonitor::on_pushButtonStop_clicked()
{
    m_tcpServer->close();
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelServerStatus->setText(tr("Listening"));
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelServerStatus->setText(tr("Stop"));
    }
}

void TcpServerMonitor::on_tcpServer_newConnection()
{
    QTcpSocket *tcpSocket = m_tcpServer->nextPendingConnection();
    emit tcpSocketConnected(tcpSocket, tcpSocket->peerAddress(),
                            tcpSocket->peerPort());
}

void TcpServerMonitor::on_tcpServer_acceptError()
{
    QString msg =
        tr("%1 Error: %2")
            .arg(QTime::currentTime().toString(), m_tcpServer->errorString());
    m_textBrowserReceivedData->append(msg);
}

void TcpServerMonitor::on_tcpSocket_receiveData(QString info, QString data)
{
    m_textBrowserReceivedData->append(info);
    m_textBrowserReceivedData->append(data);
}
