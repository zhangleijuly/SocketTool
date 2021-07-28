#ifndef UDPSOCKETWIDGET_H
#define UDPSOCKETWIDGET_H

#include <QWidget>

#include "stdafx.h"

class UdpSocketWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UdpSocketWidget(QWidget *parent = nullptr);

signals:
    void udpSocketCreated(QUdpSocket *udpSocket);

private:
    QComboBox *m_comboBoxIP;
    QSpinBox *m_spinBoxPort;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
};

#endif  // UDPSOCKETWIDGET_H
