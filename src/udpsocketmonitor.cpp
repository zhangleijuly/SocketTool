#include "udpsocketmonitor.h"

UdpSocketMonitor::UdpSocketMonitor(QWidget *parent) : QWidget(parent)
{
    m_udpSocket = nullptr;
    m_isMulticast = false;
    m_timer.stop();
    m_currentTimes = 0;
    m_totalTimes = 0;

    // Socket status
    QGroupBox *groupBoxSocketStatus = new QGroupBox(tr("Socket Status"));

    QHBoxLayout *layoutSocketStatus = new QHBoxLayout;

    m_labelLocalIP = new QLabel;
    m_labelLocalPort = new QLabel;
    m_labelMulticastIP = new QLabel;

    layoutSocketStatus->addWidget(m_labelLocalIP);
    layoutSocketStatus->addSpacing(5);
    layoutSocketStatus->addWidget(m_labelLocalPort);
    layoutSocketStatus->addStretch();
    layoutSocketStatus->addWidget(m_labelMulticastIP);

    groupBoxSocketStatus->setLayout(layoutSocketStatus);

    // Receive data
    QGroupBox *groupBoxReceiveData =
        new QGroupBox(tr("Received data and notification"));

    m_textBrowserReceivedData = new QTextBrowser;

    groupBoxReceiveData->setLayout(new QVBoxLayout);
    groupBoxReceiveData->layout()->addWidget(m_textBrowserReceivedData);

    // Send data
    QGroupBox *groupBoxSendData = new QGroupBox(tr("Send data"));

    m_textEditSendData = new QTextEdit;

    groupBoxSendData->setLayout(new QVBoxLayout);
    groupBoxSendData->layout()->addWidget(m_textEditSendData);

    // Send data options
    QGroupBox *groupBoxSendOptions = new QGroupBox(tr("Send options"));

    m_lineEditPeerIp = new QLineEdit("127.0.0.1");
    m_spinBoxPeerPort = new QSpinBox;
    m_comboBoxNumberSend = new QComboBox;
    m_lineEditInterval = new QLineEdit("100");
    m_pushButtonSendPeriod = new QPushButton(tr("Cycle sending"));
    m_pushButtonStopSend = new QPushButton(tr("Stop sending"));

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";  // 255
    QRegExp ipRegex("^" + ipRange + "(\\." + ipRange + ")" + "(\\." + ipRange +
                    ")" + "(\\." + ipRange + ")$");

    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    m_lineEditPeerIp->setValidator(ipValidator);

    m_spinBoxPeerPort->setRange(1, 65535);
    m_spinBoxPeerPort->setValue(7000);

    connect(m_pushButtonSendPeriod, &QPushButton::clicked, this,
            &UdpSocketMonitor::on_pushButtonSendPeriod_clicked);
    connect(m_pushButtonStopSend, &QPushButton::clicked, this,
            &UdpSocketMonitor::on_pushButtonStopSend_clicked);

    for (int i = 0; i < 5; ++i)
    {
        int temp = pow(10, i);
        m_comboBoxNumberSend->addItem(QString::number(temp), temp);
    }

    QVBoxLayout *layoutSendOptions = new QVBoxLayout;
    QHBoxLayout *layoutSendOptionsR1 = new QHBoxLayout;
    QHBoxLayout *layoutSendOptionsR2 = new QHBoxLayout;

    m_labelPeerIP = new QLabel(tr("Peer IP:"));
    m_labelPeerPort = new QLabel(tr("Peer port:"));
    QLabel *labelNumberSend = new QLabel(tr("Send times:"));
    QLabel *labelInterval = new QLabel(tr("Send interval(ms):"));

    layoutSendOptionsR1->addStretch();
    layoutSendOptionsR1->addWidget(m_labelPeerIP);
    m_labelPeerIP->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_labelPeerIP->setMinimumWidth(200);
    layoutSendOptionsR1->addWidget(m_lineEditPeerIp);
    m_lineEditPeerIp->setFixedWidth(200);
    layoutSendOptionsR1->addStretch();
    layoutSendOptionsR1->addWidget(labelNumberSend);
    labelNumberSend->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelNumberSend->setMinimumWidth(250);
    layoutSendOptionsR1->addWidget(m_comboBoxNumberSend);
    m_comboBoxNumberSend->setFixedWidth(200);
    layoutSendOptionsR1->addStretch();
    layoutSendOptionsR1->addWidget(m_pushButtonSendPeriod);
    m_pushButtonSendPeriod->setMinimumWidth(200);
    layoutSendOptionsR1->addStretch();

    layoutSendOptionsR2->addStretch();
    layoutSendOptionsR2->addWidget(m_labelPeerPort);
    m_labelPeerPort->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_labelPeerPort->setMinimumWidth(200);
    layoutSendOptionsR2->addWidget(m_spinBoxPeerPort);
    m_spinBoxPeerPort->setFixedWidth(200);
    layoutSendOptionsR2->addStretch();
    layoutSendOptionsR2->addWidget(labelInterval);
    labelInterval->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelInterval->setMinimumWidth(250);
    layoutSendOptionsR2->addWidget(m_lineEditInterval);
    m_lineEditInterval->setFixedWidth(200);
    layoutSendOptionsR2->addStretch();
    layoutSendOptionsR2->addWidget(m_pushButtonStopSend);
    m_pushButtonStopSend->setMinimumWidth(200);
    layoutSendOptionsR2->addStretch();

    layoutSendOptions->addLayout(layoutSendOptionsR1);
    layoutSendOptions->addLayout(layoutSendOptionsR2);

    groupBoxSendOptions->setLayout(layoutSendOptions);

    // Layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxSocketStatus);
    vBoxLayout->addWidget(groupBoxReceiveData);
    vBoxLayout->addWidget(groupBoxSendData);
    vBoxLayout->addWidget(groupBoxSendOptions);

    setLayout(vBoxLayout);

    connect(&m_timer, &QTimer::timeout, this,
            &UdpSocketMonitor::on_timer_timeout);
}

UdpSocketMonitor::~UdpSocketMonitor()
{
    if (m_udpSocket != nullptr)
    {
        m_udpSocket->close();
        m_udpSocket->deleteLater();
    }
}

void UdpSocketMonitor::setUdpSocket(QUdpSocket *udpSocket)
{
    Q_ASSERT(m_udpSocket == nullptr);
    if (udpSocket == nullptr)
    {
        return;
    }

    m_udpSocket = udpSocket;
    connect(m_udpSocket, &QUdpSocket::readyRead, this,
            &UdpSocketMonitor::on_udpSocket_readyRead);
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(on_udpSocket_error(QAbstractSocket::SocketError)));
    m_labelLocalIP->setText(
        tr("Local IP: %1").arg(m_udpSocket->localAddress().toString()));
    m_labelLocalPort->setText(
        tr("Local port: %1").arg(m_udpSocket->localPort()));
}

void UdpSocketMonitor::setUdpMulticast(QUdpSocket *udpSocket,
                                       const QString &multicastIP)
{
    Q_ASSERT(m_udpSocket == nullptr);
    if (udpSocket == nullptr)
    {
        return;
    }

    m_udpSocket = udpSocket;
    m_isMulticast = true;
    m_multicastIP = QHostAddress(multicastIP);
    m_multicastPort = 45454;
    connect(m_udpSocket, &QUdpSocket::readyRead, this,
            &UdpSocketMonitor::on_udpSocket_readyRead);
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(on_udpSocket_error(QAbstractSocket::SocketError)));
    m_labelLocalIP->setText(
        tr("Local IP: %1").arg(m_udpSocket->localAddress().toString()));
    m_labelLocalPort->setText(
        tr("Local port: %1").arg(m_udpSocket->localPort()));
    m_labelMulticastIP->setText(
        tr("Multicast IP: %1").arg(m_multicastIP.toString()));
    m_labelPeerIP->hide();
    m_labelPeerPort->hide();
    m_lineEditPeerIp->hide();
    m_spinBoxPeerPort->hide();
}

void UdpSocketMonitor::on_pushButtonSendPeriod_clicked()
{
    m_pushButtonSendPeriod->setEnabled(false);
    int interval = m_lineEditInterval->text().toInt();
    if (interval > 0)
    {
        m_timer.start(interval);
        m_currentTimes = 0;
        m_totalTimes = m_comboBoxNumberSend->currentData().toInt();
    }
    else
    {
        m_timer.stop();
    }
}

void UdpSocketMonitor::on_pushButtonStopSend_clicked()
{
    m_timer.stop();
    m_currentTimes = 0;
    m_totalTimes = 0;
    m_pushButtonSendPeriod->setEnabled(true);
}

void UdpSocketMonitor::on_udpSocket_readyRead()
{
    if (m_udpSocket != nullptr)
    {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress hostAddress;
        quint16 port;
        m_udpSocket->readDatagram(datagram.data(), datagram.size(),
                                  &hostAddress, &port);
        QString src = QString("[%1:%2]").arg(hostAddress.toString()).arg(port);
        m_textBrowserReceivedData->append(
            tr("%1 Received data from %2:")
                .arg(QTime::currentTime().toString(), src));
        m_textBrowserReceivedData->append(datagram);
    }
}

void UdpSocketMonitor::on_udpSocket_error(QAbstractSocket::SocketError)
{
    QString msg =
        tr("%1 Error: %2")
            .arg(QTime::currentTime().toString(), m_udpSocket->errorString());
    m_textBrowserReceivedData->append(msg);
}

void UdpSocketMonitor::on_timer_timeout()
{
    QString data = m_textEditSendData->toPlainText();
    if (m_udpSocket != nullptr && !data.isEmpty())
    {
        if (m_isMulticast)
        {
            QString src = QString("[%1:%2]")
                              .arg(m_multicastIP.toString())
                              .arg(m_multicastPort);
            m_textBrowserReceivedData->append(
                tr("%1 Send data to %2:")
                    .arg(QTime::currentTime().toString(), src));
            m_textBrowserReceivedData->append(data);
            m_udpSocket->writeDatagram(data.toUtf8(), m_multicastIP,
                                       m_multicastPort);
        }
        else
        {
            QString peerIP = m_lineEditPeerIp->text();
            quint16 peerPort = m_spinBoxPeerPort->value();
            if (!peerIP.isEmpty())
            {
                QString src = QString("[%1:%2]").arg(peerIP).arg(peerPort);
                m_textBrowserReceivedData->append(
                    tr("%1 Send data to %2:")
                        .arg(QTime::currentTime().toString(), src));
                m_textBrowserReceivedData->append(data);
                m_udpSocket->writeDatagram(data.toUtf8(), QHostAddress(peerIP),
                                           peerPort);
            }
        }
    }
    if (++m_currentTimes == m_totalTimes)
    {
        m_timer.stop();
        m_currentTimes = 0;
        m_totalTimes = 0;
        m_pushButtonSendPeriod->setEnabled(true);
    }
}
