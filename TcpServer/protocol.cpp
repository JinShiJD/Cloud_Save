#include "protocol.h"

PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;
    PDU *pdu = (PDU*)malloc(uiPDULen);
    if (NULL == pdu)
    {
        qDebug() << "分配内存失败" ;
        exit(EXIT_FAILURE);
    }
    memset(pdu, 0, uiPDULen);
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
