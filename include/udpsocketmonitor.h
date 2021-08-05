#ifndef UDPSOCKETMONITOR_H
#define UDPSOCKETMONITOR_H

#include <QWidget>

#include "stdafx.h"

class UdpSocketMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit UdpSocketMonitor(QWidget *parent = nullptr);
    ~UdpSocketMonitor();
    void setUdpSocket(QUdpSocket *udpSocket);
    void setUdpMulticast(QUdpSocket *udpSocket, const QString &multicastIP);

signals:

private:
    QUdpSocket *m_udpSocket;
    bool m_isMulticast;
    QHostAddress m_multicastIP;
    quint16 m_multicastPort;
    QTimer m_timer;
    int m_currentTimes;
    int m_totalTimes;
    QLabel *m_labelLocalIP;
    QLabel *m_labelLocalPort;
    QLabel *m_labelMulticastIP;
    QLabel *m_labelPeerIP;
    QLabel *m_labelPeerPort;
    QTextBrowser *m_textBrowserReceivedData;
    QTextEdit *m_textEditSendData;
    QCheckBox *m_checkBoxHex;
    QLineEdit *m_lineEditPeerIp;
    QSpinBox *m_spinBoxPeerPort;
    QComboBox *m_comboBoxNumberSend;
    QLineEdit *m_lineEditInterval;
    QPushButton *m_pushButtonSendPeriod;
    QPushButton *m_pushButtonStopSend;
    bool eventFilter(QObject *obj, QEvent *e);
    void formatTextEdit(const QString &str = "");

private slots:
    void on_pushButtonSendPeriod_clicked();
    void on_pushButtonStopSend_clicked();
    void on_udpSocket_readyRead();
    void on_udpSocket_error(QAbstractSocket::SocketError);
    void on_timer_timeout();
    void on_checkBox_stateChanged();
};

#endif  // UDPSOCKETMONITOR_H
