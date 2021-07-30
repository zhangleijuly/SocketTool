#include "udpsocketwidget.h"

UdpSocketWidget::UdpSocketWidget(QWidget *parent) : QWidget(parent)
{
    m_comboBoxIP = new QComboBox;
    m_spinBoxPort = new QSpinBox;
    QPushButton *pushButtonOK = new QPushButton(tr("OK"));
    QPushButton *pushButtonCancel = new QPushButton(tr("Cancel"));

    m_comboBoxIP->addItem("Any-address", "");
    QList<QHostAddress> listHostAddress = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, listHostAddress)
    {
        m_comboBoxIP->addItem(address.toString(), address.toString());
    }

    m_spinBoxPort->setRange(1, 65535);
    m_spinBoxPort->setValue(7000);

    connect(pushButtonOK, &QPushButton::clicked, this,
            &UdpSocketWidget::on_pushButtonOK_clicked);
    connect(pushButtonCancel, &QPushButton::clicked, this,
            &UdpSocketWidget::on_pushButtonCancel_clicked);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Local IP:")), 0, 0, Qt::AlignRight);
    gridLayout->addWidget(m_comboBoxIP, 0, 1, Qt::AlignLeft);
    gridLayout->addWidget(new QLabel(tr("Local port:")), 1, 0, Qt::AlignRight);
    gridLayout->addWidget(m_spinBoxPort, 1, 1, Qt::AlignLeft);
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

void UdpSocketWidget::on_pushButtonOK_clicked()
{
    int port = m_spinBoxPort->value();
    QString address = m_comboBoxIP->currentData().toString();

    QObject *obj = parent();
    if (obj != nullptr)
    {
        QUdpSocket *udpSocket = new QUdpSocket(obj);
        if (udpSocket->bind(
                address.isEmpty() ? QHostAddress::Any : QHostAddress(address),
                port))
        {
            emit udpSocketCreated(udpSocket);
            close();
        }
        else
        {
            QString estr = tr("Create UDP socket failed! %1")
                               .arg(udpSocket->errorString());
            QMessageBox::critical(this, tr("Error"), estr);
            udpSocket->deleteLater();
        }
    }
}

void UdpSocketWidget::on_pushButtonCancel_clicked() { close(); }
