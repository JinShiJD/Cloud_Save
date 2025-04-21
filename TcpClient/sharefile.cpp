#include "sharefile.h"

#include <qcheckbox.h>
#include "tcpclient.h"
#include "opewidget.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancelSelectPB = new QPushButton("取消选择");

    m_pOKPB = new QPushButton("确定");
    m_pCancelPB = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendW = new QWidget;
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);
    m_pButtonGroup->setExclusive(false);       //可多选

    QHBoxLayout *pTomHBL = new QHBoxLayout;
    pTomHBL->addWidget(m_pSelectAllPB);
    pTomHBL->addWidget(m_pCancelSelectPB);
    pTomHBL->addStretch();


    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);


    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTomHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);
    setLayout(pMainVBL);

    connect(m_pCancelSelectPB, SIGNAL(clicked(bool)), this, SLOT(cancelSelect()));
    connect(m_pSelectAllPB, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(m_pOKPB, SIGNAL(clicked(bool)), this, SLOT(okShare()));
    connect(m_pCancelPB, SIGNAL(clicked(bool)), this, SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::test()
{
    QVBoxLayout *p = new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB = NULL;
    for(int i = 0; i < 15; i ++)
    {
        pCB = new QCheckBox("Star");
        p->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL == pFriendList)
    {
        return;
    }
    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();
    for(int i = 0; i < preFriendList.size(); i ++)
    {
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }
    QCheckBox *pCB = NULL;
    for(int i = 0; i < pFriendList->count(); i ++)
    {
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancelSelect()
{
     QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
     for(int i = 0; i < cbList.size(); i ++)
     {
         if(cbList[i]->isChecked())
         {
             cbList[i]->setChecked(false);
         }
     }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i = 0; i < cbList.size(); i ++)
    {
        if(!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare()
{
    QString strName = TcpClient::getInstance().loginName();
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();

    QString strPath = strCurPath + "/" + strShareFileName;
    //转换为utf-8字节数组
    QByteArray pathData = strPath.toUtf8();
    qDebug() << "文件分享路径: " << strPath;

    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for(int i = 0; i < cbList.size(); i ++)
    {
        if(cbList[i]->isChecked())
        {
            num ++;
            qDebug() << "分享---" <<  strName << "给->" << cbList[i]->text();
        }
    }

    qDebug() << "num: " << num;
    //消息总长度 = 好友部分 + 路径数据长度
    uint totalMsgLen = 32 * num + pathData.size();
    PDU *pdu = mkPDU(totalMsgLen);//+1
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData, "%s %d", strName.toStdString().c_str(), num);
    int j = 0;
    for(int i = 0; i < cbList.size(); i ++)
    {
        if(cbList[i]->isChecked())
        {
            qDebug() << "选中分享的好友：" << cbList[i]->text().split('\t')[0].toStdString().c_str();
            QByteArray friendData = cbList[i]->text().toUtf8();
            // 复制32字节，确保不溢出
            memcpy((pdu->caMsg) + j*32, friendData.constData(), qMin(friendData.size(),32));
            qDebug() << "01 pdu->caMsg :" << pdu->caMsg;
             ++ j;
        }
    }

    qDebug() << "02 strPath.size() : " << strPath.size();
    qDebug() << "02 num: " << num;

    //strncpy((pdu->caMsg) + num*32, strPath.toStdString().c_str(), strPath.size() + 32);
    memcpy((pdu->caMsg) + num*32, pathData.constData(), pathData.size());
    qDebug() << "02文件分享路径: " << strPath;

    qDebug() << "发送前测试： " << ((pdu->caMsg) + num * 32);

    // 调试：打印路径部分
    qDebug() << "文件路径原始字节: " << pathData.toHex();
    qDebug() << "文件路径解码测试: " << QString::fromUtf8(pathData);

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);

    free(pdu);
    pdu = NULL;
    hide();

}

void ShareFile::cancelShare()
{
    hide();
}
