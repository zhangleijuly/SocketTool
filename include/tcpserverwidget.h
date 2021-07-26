#ifndef TCPSERVERWIDGET_H
#define TCPSERVERWIDGET_H

#include <QComboBox>
#include <QHostAddress>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include <QtNetwork/QTcpServer>

class TcpServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServerWidget(QWidget *parent = nullptr);

signals:
    void tcpServerCreated(QTcpServer *tcpServer);

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();

private:
    QComboBox *m_comboBoxIP;
    QSpinBox *m_spinBoxPort;
};

#endif  // TCPSERVERWIDGET_H
