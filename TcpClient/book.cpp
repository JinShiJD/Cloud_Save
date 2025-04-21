#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "opewidget.h"
#include "sharefile.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    m_strEnterDir.clear();

    m_bDownload = false;
    m_pTimer = new QTimer;

    m_pBookListW = new QListWidget;
    m_pReturnPB = new  QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownLoadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool))
            , this, SLOT(createDir()));
    connect(m_pFlushFilePB, SIGNAL(clicked(bool))
            , this, SLOT(flushFile()));
    connect(m_pDelDirPB, SIGNAL(clicked(bool))
            , this, SLOT(delDir()));
    connect(m_pRenamePB, SIGNAL(clicked(bool))
            , this, SLOT(renameFile()));
    connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex))
            , this, SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB, SIGNAL(clicked(bool))
            , this, SLOT(returnPre()));
    connect(m_pUploadPB, SIGNAL(clicked(bool))
            , this, SLOT(uploadFile()));
    connect(m_pTimer, SIGNAL(timeout())
            , this, SLOT(uploadFileData()));  //避免内容和请求同时发生，有冲突
    connect(m_pDelFilePB, SIGNAL(clicked(bool))
            ,this, SLOT(delRegFile()));
    connect(m_pDownLoadPB, SIGNAL(clicked(bool))
            , this, SLOT(downloadFile()));
    connect(m_pShareFilePB, SIGNAL(clicked(bool))
            , this, SLOT(shareFile()));
    connect(m_pMoveFilePB, SIGNAL(clicked(bool))
            , this, SLOT(moveFile()));
    connect(m_pSelectDirPB, SIGNAL(clicked(bool))
            , this, SLOT(selectDestDir()));
}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }

    QListWidgetItem *pItemTmp = NULL;
    int row = m_pBookListW->count();
    while(row > 0)
    {
        pItemTmp = m_pBookListW->item(row - 1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row - 1;
    }

    FileInfo *pFileInfo = NULL;
    int iCount = pdu->uiMsgLen/sizeof (FileInfo);
    for(int i = 0; i < iCount; i ++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug() << pFileInfo->caFileName << pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.png")));
        }
        else if(1 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":map/file.png")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

void Book::setDownloadStatus(bool status)
{
    m_bDownload = status;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this, "新文件夹", "新文件夹名字");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size() > 32)
        {
            QMessageBox::warning(this, "新建文件夹", "新文件夹名不能超过32个字符");
        }
        else
        {
            QString strName = TcpClient::getInstance().loginName();
            QString strCurPath = TcpClient::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }

    }
    else
    {
        QMessageBox::warning(this, "新建文件夹", "新文件夹名不能为空");
    }
}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg), strCurPath.toUtf8().toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData, strDelName.toUtf8().toStdString().c_str(), strDelName.size());
        memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size()); //路径

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "重命名文件", "请选择要重命名的文件");
    }
    else
    {
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this, "重命名文件", "请输入新的文件名");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData, strOldName.toUtf8().toStdString().c_str(), strOldName.size());
            strncpy(pdu->caData + 32, strNewName.toUtf8().toStdString().c_str(), strNewName.size());
            memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this, "重命名文件", "文件名不能为空");

        }
    }

}

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
    m_strEnterDir = strDirName;
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData, strDirName.toUtf8().toStdString().c_str(), strDirName.size());
    memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strRootPath = "./" + TcpClient::getInstance().loginName();
    qDebug() << strCurPath <<strRootPath;
    if(strCurPath == strRootPath)
    {
        QMessageBox::warning(this, "返回", "返回失败: 已经处于最开始的文件夹目录中");
    }
    else    //返回上一级目录
    {   
        int index = strCurPath.lastIndexOf('/');
        strCurPath.remove(index, strCurPath.size() - index);
        qDebug() << "return -->" << strCurPath;
        TcpClient::getInstance().setCurPath(strCurPath);

        clearEnterDir();

        flushFile();
    }
}

void Book::uploadFile()
{

        m_strUploadFilePath = QFileDialog::getOpenFileName(); //打开上传文件的窗口
        qDebug() << m_strUploadFilePath;

        if(!m_strUploadFilePath.isEmpty())
        {
            // aa/bb/cc last==>从后往前找
            int index = m_strUploadFilePath.lastIndexOf('/');
            QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
            qDebug() << strFileName;

            QFile file(m_strUploadFilePath);
            qint64 fileSize = file.size(); //获得文件大小

            QString strCurPath = TcpClient::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
            pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
            memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size());
            sprintf(pdu->caData, "%s %lld", strFileName.toUtf8().toStdString().c_str(), fileSize);

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;

            m_pTimer->start(1000);
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传文件不能为空");
        }
}

void Book::delRegFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData, strDelName.toUtf8().toStdString().c_str(), strDelName.size());
        memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size()); //路径

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "上传文件", "上传文件失败");
        return;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true)
    {
        ret = file.read(pBuffer, 4096);
        if(ret > 0 && ret <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if(0 == ret)
        {
             break;
        }
        else //既不大于0也不小于0
        {
            QMessageBox::warning(this, "上传文件", "上传文件出错: 读取文件失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;

}

void Book::downloadFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
    }
    else
    {

        QString strSaveFilePath = QFileDialog::getSaveFileName(); //弹出窗口，选择路径
        if(strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this, "下载文件", "请指定要保存的位置");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
            //m_bDownload = true;
        }

        QString strCurPath = TcpClient::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData, strFileName.toUtf8().toStdString().c_str());
        memcpy(pdu->caMsg, strCurPath.toUtf8().toStdString().c_str(), strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::shareFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }
    else
    {
        m_strShareFileName = pItem->text();
    }

    Friend *pFriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();

    ShareFile::getInstance().updateFriend(pFriendList);
    if(ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }


}

void Book::moveFile()
{
    QListWidgetItem *pCurItem =  m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        qDebug() << "移动的文件为： " << m_strMoveFileName;
        QString strCurPath =  TcpClient::getInstance().curPath();
        m_strMoveFilePath = strCurPath + '/' + m_strMoveFileName;
        qDebug() << "源文件完整路径: " << m_strMoveFilePath;
        m_pSelectDirPB->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "01请选择要移动的文件");
    }
    //m_pSelectDirPB->setEnabled(false);
}

void Book::selectDestDir()
{

    QListWidgetItem *pCurItem =  m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();
        QString strCurPath =  TcpClient::getInstance().curPath();
        m_strDestDir = strCurPath + '/' + strDestDir;

        int srcLen = m_strMoveFilePath.size();
        int destLen = m_strDestDir.size();
        PDU *pdu = mkPDU(srcLen + destLen + 2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
//        QString strFileName = QFileInfo(m_strMoveFilePath).fileName();
        sprintf(pdu->caData, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());

        memcpy(pdu->caMsg, m_strMoveFilePath.toUtf8().toStdString().c_str(), srcLen);
        memcpy((pdu->caMsg) + srcLen, m_strDestDir.toUtf8().toStdString().c_str(), destLen);

        qDebug() << "目标路径： " << (pdu->caMsg) + srcLen;

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {

        QMessageBox::warning(this, "移动文件", "02请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);
}















