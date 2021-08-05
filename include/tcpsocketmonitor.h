#ifndef TCPSOCKETMONITOR_H
#define TCPSOCKETMONITOR_H

#include <QWidget>

#include "stdafx.h"

class TcpSocketMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TcpSocketMonitor(QWidget *parent = nullptr);
    ~TcpSocketMonitor();
    void setTcpSocket(QTcpSocket *tcpSocket, QHostAddress hostAddress,
                      quint16 port);

signals:
    void tcpSocketReceivedData(QString info, QString data);

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
    QCheckBox *m_checkBoxHex;
    QComboBox *m_comboBoxNumberSend;
    QLineEdit *m_lineEditInterval;
    QPushButton *m_pushButtonSendPeriod;
    QPushButton *m_pushButtonStopSend;
    QHostAddress m_peerIP;
    quint16 m_peerPort;
    QTimer m_timer;
    int m_currentTimes;
    int m_totalTimes;
    bool eventFilter(QObject *obj, QEvent *e);
    void formatTextEdit(const QString &str = "");

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonSendPeriod_clicked();
    void on_pushButtonStopSend_clicked();
    void on_tcpSocket_stateChanged(QAbstractSocket::SocketState socketState);
    void on_tcpSocket_connect();
    void on_tcpSocket_disconnect();
    void on_tcpSocket_readyRead();
    void on_tcpSocket_error(QAbstractSocket::SocketError);
    void on_timer_timeout();
    void on_checkBox_stateChanged();
};

#endif  // TCPSOCKETMONITOR_H
