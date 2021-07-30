#include "mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setMinimumSize(1600, 900);

    // Menubar
    this->setMenuBar(new QMenuBar);

    QMenu *menuCreate = this->menuBar()->addMenu(tr("Create Socket"));
    QAction *actionDelete = this->menuBar()->addAction(tr("Delete Socket"));

    QAction *actionTcpServer = menuCreate->addAction(tr("TCP Server"));
    QAction *actionTcpClient = menuCreate->addAction(tr("TCP Client"));
    QAction *actionUdpSocket = menuCreate->addAction(tr("UDP Socket"));
    QAction *actionUdpGroup = menuCreate->addAction(tr("UDP Group"));

    connect(actionTcpServer, &QAction::triggered, this,
            &MyMainWindow::on_actionTcpServer_triggered);
    connect(actionTcpClient, &QAction::triggered, this,
            &MyMainWindow::on_actionTcpClient_triggered);
    connect(actionUdpSocket, &QAction::triggered, this,
            &MyMainWindow::on_actionUdpSocket_triggered);
    connect(actionUdpGroup, &QAction::triggered, this,
            &MyMainWindow::on_actionUdpGroup_triggered);
    connect(actionDelete, &QAction::triggered, this,
            &MyMainWindow::on_actionDelete_triggered);

    // Centralwidget
    this->setCentralWidget(new QWidget);

    m_treeWidget = new QTreeWidget;
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setRootIsDecorated(false);
    m_stackedWidget = new QStackedWidget;

    m_treeWidget->setFixedWidth(600);
    m_treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_treeWidget->header()->setStretchLastSection(false);
    m_stackedWidget->setMinimumWidth(1000);
    connect(m_treeWidget, &QTreeWidget::currentItemChanged, this,
            &MyMainWindow::on_treeWidget_currentItemChanged);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(m_treeWidget);
    layout->addWidget(m_stackedWidget);

    this->centralWidget()->setLayout(layout);
}

MyMainWindow::~MyMainWindow() {}

void MyMainWindow::on_actionTcpServer_triggered()
{
    TcpServerWidget *w = new TcpServerWidget(this);
    connect(w, &TcpServerWidget::tcpServerCreated, this,
            &MyMainWindow::on_tcpServerCreated);
    w->show();
}

void MyMainWindow::on_actionTcpClient_triggered()
{
    TcpSocketWidget *w = new TcpSocketWidget(this);
    connect(w, &TcpSocketWidget::tcpSocketCreated, this,
            &MyMainWindow::on_tcpSocketCreated);
    w->show();
}

void MyMainWindow::on_actionUdpSocket_triggered()
{
    UdpSocketWidget *w = new UdpSocketWidget(this);
    connect(w, &UdpSocketWidget::udpSocketCreated, this,
            &MyMainWindow::on_udpSocketCreated);
    w->show();
}

void MyMainWindow::on_actionUdpGroup_triggered()
{
    UdpGroupWidget *w = new UdpGroupWidget(this);
    connect(w, &UdpGroupWidget::udpMulticastCreated, this,
            &MyMainWindow::on_udpMulticastCreated);
    w->show();
}

void MyMainWindow::on_actionDelete_triggered()
{
    QTreeWidgetItem *current = m_treeWidget->currentItem();
    if (current != nullptr)
    {
        QWidget *w = current->data(0, Qt::UserRole).value<QWidget *>();
        if (w != nullptr)
        {
            QTreeWidgetItem *parent = current->parent();
            int index;
            if (parent)
            {
                index = parent->indexOfChild(current);
                delete parent->takeChild(index);
            }
            else
            {
                index = m_treeWidget->indexOfTopLevelItem(current);
                delete m_treeWidget->takeTopLevelItem(index);
            }
            m_stackedWidget->removeWidget(w);
            w->deleteLater();
        }
    }
}

void MyMainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current)
{
    if (current != nullptr)
    {
        QWidget *w = current->data(0, Qt::UserRole).value<QWidget *>();
        if (w != nullptr)
        {
            m_stackedWidget->setCurrentWidget(w);
        }
    }
    else
    {
        m_stackedWidget->setCurrentIndex(0);
    }
}

void MyMainWindow::on_tcpServerCreated(QTcpServer *tcpServer)
{
    QString tab = QString("[TCP Server]-%1:%2")
                      .arg(tcpServer->serverAddress().toString())
                      .arg(tcpServer->serverPort());
    TcpServerMonitor *w = new TcpServerMonitor(this);
    w->setTcpServer(tcpServer);
    m_stackedWidget->addWidget(w);
    connect(w, &TcpServerMonitor::tcpSocketConnected, this,
            &MyMainWindow::on_tcpSocketConnected);
    QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, tab);
    item->setData(0, Qt::UserRole, QVariant::fromValue(w));
    m_treeWidget->setCurrentItem(item);
}

void MyMainWindow::on_tcpSocketCreated(QTcpSocket *tcpSocket,
                                       QHostAddress hostAddress, quint16 port)
{
    QString tab =
        QString("[TCP Socket]-%1:%2").arg(hostAddress.toString()).arg(port);
    TcpSocketMonitor *w = new TcpSocketMonitor(this);
    w->setTcpSocket(tcpSocket, hostAddress, port);
    m_stackedWidget->addWidget(w);
    QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, tab);
    item->setData(0, Qt::UserRole, QVariant::fromValue(w));
    m_treeWidget->setCurrentItem(item);
}

void MyMainWindow::on_tcpSocketConnected(QTcpSocket *tcpSocket,
                                         QHostAddress hostAddress, quint16 port)
{
    QString tab =
        QString("[TCP Socket]-%1:%2").arg(hostAddress.toString()).arg(port);
    TcpSocketMonitor *w = new TcpSocketMonitor(this);
    w->setTcpSocket(tcpSocket, hostAddress, port);
    m_stackedWidget->addWidget(w);
    TcpServerMonitor *monitor = qobject_cast<TcpServerMonitor *>(sender());
    connect(w, &TcpSocketMonitor::tcpSocketReceivedData, monitor,
            &TcpServerMonitor::on_tcpSocket_receiveData);
    int size = m_treeWidget->topLevelItemCount();
    QTreeWidgetItem *parent;
    QTreeWidgetItem *item;
    for (int i = 0; i < size; ++i)
    {
        parent = m_treeWidget->topLevelItem(i);
        if (monitor == parent->data(0, Qt::UserRole).value<QObject *>())
        {
            item = new QTreeWidgetItem(parent);
            item->setText(0, tab);
            item->setData(0, Qt::UserRole, QVariant::fromValue(w));
            m_treeWidget->setCurrentItem(item);
            break;
        }
    }
}

void MyMainWindow::on_udpSocketCreated(QUdpSocket *udpSocket)
{
    QString tab = QString("[UDP Socket]-%1:%2")
                      .arg(udpSocket->localAddress().toString())
                      .arg(udpSocket->localPort());
    UdpSocketMonitor *w = new UdpSocketMonitor(this);
    w->setUdpSocket(udpSocket);
    m_stackedWidget->addWidget(w);
    QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, tab);
    item->setData(0, Qt::UserRole, QVariant::fromValue(w));
    m_treeWidget->setCurrentItem(item);
}

void MyMainWindow::on_udpMulticastCreated(QUdpSocket *udpSocket,
                                          QString multicastIP)
{
    QString tab = QString("[UDP Socket]-%1:%2(%3)")
                      .arg(udpSocket->localAddress().toString())
                      .arg(udpSocket->localPort())
                      .arg(multicastIP);
    UdpSocketMonitor *w = new UdpSocketMonitor(this);
    w->setUdpMulticast(udpSocket, multicastIP);
    m_stackedWidget->addWidget(w);
    QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, tab);
    item->setData(0, Qt::UserRole, QVariant::fromValue(w));
    m_treeWidget->setCurrentItem(item);
}
