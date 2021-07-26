#ifndef TCPSOCKETWIDGET_H
#define TCPSOCKETWIDGET_H

#include <QHostAddress>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QSpinBox>
#include <QValidator>
#include <QWidget>
#include <QtNetwork/QTcpSocket>

class TcpSocketWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpSocketWidget(QWidget *parent = nullptr);

signals:
    void tcpSocketCreated(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                          quint16 port);

private:
    QLineEdit *m_lineEditPeerIp;
    QSpinBox *m_spinBoxPeerPort;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
};

#endif  // TCPSOCKETWIDGET_H
