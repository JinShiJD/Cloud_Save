#ifndef SHARFILE_H
#define SHARFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);

    static ShareFile &getInstance();

    void test();

    void updateFriend(QListWidget *pFriendList);

signals:

public slots:
    void cancelSelect();    // 取消全选
    void selectAll();  //全选

    void okShare();
    void cancelShare();


private:
    QPushButton *m_pSelectAllPB; //选择
    QPushButton *m_pCancelSelectPB; // 取消选择

    QPushButton *m_pOKPB; //确定选择
    QPushButton *m_pCancelPB; // 取消选择

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;

};

#endif // SHARFILE_H
