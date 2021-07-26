#include "mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Menubar
    this->setMenuBar(new QMenuBar);

    QMenu *menuCreate = this->menuBar()->addMenu("Create Socket");
    QAction *actionDelete = this->menuBar()->addAction("Delete Socket");

    QAction *actionTcpServer = menuCreate->addAction("TCP Server");
    QAction *actionTcpClient = menuCreate->addAction("TCP Client");
    QAction *actionUdpSocket = menuCreate->addAction("UDP Socket");
    QAction *actionUdpGroup = menuCreate->addAction("UDP Group");

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

    m_listWidget = new QListWidget;
    m_stackedWidget = new QStackedWidget;

    m_listWidget->setMinimumWidth(200);
    m_listWidget->setMaximumWidth(400);
    m_stackedWidget->setMinimumWidth(600);
    connect(m_listWidget, &QListWidget::currentItemChanged, this,
            &MyMainWindow::on_listWidget_currentItemChanged);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(m_listWidget);
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
    w->show();
}

void MyMainWindow::on_actionUdpSocket_triggered() {}

void MyMainWindow::on_actionUdpGroup_triggered() {}

void MyMainWindow::on_actionDelete_triggered()
{
    QListWidgetItem *current = m_listWidget->currentItem();
    if (current != nullptr)
    {
        QWidget *w = current->data(Qt::UserRole).value<QWidget *>();
        if (w != nullptr)
        {
            m_listWidget->removeItemWidget(current);
            delete current;
            m_stackedWidget->removeWidget(w);
            w->deleteLater();
        }
    }
}

void MyMainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current)
{
    if (current != nullptr)
    {
        QWidget *w = current->data(Qt::UserRole).value<QWidget *>();
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
    QListWidgetItem *item = new QListWidgetItem(tab, m_listWidget);
    item->setData(Qt::UserRole, QVariant::fromValue(w));
    m_listWidget->addItem(item);
    m_listWidget->setCurrentItem(item);
}
