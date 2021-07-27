#include "tcpsocketmonitor.h"

TcpSocketMonitor::TcpSocketMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpSocket = nullptr;
    m_peerPort = 0;
    m_timer.stop();
    m_currentTimes = 0;
    m_totalTimes = 0;

    // Socket status
    QGroupBox *groupBoxSocketStatus = new QGroupBox("Socket Status");

    QGridLayout *layoutSocketStatus = new QGridLayout;

    m_pushButtonConnect = new QPushButton("Connect");
    m_pushButtonDisconnect = new QPushButton("Disconnect");
    connect(m_pushButtonConnect, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonConnect_clicked);
    connect(m_pushButtonDisconnect, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonDisconnect_clicked);

    m_labelSocketStatus = new QLabel;
    m_labelLocalIP = new QLabel;
    m_labelLocalPort = new QLabel;
    m_labelPeerIP = new QLabel;
    m_labelPeerPort = new QLabel;

    layoutSocketStatus->addWidget(m_labelSocketStatus, 0, 0);
    layoutSocketStatus->addWidget(m_labelLocalIP, 0, 2);
    layoutSocketStatus->addWidget(m_labelLocalPort, 0, 3);
    layoutSocketStatus->addWidget(m_pushButtonConnect, 1, 0);
    layoutSocketStatus->addWidget(m_pushButtonDisconnect, 1, 1);
    layoutSocketStatus->addWidget(m_labelPeerIP, 1, 2);
    layoutSocketStatus->addWidget(m_labelPeerPort, 1, 3);

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

    m_comboBoxNumberSend = new QComboBox;
    m_lineEditInterval = new QLineEdit("100");
    m_pushButtonSendPeriod = new QPushButton("Cycle sending");
    m_pushButtonStopSend = new QPushButton("Stop sending");
    connect(m_pushButtonSendPeriod, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonSendPeriod_clicked);
    connect(m_pushButtonStopSend, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonStopSend_clicked);

    for (int i = 0; i < 5; ++i)
    {
        int temp = pow(10, i);
        m_comboBoxNumberSend->addItem(QString::number(temp), temp);
    }

    QGridLayout *gridLayoutSendOptions = new QGridLayout;

    gridLayoutSendOptions->addWidget(new QLabel("Send times"), 0, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(new QLabel("Send interval(ms)"), 1, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(m_comboBoxNumberSend, 0, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(m_lineEditInterval, 1, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(m_pushButtonSendPeriod, 0, 2);
    gridLayoutSendOptions->addWidget(m_pushButtonStopSend, 1, 2);

    groupBoxSendOptions->setLayout(gridLayoutSendOptions);

    // Layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxSocketStatus);
    vBoxLayout->addWidget(groupBoxReceiveData);
    vBoxLayout->addWidget(groupBoxSendData);
    vBoxLayout->addWidget(groupBoxSendOptions);

    setLayout(vBoxLayout);
}

TcpSocketMonitor::~TcpSocketMonitor()
{
    if (m_tcpSocket != nullptr)
    {
        m_tcpSocket->close();
        m_tcpSocket->deleteLater();
    }
}

void TcpSocketMonitor::setTcpSocket(QTcpSocket *tcpSocket,
                                    QHostAddress hostAddress, quint16 port)
{
    Q_ASSERT(m_tcpSocket == nullptr);
    if (tcpSocket == nullptr)
    {
        return;
    }

    m_tcpSocket = tcpSocket;
    m_tcpSocket->setProxy(QNetworkProxy::NoProxy);
    m_peerIP = hostAddress;
    m_peerPort = port;

    on_tcpSocket_stateChanged(m_tcpSocket->state());
    m_labelLocalIP->setText(
        QString("Local IP: %1").arg(m_tcpSocket->localAddress().toString()));
    m_labelLocalPort->setText(
        QString("Local port: %1").arg(m_tcpSocket->localPort()));
    m_labelPeerIP->setText(QString("Peer IP: %1").arg(m_peerIP.toString()));
    m_labelPeerPort->setText(QString("Peer port: %1").arg(m_peerPort));
    m_pushButtonConnect->setEnabled(true);
    m_pushButtonDisconnect->setEnabled(false);

    connect(m_tcpSocket, &QTcpSocket::connected, this,
            &TcpSocketMonitor::on_tcpSocket_connect);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this,
            &TcpSocketMonitor::on_tcpSocket_disconnect);
    connect(m_tcpSocket, &QTcpSocket::stateChanged, this,
            &TcpSocketMonitor::on_tcpSocket_stateChanged);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this,
            &TcpSocketMonitor::on_tcpSocket_readyRead);
    connect(m_tcpSocket, SIGNAL(error), this, SLOT(on_tcpSocket_error));
}

void TcpSocketMonitor::on_pushButtonConnect_clicked()
{
    m_tcpSocket->connectToHost(m_peerIP, m_peerPort);
    qDebug() << m_peerIP << ":" << m_peerPort;
    m_pushButtonConnect->setEnabled(false);
}

void TcpSocketMonitor::on_pushButtonDisconnect_clicked()
{
    m_tcpSocket->disconnectFromHost();
}

void TcpSocketMonitor::on_pushButtonSendPeriod_clicked()
{
    m_pushButtonSendPeriod->setEnabled(false);
    int interval = m_lineEditInterval->text().toInt();
    if (interval > 0)
    {
        m_timer.start(interval);
        m_currentTimes = 0;
        m_totalTimes = m_comboBoxNumberSend->currentData().toInt();
        connect(&m_timer, &QTimer::timeout, this,
                &TcpSocketMonitor::on_timer_timeout);
    }
    else
    {
        m_timer.stop();
    }
}

void TcpSocketMonitor::on_pushButtonStopSend_clicked()
{
    m_timer.stop();
    m_currentTimes = 0;
    m_totalTimes = 0;
    m_pushButtonSendPeriod->setEnabled(true);
}

void TcpSocketMonitor::on_tcpSocket_stateChanged(
    QAbstractSocket::SocketState socketState)
{
    switch (socketState)
    {
        case QAbstractSocket::UnconnectedState:
            m_labelSocketStatus->setText("Unconnected");
            break;
        case QAbstractSocket::HostLookupState:
            m_labelSocketStatus->setText("Looking up host");
            break;
        case QAbstractSocket::ConnectingState:
            m_labelSocketStatus->setText("Connecting");
            break;
        case QAbstractSocket::ConnectedState:
            m_labelSocketStatus->setText("Connected");
            break;
        case QAbstractSocket::BoundState:
            m_labelSocketStatus->setText("Bound");
            break;
        case QAbstractSocket::ClosingState:
            m_labelSocketStatus->setText("Closing");
            break;
        case QAbstractSocket::ListeningState:
            m_labelSocketStatus->setText("Listening");
            break;
        default:
            break;
    }

    m_labelLocalPort->setText(
        QString("Local port: %1").arg(m_tcpSocket->localPort()));
}

void TcpSocketMonitor::on_tcpSocket_connect()
{
    m_pushButtonConnect->setEnabled(false);
    m_pushButtonDisconnect->setEnabled(true);
}

void TcpSocketMonitor::on_tcpSocket_disconnect()
{
    m_pushButtonConnect->setEnabled(true);
    m_pushButtonDisconnect->setEnabled(false);
}

void TcpSocketMonitor::on_tcpSocket_readyRead()
{
    if (m_tcpSocket != nullptr && m_tcpSocket->bytesAvailable() > 0)
    {
        QString src = QString("%1:%2").arg(m_peerIP.toString(), m_peerPort);
        QString data = m_tcpSocket->readAll();
        m_textBrowserReceivedData->append(
            QString("%1 Receiced data from %2:")
                .arg(QTime::currentTime().toString(), src));
        m_textBrowserReceivedData->append(data);
    }
}

void TcpSocketMonitor::on_tcpSocket_error()
{
    QString msg =
        QString("%1 Error: %2")
            .arg(QTime::currentTime().toString(), m_tcpSocket->errorString());
    m_textBrowserReceivedData->append(msg);
}

void TcpSocketMonitor::on_timer_timeout()
{
    QString data = m_textEditSendData->toPlainText();
    if (m_tcpSocket != nullptr && m_tcpSocket->isWritable() && !data.isEmpty())
    {
        m_tcpSocket->write(data.toUtf8());
        m_textBrowserReceivedData->append(
            QString("%1 Send data to %2:%3 :")
                .arg(QTime::currentTime().toString(), m_peerIP.toString(),
                     QString::number(m_peerPort)));
        m_textBrowserReceivedData->append(data);
    }
    if (++m_currentTimes == m_totalTimes)
    {
        m_timer.stop();
        m_currentTimes = 0;
        m_totalTimes = 0;
        m_pushButtonSendPeriod->setEnabled(true);
    }
}
