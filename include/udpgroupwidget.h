#ifndef UDPGROUPWIDGET_H
#define UDPGROUPWIDGET_H

#include <QWidget>

#include "stdafx.h"

class UdpGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UdpGroupWidget(QWidget *parent = nullptr);

signals:
    void udpMulticastCreated(QUdpSocket *udpSocket, QString multicastIP);

private:
    QComboBox *m_comboBoxIP;
    QSpinBox *m_spinBoxPort;
    QLineEdit *m_lineEditMulticastIP;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
};

#endif  // UDPGROUPWIDGET_H
