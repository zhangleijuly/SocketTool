#include "tcpservermonitor.h"

TcpServerMonitor::TcpServerMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpServer = nullptr;

    // Server status
    QGroupBox *groupBoxSocketStatus = new QGroupBox("Server Status");

    QGridLayout *layoutSocketStatus = new QGridLayout;

    m_pushButtonStart = new QPushButton("Start listening");
    m_pushButtonStop = new QPushButton("Stop listening");
    connect(m_pushButtonStart, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStart_clicked);
    connect(m_pushButtonStop, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStop_clicked);

    m_labelSocketStatus = new QLabel;
    m_labelServerIP = new QLabel;
    m_labelServerPort = new QLabel;
    m_labelClientIP = new QLabel;
    m_labelClientPort = new QLabel;

    layoutSocketStatus->addWidget(m_labelSocketStatus, 0, 0);
    layoutSocketStatus->addWidget(m_labelClientIP, 0, 2);
    layoutSocketStatus->addWidget(m_labelClientPort, 0, 3);
    layoutSocketStatus->addWidget(m_pushButtonStart, 1, 0);
    layoutSocketStatus->addWidget(m_pushButtonStop, 1, 1);
    layoutSocketStatus->addWidget(m_labelServerIP, 1, 2);
    layoutSocketStatus->addWidget(m_labelServerPort, 1, 3);

    groupBoxSocketStatus->setLayout(layoutSocketStatus);

    // Receive data
    QGroupBox *groupBoxReceiveData =
        new QGroupBox("Received data and notification");

    m_textBrowserReceivedData = new QTextBrowser;

    groupBoxReceiveData->setLayout(new QVBoxLayout);
    groupBoxReceiveData->layout()->addWidget(m_textBrowserReceivedData);

    // Send data
    QGroupBox *groupBoxSendData = new QGroupBox("Send data");

    m_textEditSendData = new QTextEdit;

    groupBoxSendData->setLayout(new QVBoxLayout);
    groupBoxSendData->layout()->addWidget(m_textEditSendData);

    // Send data options
    QGroupBox *groupBoxSendOptions = new QGroupBox("Send options");

    m_comboBoxTcpSocket = new QComboBox;
    m_comboBoxNumberSend = new QComboBox;
    m_lineEditInterval = new QLineEdit("100");
    QPushButton *pushButtonSend = new QPushButton("Send data");
    QPushButton *pushButtonSendPeriod = new QPushButton("Cycle sending");
    QPushButton *pushButtonStopSend = new QPushButton("Stop sending");
    connect(pushButtonSend, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonSend_clicked);
    connect(pushButtonSendPeriod, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonSendPeriod_clicked);
    connect(pushButtonStopSend, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStopSend_clicked);

    for (int i = 1; i < 5; ++i)
    {
        int temp = pow(10, i);
        m_comboBoxNumberSend->addItem(QString::number(temp), temp);
    }

    QGridLayout *gridLayoutSendOptions = new QGridLayout;

    gridLayoutSendOptions->addWidget(new QLabel("Destination"), 0, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(new QLabel("Send times"), 1, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(new QLabel("Send interval(ms)"), 2, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(m_comboBoxTcpSocket, 0, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(m_comboBoxNumberSend, 1, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(m_lineEditInterval, 2, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(pushButtonSend, 0, 2);
    gridLayoutSendOptions->addWidget(pushButtonSendPeriod, 1, 2);
    gridLayoutSendOptions->addWidget(pushButtonStopSend, 2, 2);

    groupBoxSendOptions->setLayout(gridLayoutSendOptions);

    // Layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxSocketStatus);
    vBoxLayout->addWidget(groupBoxReceiveData);
    vBoxLayout->addWidget(groupBoxSendData);
    vBoxLayout->addWidget(groupBoxSendOptions);

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
    // connect(m_tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)),
    // this, SLOT(acceptSocketError(QAbstractSocket::SocketError)));
    m_labelServerIP->setText(
        QString("Server IP: %1").arg(m_hostAddress.toString()));
    m_labelServerPort->setText(
        QString("Server port: %1").arg(QString::number(m_port)));

    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonStart_clicked()
{
    if (!m_tcpServer->listen(m_hostAddress, m_port))
    {
        QString estr = QString("Start TCP server failed! %1")
                           .arg(m_tcpServer->errorString());
        QMessageBox::critical(this, tr("Error"), estr);
    }
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonStop_clicked()
{
    m_tcpServer->close();
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonSend_clicked()
{
    if (m_comboBoxTcpSocket->currentIndex() == -1)
    {
        return;
    }
    else
    {
        QString dest = m_comboBoxTcpSocket->currentText();
        QTcpSocket *tcpSocket =
            m_comboBoxTcpSocket->currentData().value<QTcpSocket *>();
        Q_ASSERT(tcpSocket != nullptr);
        QString data = m_textEditSendData->toPlainText();
        tcpSocket->write(data.toLocal8Bit());
        m_lock.lock();
        m_textBrowserReceivedData->append(
            QString("Send data to %1:").arg(dest));
        m_textBrowserReceivedData->append(data);
        m_lock.unlock();
    }
}

void TcpServerMonitor::on_pushButtonSendPeriod_clicked() {}

void TcpServerMonitor::on_pushButtonStopSend_clicked() {}

void TcpServerMonitor::on_tcpServer_newConnection()
{
    QTcpSocket *tcpSocket = m_tcpServer->nextPendingConnection();
    m_tcpSockets.append(tcpSocket);
    QHostAddress remoteIp = tcpSocket->peerAddress();
    quint16 remotePort = tcpSocket->peerPort();
    m_comboBoxTcpSocket->addItem(QString("%1:%2").arg(remoteIp.toString().arg(
                                     QString::number(remotePort))),
                                 QVariant::fromValue(tcpSocket));
    connect(tcpSocket, &QTcpSocket::readyRead, this,
            &TcpServerMonitor::on_tcpSocket_receiveData);
}

void TcpServerMonitor::on_tcpSocket_receiveData()
{
    QTcpSocket *tcpSocket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(tcpSocket != nullptr);
    QString src = QString("%1:%2")
                      .arg(tcpSocket->peerAddress().toString())
                      .arg(QString::number(tcpSocket->peerPort()));
    QString data = tcpSocket->readAll();
    m_lock.lock();
    m_textBrowserReceivedData->append(
        QString("Receiced data from %1:").arg(src));
    m_textBrowserReceivedData->append(data);
    m_lock.unlock();
}
