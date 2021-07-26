#ifndef TCPSERVERMONITOR_H
#define TCPSERVERMONITOR_H

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QMutex>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <cmath>
class TcpServerMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServerMonitor(QWidget *parent = nullptr);
    ~TcpServerMonitor();
    void setTcpServer(QTcpServer *tcpServer);

signals:

private:
    QLabel *m_labelSocketStatus;
    QLabel *m_labelServerIP;
    QLabel *m_labelServerPort;
    QLabel *m_labelClientIP;
    QLabel *m_labelClientPort;
    QPushButton *m_pushButtonStart;
    QPushButton *m_pushButtonStop;
    QTextBrowser *m_textBrowserReceivedData;
    QTextEdit *m_textEditSendData;
    QComboBox *m_comboBoxTcpSocket;
    QComboBox *m_comboBoxNumberSend;
    QLineEdit *m_lineEditInterval;
    QTcpServer *m_tcpServer;
    QHostAddress m_hostAddress;
    quint16 m_port;
    QList<QTcpSocket *> m_tcpSockets;
    QMutex m_lock;

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonSendPeriod_clicked();
    void on_pushButtonStopSend_clicked();
    void on_tcpServer_newConnection();
    void on_tcpSocket_receiveData();
};

#endif  // TCPSERVERMONITOR_H
