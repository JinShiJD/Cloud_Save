#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*))
            ,this ,SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if (NULL == pername || NULL == pdu)
    {
        return;
    }
    QString strName = pername;
    for (int i = 0; i < m_tcpSocketList.size(); i++)
    {
        if(strName == m_tcpSocketList.at(i)->getName())
        {
            m_tcpSocketList.at(i)->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }

}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter++)
    {
        if (mysocket == *iter)
        {       //延迟释放空间
            (*iter)->deleteLater();   //服务器在delete掉当前要关闭的socket时服务器总是异常结束啊
            *iter = NULL;             //(添加delete *iter会出现问题，不添加delete
            m_tcpSocketList.erase(iter);  //只是从列表中删除并不会异常结束)的解决方案：
            break;                   //将delete改为deleteLater()，延迟释放空间

        }
    }

    for(int i = 0; i < m_tcpSocketList.size(); i++)
    {
        if(m_tcpSocketList.at(i)) //确保没有空指针
        {
            qDebug() << m_tcpSocketList.at(i)->getName();
        }
    }

}
