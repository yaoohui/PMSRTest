#include "stdafx.h"
#include "CommunicationProtocol.h"


#define CRC_LENGTH 1
#define FIX_HEADER_LENGTH
//static ProtocolInvoke* s_d1Instance = 0;
static CommunicationProtocol *s_d1Instance = 0;
static void(*s_D1Inovke_cmdProcess)(PHeader*, unsigned char*, unsigned int) = NULL;


CommunicationProtocol::CommunicationProtocol()
{
}


CommunicationProtocol::~CommunicationProtocol()
{
	//delete s_d1Instance;
}


// 通信协议初始化
void CommunicationProtocol::Init(CommunicationProtocol*  d1Invoke, void(*cmdProcess)(PHeader*, unsigned char*, unsigned int))
{
	s_d1Instance = d1Invoke;
	s_D1Inovke_cmdProcess = cmdProcess;
}


// 通信协议验证
bool CommunicationProtocol::IsValidProtocol(unsigned char * pdata, unsigned int length)
{
	bool result = false;
	unsigned char crc;
	unsigned short len;

	if (length < sizeof(PHeader) + CRC_LENGTH)	// 长度不足
		return result;

	len = ((unsigned short)pdata[4] << 8) + pdata[5];
	if (len + CRC_LENGTH + sizeof(PHeader) != length)	// 长度错误
		return result;

	crc = XOR(pdata, length - CRC_LENGTH);
	if (crc != pdata[length - 1])
	{
		result = false;
		return result;
	}
	s_d1Instance->rcvHeader = (PHeader*)pdata;
	s_d1Instance->payload = pdata + sizeof(PHeader);
	s_d1Instance->payloadLength = len;
	result = true;
	return result;
}


// 通信协议校验并处理
bool CommunicationProtocol::ProtocolProcess(unsigned char* pdata, unsigned int length)
{
	bool result = false;

	result = IsValidProtocol(pdata, length);
	if (!result)
		return result;

	if (s_D1Inovke_cmdProcess != NULL)
	{
		s_D1Inovke_cmdProcess(s_d1Instance->rcvHeader, s_d1Instance->payload, s_d1Instance->payloadLength);
	}
	return 0;
}


void CommunicationProtocol::PackProtocol(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength)
{
	unsigned char crc = 0;

	pheader->len = payloadLength;
	*resultlength = payloadLength + sizeof(PHeader) + CRC_LENGTH;

	memcpy(resultdata, pheader, sizeof(*(pheader)));
	memcpy(resultdata + sizeof(*(pheader)), payload, payloadlength);
	crc = XOR(resultdata, *resultlength - CRC_LENGTH);
	*(resultdata + *resultlength - CRC_LENGTH) = crc;
}

void CommunicationProtocol::PackResponse(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength)
{
	if (pheader->flag == 0x99)
	{
		pheader->flag = 0;
	}
	PackProtocol(pheader, payload, payloadLength, resultdata, resultlength);
}

void CommunicationProtocol::HeaderInit(PHeader * pheader)
{
	pheader->prefix = 0xAA;
	pheader->flag = 0x99;
	pheader->modeid = MODEID;
	pheader->cmd = 0;
	pheader->len = 0;
}

//
// 异或运算
//
unsigned char CommunicationProtocol::XOR(unsigned char *pdata, unsigned int length)
{
	unsigned int i=0;
	unsigned char result = 0;

	while (i < length)
	{
		result ^= pdata[i];
		i++;
	}
	return result;
}
