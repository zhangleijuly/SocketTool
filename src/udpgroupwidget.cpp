#include "udpgroupwidget.h"

UdpGroupWidget::UdpGroupWidget(QWidget *parent) : QWidget(parent)
{
    m_comboBoxIP = new QComboBox;
    m_spinBoxPort = new QSpinBox;
    m_lineEditMulticastIP = new QLineEdit("224.0.0.0");
    QPushButton *pushButtonOK = new QPushButton("OK");
    QPushButton *pushButtonCancel = new QPushButton("Cancel");

    m_comboBoxIP->addItem("Any-address", "");
    QList<QHostAddress> listHostAddress = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, listHostAddress)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            m_comboBoxIP->addItem(address.toString(), address.toString());
        }
    }

    m_spinBoxPort->setRange(1, 65535);
    m_spinBoxPort->setValue(7000);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";  // 255
    QRegExp ipRegex("^" + ipRange + "(\\." + ipRange + ")" + "(\\." + ipRange +
                    ")" + "(\\." + ipRange + ")$");

    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    m_lineEditMulticastIP->setValidator(ipValidator);

    connect(pushButtonOK, &QPushButton::clicked, this,
            &UdpGroupWidget::on_pushButtonOK_clicked);
    connect(pushButtonCancel, &QPushButton::clicked, this,
            &UdpGroupWidget::on_pushButtonCancel_clicked);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel("Server IP:"), 0, 0, Qt::AlignRight);
    gridLayout->addWidget(m_comboBoxIP, 0, 1);
    gridLayout->addWidget(new QLabel("Server port:"), 1, 0, Qt::AlignRight);
    gridLayout->addWidget(m_spinBoxPort, 1, 1);
    gridLayout->addWidget(new QLabel("Multicast IP:"), 2, 0, Qt::AlignRight);
    gridLayout->addWidget(m_lineEditMulticastIP, 2, 1);
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(pushButtonOK);
    hBoxLayout->addWidget(pushButtonCancel);
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(gridLayout);
    vBoxLayout->addLayout(hBoxLayout);

    setLayout(vBoxLayout);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setWindowFlag(Qt::Tool);
}

void UdpGroupWidget::on_pushButtonOK_clicked()
{
    int port = m_spinBoxPort->value();
    QString address = m_comboBoxIP->currentData().toString();
    QString multicastIP = m_lineEditMulticastIP->text();
    if (multicastIP.isEmpty())
    {
        return;
    }

    QObject *obj = parent();
    if (obj != nullptr)
    {
        QUdpSocket *udpSocket = new QUdpSocket(obj);
        if (udpSocket->bind(
                address.isEmpty() ? QHostAddress::Any : QHostAddress(address),
                port, QAbstractSocket::ShareAddress))
        {
            //            udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,
            //                                       0);
            udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 10);
            if (udpSocket->joinMulticastGroup(QHostAddress(multicastIP)))
            {
                emit udpMulticastCreated(udpSocket, multicastIP);
                close();
            }
            else
            {
                QString estr = QString("Join multicast group failed! %1")
                                   .arg(udpSocket->errorString());
                QMessageBox::critical(this, tr("Error"), estr);
                udpSocket->deleteLater();
            }
        }
        else
        {
            QString estr = QString("Create UDP socket failed! %1")
                               .arg(udpSocket->errorString());
            QMessageBox::critical(this, tr("Error"), estr);
            udpSocket->deleteLater();
        }
    }
}

void UdpGroupWidget::on_pushButtonCancel_clicked() { close(); }
