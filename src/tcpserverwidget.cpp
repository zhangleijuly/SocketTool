#include "tcpserverwidget.h"

TcpServerWidget::TcpServerWidget(QWidget *parent) : QWidget(parent)
{
    m_comboBoxIP = new QComboBox;
    m_spinBoxPort = new QSpinBox;
    QPushButton *pushButtonOK = new QPushButton("OK");
    QPushButton *pushButtonCancel = new QPushButton("Cancel");

    m_comboBoxIP->addItem("Any-address", "");
    QList<QHostAddress> listHostAddress = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, listHostAddress)
    {
        m_comboBoxIP->addItem(address.toString(), address.toString());
    }

    m_spinBoxPort->setRange(1, 65535);
    m_spinBoxPort->setValue(7000);

    connect(pushButtonOK, &QPushButton::clicked, this,
            &TcpServerWidget::on_pushButtonOK_clicked);
    connect(pushButtonCancel, &QPushButton::clicked, this,
            &TcpServerWidget::on_pushButtonCancel_clicked);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel("Server IP:"), 0, 0, Qt::AlignRight);
    gridLayout->addWidget(m_comboBoxIP, 0, 1);
    gridLayout->addWidget(new QLabel("Server port:"), 1, 0, Qt::AlignRight);
    gridLayout->addWidget(m_spinBoxPort, 1, 1);
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

void TcpServerWidget::on_pushButtonOK_clicked()
{
    int port = m_spinBoxPort->value();
    QString address = m_comboBoxIP->currentData().toString();

    QObject *obj = parent();
    if (obj != nullptr)
    {
        QTcpServer *tcpServer = new QTcpServer(obj);
        if (tcpServer->listen(
                address.isEmpty() ? QHostAddress::Any : QHostAddress(address),
                port))
        {
            emit tcpServerCreated(tcpServer);
            close();
        }
        else
        {
            QString estr = QString("Create TCP server failed! %1")
                               .arg(tcpServer->errorString());
            QMessageBox::critical(this, tr("Error"), estr);
            tcpServer->deleteLater();
        }
    }
}

void TcpServerWidget::on_pushButtonCancel_clicked() { close(); }
