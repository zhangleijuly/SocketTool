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

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_comboBoxIP, 0, 0, 1, 2);
    layout->addWidget(m_spinBoxPort, 1, 0, 1, 2);
    layout->addWidget(pushButtonOK, 2, 0);
    layout->addWidget(pushButtonCancel, 2, 1);

    setLayout(layout);
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
