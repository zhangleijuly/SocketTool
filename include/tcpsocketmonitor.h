#ifndef TCPSOCKETMONITOR_H
#define TCPSOCKETMONITOR_H

#include <QComboBox>
#include <QGroupBox>
#include <QHostAddress>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMutex>
#include <QNetworkProxy>
#include <QPushButton>
#include <QTextBrowser>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <cmath>

class TcpSocketMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TcpSocketMonitor(QWidget *parent = nullptr);
    ~TcpSocketMonitor();

    void setTcpSocket(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                      quint16 port);

signals:

private:
    QTcpSocket *m_tcpSocket;
    QLabel *m_labelSocketStatus;
    QLabel *m_labelLocalIP;
    QLabel *m_labelLocalPort;
    QLabel *m_labelPeerIP;
    QLabel *m_labelPeerPort;
    QPushButton *m_pushButtonConnect;
    QPushButton *m_pushButtonDisconnect;
    QTextBrowser *m_textBrowserReceivedData;
    QTextEdit *m_textEditSendData;
    QComboBox *m_comboBoxNumberSend;
    QLineEdit *m_lineEditInterval;
    QPushButton *m_pushButtonSendPeriod;
    QPushButton *m_pushButtonStopSend;
    QHostAddress m_peerIP;
    quint16 m_peerPort;
    QTimer m_timer;
    int m_currentTimes;
    int m_totalTimes;

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonSendPeriod_clicked();
    void on_pushButtonStopSend_clicked();
    void on_tcpSocket_stateChanged(QAbstractSocket::SocketState socketState);
    void on_tcpSocket_connect();
    void on_tcpSocket_disconnect();
    void on_tcpSocket_readyRead();
    void on_tcpSocket_error();
    void on_timer_timeout();
};

#endif  // TCPSOCKETMONITOR_H
