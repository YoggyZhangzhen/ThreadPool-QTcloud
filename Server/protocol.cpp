#include "protocol.h"
#include "stdlib.h"

PDU *mkPDU(uint uiMsgType, uint uiMsgLen)
{
    PDU* pdu = (PDU*)malloc(uiMsgLen + sizeof(PDU));
    if (pdu == NULL)
    {
        exit(1);
    }
    pdu->uiMsgLen = uiMsgLen;
    pdu->uiPDULen = uiMsgLen + sizeof(PDU);
    pdu->uiMsgType = uiMsgType;
    return pdu;
}
