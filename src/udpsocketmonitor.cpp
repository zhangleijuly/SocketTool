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
    m_textEditSendData->installEventFilter(this);
    m_checkBoxHex = new QCheckBox(tr("Send in hexadecimal"));
    m_checkBoxHex->setChecked(true);
    connect(m_checkBoxHex, &QCheckBox::stateChanged, this,
            &UdpSocketMonitor::on_checkBox_stateChanged);

    groupBoxSendData->setLayout(new QVBoxLayout);
    groupBoxSendData->layout()->addWidget(m_textEditSendData);
    groupBoxSendData->layout()->addWidget(m_checkBoxHex);

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

bool UdpSocketMonitor::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_textEditSendData)  //判断过滤事件是否是发送窗口事件
    {
        if (m_checkBoxHex->isChecked() == false)  //字符串格式，不过滤
            return false;                         //不拦截，继续传播
        else if (m_checkBoxHex->isChecked() == true)  // hex格式
        {
            //英文输入法下的键盘事件
            if (e->type() == QEvent::KeyPress)  //过滤键盘事件
            {
                QKeyEvent *keyEvent =
                    static_cast<QKeyEvent *>(e);  //强行转换为按键事件
                int keyValue = keyEvent->key();   //获取按键值

                if (keyValue >= Qt::Key_0 && keyValue <= Qt::Key_9)  // 0~9
                {
                    formatTextEdit();
                    return false;  //不拦截，继续传播
                }
                else if (keyValue >= Qt::Key_A &&
                         keyValue <= Qt::Key_F)  // a~f,A~F
                {
                    formatTextEdit();
                    return false;  //不拦截，继续传播
                }
                else if (keyValue == Qt::Key_Space ||
                         keyValue == Qt::Key_Comma ||
                         keyValue == Qt::Key_Backspace)
                {
                    return false;  //不拦截，继续传播，空格，逗号，退格按键
                }
                else              //对其他情况拦截
                    return true;  //拦截，不再传播
            }
            //中文输入法状态下的键盘事件
            else if (e->type() == QEvent::InputMethod)
            {
                QInputMethodEvent *keyEvent = dynamic_cast<QInputMethodEvent *>(
                    e);  //强制转换为输入法事件
                QString strInput =
                    keyEvent->commitString();  //此时的strInput就是键盘上按键

                if (strInput
                        .isEmpty())  //非按键事件不拦截，不加这个操作，窗口不会实时显示输入的按键字母
                    return false;    //不拦截，继续传播

                strInput.remove(
                    QRegExp("[^a-fA-F0-9]"));  //对输入的内容筛选，hex格式的留下
                // keyEvent->setCommitString(strInput);  //更改指针的字符串内容
                keyEvent->setCommitString("");
                formatTextEdit(strInput);
                return false;  //不拦截，继续传播(输入的数据已经被更改了)
            }

            return false;  //不拦截，不再传播
        }
        else
            return false;  //不拦截，继续传播
    }
    else
        return QWidget::eventFilter(
            obj,
            e);  //当不确定是否继续传播时，按照父类的方法来处理.即调用父类的evenFilter函数
}

void UdpSocketMonitor::formatTextEdit(const QString &str)
{
    if (m_checkBoxHex->isChecked())
    {
        QString temp = m_textEditSendData->toPlainText() + str;
        temp = temp.remove(" ");
        QString res;
        if (temp.size() <= 1)
        {
            res = temp;
        }
        else
        {
            for (int i = 0; i < (temp.size() - 1) / 2; i++)
            {
                res += temp[i * 2];
                res += temp[i * 2 + 1];
                res += " ";
            }
            if (temp.size() % 2 == 0)
            {
                res += temp[temp.size() - 2];
                res += temp[temp.size() - 1];
                res += " ";
            }
            else
            {
                res += temp[temp.size() - 1];
            }
        }
        m_textEditSendData->setText(res);
        m_textEditSendData->moveCursor(QTextCursor::End,
                                       QTextCursor::MoveAnchor);
    }
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
    if (m_checkBoxHex->isChecked())
    {
        data.remove(" ");
    }
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

void UdpSocketMonitor::on_checkBox_stateChanged()
{
    if (m_checkBoxHex->isChecked())
    {
        QString str = m_textEditSendData->toPlainText();
        str.remove(QRegExp("[^a-fA-F0-9]"));
        m_textEditSendData->setPlainText(str);
        formatTextEdit();
    }
    else
    {
        QString str = m_textEditSendData->toPlainText();
        str.remove(" ");
        m_textEditSendData->setPlainText(str);
    }
}
