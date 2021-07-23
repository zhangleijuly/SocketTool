#ifndef TCPSERVERMONITOR_H
#define TCPSERVERMONITOR_H

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

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
    QComboBox *m_comboBoxTcpSocket;
    QTcpServer *m_tcpServer;
    QHostAddress m_hostAddress;
    quint16 m_port;
    QList<QTcpSocket *> m_tcpSockets;

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonSendPeriod_clicked();
    void on_pushButtonStopSend_clicked();
    void on_tcpServer_newConnection();
};

#endif  // TCPSERVERMONITOR_H
