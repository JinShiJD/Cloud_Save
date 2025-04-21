#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qdebug.h>

typedef unsigned int uint;

#define REGIST_OK "Regist ok"
#define REGIST_FAILED  "Regist failed: name has existed"


#define LOGIN_OK "Login ok"
#define LOGIN_FAILED  "Login failed: name error or pwd error or relogin"

#define SEARCH_USR_NO "No this person"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend has existed"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NO_EXIST "usr not exist"

#define DEL_FRIEND_OK "delete friend ok"

#define DIR_NO_EXIST "current dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir ok"

#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILED "delete dir failed: is a regular file"

#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILED "rename file failed"

#define ENTER_DIR_FAILED "enter dir failed: is a regular file"

#define DEL_FILE_OK "delete file ok"
#define DEL_FILE_FAILED "delete file failed: is a director"

#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILED "upload file failed"

#define SHARE_FILE_OK "share file ok"

#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILED "move file failed: is a regular file"

#define COMMON_ERR "operate failed: system is busy"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_MIN = 0,
    ENUN_MSG_TYPE_REGIST_REQUEST,      //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,      //注册回复

    ENUN_MSG_TYPE_LOGIN_REQUEST,      //注册请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,      //注册回复

    ENUN_MSG_TYPE_ALL_ONLINE_REQUEST,   //在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   //在线用户回复

    ENUN_MSG_TYPE_SEARCH_USR_REQUEST,   //搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,   //搜索用户回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   //加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   //加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,   //同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,   //拒绝添加好友

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,   //刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,   //刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,   //删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,   //删除好友回复

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,   //私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,   //私聊回复

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,   //群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,   //群聊回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,   //创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,   //创建文件夹回复

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,   //刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,   //刷新文件回复

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,   //删除目录请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,   //删除目录回复

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,   //重命名文件请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,   //重命名文件回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,   //进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,   //进入文件夹回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,   //上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,  //上传文件回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,   //删除常规文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,  //删除常规文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,   //下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,  //下载文件回复

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,   //共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,  //共享文件回复

    ENUM_MSG_TYPE_SHARE_FILE_NOTE,  //共享文件通知
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,  //共享文件通知回复

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,   //移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,  //移动文件回复

    ENUM_MSG_TYPE_MAX = 0x00ffffff


};

struct FileInfo
{
    char caFileName[64];    //文件名
    int iFileType;          //文件类型
};


struct PDU
{
    uint uiPDULen;      //总的协议数据大小
    uint uiMsgType;     //消息类型
    char caData[64];
    uint uiMsgLen;      //实际消息长度
    char caMsg[];        //实际消息

};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
