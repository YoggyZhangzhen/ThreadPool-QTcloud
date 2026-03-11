#include "protocol.h"
#include "stdlib.h"
#include "string.h"

PDU *mkPDU(uint uiMsgType, uint uiMsgLen)
{
    uint uiPDULen = uiMsgLen + sizeof(PDU);
    PDU* pdu = (PDU*)malloc(uiPDULen);
    if (pdu == NULL)
    {
        exit(1);
    }
    memset(pdu, 0, uiPDULen);
    pdu->uiMsgLen = uiMsgLen;
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgType = uiMsgType;
    return pdu;
}
