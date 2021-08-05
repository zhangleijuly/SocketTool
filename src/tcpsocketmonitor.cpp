#include "tcpsocketmonitor.h"

TcpSocketMonitor::TcpSocketMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpSocket = nullptr;
    m_peerPort = 0;
    m_timer.stop();
    m_currentTimes = 0;
    m_totalTimes = 0;

    // Socket status
    QGroupBox *groupBoxSocketStatus = new QGroupBox(tr("Socket Status"));

    m_pushButtonConnect = new QPushButton(tr("Connect"));
    m_pushButtonDisconnect = new QPushButton(tr("Disconnect"));
    connect(m_pushButtonConnect, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonConnect_clicked);
    connect(m_pushButtonDisconnect, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonDisconnect_clicked);

    m_labelSocketStatus = new QLabel;
    m_labelLocalIP = new QLabel;
    m_labelLocalPort = new QLabel;
    m_labelPeerIP = new QLabel;
    m_labelPeerPort = new QLabel;

    QVBoxLayout *layoutSocketStatus = new QVBoxLayout;
    QHBoxLayout *layoutSocketStatusR1 = new QHBoxLayout;
    QHBoxLayout *layoutSocketStatusR2 = new QHBoxLayout;

    layoutSocketStatusR1->addWidget(m_labelSocketStatus);
    m_labelSocketStatus->setMinimumWidth(200);
    m_labelSocketStatus->setAlignment(Qt::AlignHCenter);
    layoutSocketStatusR1->addStretch();
    layoutSocketStatusR1->addWidget(m_labelLocalIP);
    layoutSocketStatusR1->addSpacing(5);
    layoutSocketStatusR1->addWidget(m_labelLocalPort);
    m_labelLocalPort->setMinimumWidth(200);

    layoutSocketStatusR2->addWidget(m_pushButtonConnect);
    m_pushButtonConnect->setMinimumWidth(200);
    layoutSocketStatusR2->addSpacing(5);
    layoutSocketStatusR2->addWidget(m_pushButtonDisconnect);
    m_pushButtonDisconnect->setMinimumWidth(200);
    layoutSocketStatusR2->addStretch();
    layoutSocketStatusR2->addWidget(m_labelPeerIP);
    layoutSocketStatusR2->addSpacing(5);
    layoutSocketStatusR2->addWidget(m_labelPeerPort);
    m_labelPeerPort->setMinimumWidth(200);

    layoutSocketStatus->addLayout(layoutSocketStatusR1);
    layoutSocketStatus->addLayout(layoutSocketStatusR2);

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
            &TcpSocketMonitor::on_checkBox_stateChanged);

    groupBoxSendData->setLayout(new QVBoxLayout);
    groupBoxSendData->layout()->addWidget(m_textEditSendData);
    groupBoxSendData->layout()->addWidget(m_checkBoxHex);

    // Send data options
    QGroupBox *groupBoxSendOptions = new QGroupBox(tr("Send options"));

    m_comboBoxNumberSend = new QComboBox;
    m_lineEditInterval = new QLineEdit("100");
    m_pushButtonSendPeriod = new QPushButton(tr("Cycle sending"));
    m_pushButtonStopSend = new QPushButton(tr("Stop sending"));
    connect(m_pushButtonSendPeriod, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonSendPeriod_clicked);
    connect(m_pushButtonStopSend, &QPushButton::clicked, this,
            &TcpSocketMonitor::on_pushButtonStopSend_clicked);

    for (int i = 0; i < 5; ++i)
    {
        int temp = pow(10, i);
        m_comboBoxNumberSend->addItem(QString::number(temp), temp);
    }

    QVBoxLayout *layoutSendOptions = new QVBoxLayout;
    QHBoxLayout *layoutSendOptionsR1 = new QHBoxLayout;
    QHBoxLayout *layoutSendOptionsR2 = new QHBoxLayout;

    QLabel *labelNumberSend = new QLabel(tr("Send times:"));
    labelNumberSend->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel *labelInterval = new QLabel(tr("Send interval(ms):"));
    labelInterval->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    layoutSendOptionsR1->addStretch();
    layoutSendOptionsR1->addWidget(labelNumberSend);
    labelNumberSend->setMinimumWidth(250);
    layoutSendOptionsR1->addWidget(m_comboBoxNumberSend);
    m_comboBoxNumberSend->setFixedWidth(200);
    layoutSendOptionsR1->addStretch();
    layoutSendOptionsR1->addWidget(m_pushButtonSendPeriod);
    m_pushButtonSendPeriod->setMinimumWidth(200);
    layoutSendOptionsR1->addStretch();

    layoutSendOptionsR2->addStretch();
    layoutSendOptionsR2->addWidget(labelInterval);
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
            &TcpSocketMonitor::on_timer_timeout);
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

    m_pushButtonConnect->setEnabled(true);
    m_pushButtonDisconnect->setEnabled(false);
    on_tcpSocket_stateChanged(m_tcpSocket->state());
    m_labelPeerIP->setText(tr("Peer IP: %1").arg(m_peerIP.toString()));
    m_labelPeerPort->setText(tr("Peer port: %1").arg(m_peerPort));

    connect(m_tcpSocket, &QTcpSocket::connected, this,
            &TcpSocketMonitor::on_tcpSocket_connect);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this,
            &TcpSocketMonitor::on_tcpSocket_disconnect);
    connect(m_tcpSocket, &QTcpSocket::stateChanged, this,
            &TcpSocketMonitor::on_tcpSocket_stateChanged);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this,
            &TcpSocketMonitor::on_tcpSocket_readyRead);
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(on_tcpSocket_error(QAbstractSocket::SocketError)));
}

bool TcpSocketMonitor::eventFilter(QObject *obj, QEvent *e)
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

void TcpSocketMonitor::formatTextEdit(const QString &str)
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

void TcpSocketMonitor::on_pushButtonConnect_clicked()
{
    m_tcpSocket->connectToHost(m_peerIP, m_peerPort);
    m_pushButtonConnect->setEnabled(false);
    m_labelLocalIP->setText(
        tr("Local IP: %1").arg(m_tcpSocket->localAddress().toString()));
    m_labelLocalPort->setText(
        tr("Local port: %1").arg(m_tcpSocket->localPort()));
}

void TcpSocketMonitor::on_pushButtonDisconnect_clicked()
{
    m_tcpSocket->disconnectFromHost();
    m_labelLocalIP->clear();
    m_labelLocalPort->clear();
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
            m_labelSocketStatus->setText(tr("Unconnected"));
            m_pushButtonConnect->setEnabled(true);
            m_pushButtonDisconnect->setEnabled(false);
            m_labelLocalIP->clear();
            m_labelLocalPort->clear();
            break;
        case QAbstractSocket::HostLookupState:
            m_labelSocketStatus->setText(tr("Looking up host"));
            break;
        case QAbstractSocket::ConnectingState:
            m_labelSocketStatus->setText(tr("Connecting"));
            break;
        case QAbstractSocket::ConnectedState:
            m_labelSocketStatus->setText(tr("Connected"));
            m_pushButtonConnect->setEnabled(false);
            m_pushButtonDisconnect->setEnabled(true);
            m_labelLocalIP->setText(
                tr("Local IP: %1").arg(m_tcpSocket->localAddress().toString()));
            m_labelLocalPort->setText(
                tr("Local port: %1").arg(m_tcpSocket->localPort()));
            break;
        case QAbstractSocket::BoundState:
            m_labelSocketStatus->setText(tr("Bound"));
            break;
        case QAbstractSocket::ClosingState:
            m_labelSocketStatus->setText(tr("Closing"));
            break;
        case QAbstractSocket::ListeningState:
            m_labelSocketStatus->setText(tr("Listening"));
            break;
        default:
            break;
    }
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
        QString src =
            QString("[%1:%2]").arg(m_peerIP.toString()).arg(m_peerPort);
        QString info = tr("%1 Receiced data from %2:")
                           .arg(QTime::currentTime().toString(), src);
        QString data = m_tcpSocket->readAll();
        m_textBrowserReceivedData->append(info);
        m_textBrowserReceivedData->append(data);
        emit tcpSocketReceivedData(info, data);
    }
}

void TcpSocketMonitor::on_tcpSocket_error(QAbstractSocket::SocketError)
{
    QString msg =
        tr("%1 Error: %2")
            .arg(QTime::currentTime().toString(), m_tcpSocket->errorString());
    m_textBrowserReceivedData->append(msg);
}

void TcpSocketMonitor::on_timer_timeout()
{
    QString data = m_textEditSendData->toPlainText();
    if (m_checkBoxHex->isChecked())
    {
        data.remove(" ");
    }
    if (m_tcpSocket != nullptr && m_tcpSocket->isWritable() && !data.isEmpty())
    {
        m_tcpSocket->write(data.toUtf8());
        QString src =
            QString("[%1:%2]").arg(m_peerIP.toString()).arg(m_peerPort);
        m_textBrowserReceivedData->append(
            tr("%1 Send data to %2:")
                .arg(QTime::currentTime().toString(), src));
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

void TcpSocketMonitor::on_checkBox_stateChanged()
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
