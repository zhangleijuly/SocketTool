#ifndef UDPSOCKETMONITOR_H
#define UDPSOCKETMONITOR_H

#include <QWidget>

#include "stdafx.h"

class UdpSocketMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit UdpSocketMonitor(QWidget *parent = nullptr);

signals:

private:
    QUdpSocket *m_udpSocket;
};

#endif  // UDPSOCKETMONITOR_H
