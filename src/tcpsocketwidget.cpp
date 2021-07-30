#include "tcpsocketwidget.h"

TcpSocketWidget::TcpSocketWidget(QWidget *parent) : QWidget(parent)
{
    m_lineEditPeerIp = new QLineEdit("127.0.0.1");
    m_spinBoxPeerPort = new QSpinBox;
    QPushButton *pushButtonOK = new QPushButton(tr("OK"));
    QPushButton *pushButtonCancel = new QPushButton(tr("Cancel"));

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";  // 255
    QRegExp ipRegex("^" + ipRange + "(\\." + ipRange + ")" + "(\\." + ipRange +
                    ")" + "(\\." + ipRange + ")$");

    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    m_lineEditPeerIp->setValidator(ipValidator);

    m_spinBoxPeerPort->setRange(1, 65535);
    m_spinBoxPeerPort->setValue(7000);

    connect(pushButtonOK, &QPushButton::clicked, this,
            &TcpSocketWidget::on_pushButtonOK_clicked);
    connect(pushButtonCancel, &QPushButton::clicked, this,
            &TcpSocketWidget::on_pushButtonCancel_clicked);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Peer IP:")), 0, 0, Qt::AlignRight);
    gridLayout->addWidget(m_lineEditPeerIp, 0, 1, Qt::AlignLeft);
    gridLayout->addWidget(new QLabel(tr("Peer port:")), 1, 0, Qt::AlignRight);
    gridLayout->addWidget(m_spinBoxPeerPort, 1, 1, Qt::AlignLeft);
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(pushButtonOK);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(pushButtonCancel);
    hBoxLayout->addStretch();
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(gridLayout);
    vBoxLayout->addLayout(hBoxLayout);

    setLayout(vBoxLayout);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setWindowFlag(Qt::Tool);
}

void TcpSocketWidget::on_pushButtonOK_clicked()
{
    QString peerIP = m_lineEditPeerIp->text();
    if (peerIP.isEmpty())
    {
        return;
    }
    QHostAddress hostAddress(peerIP);
    if (hostAddress.isNull())
    {
        return;
    }

    quint16 port = m_spinBoxPeerPort->value();

    QObject *p = parent();
    if (p != nullptr)
    {
        QTcpSocket *tcpSocket = new QTcpSocket(p);
        if (tcpSocket != nullptr)
        {
            emit tcpSocketCreated(tcpSocket, hostAddress, port);
            close();
        }
    }
}

void TcpSocketWidget::on_pushButtonCancel_clicked() { close(); }
