#pragma once

typedef struct typeHeader
{
	/*!\brief Э��ͷ */
	unsigned char prefix;
	/*!\brief ���ͷ���־ */
	unsigned char flag;
	/*!\brief ģ���� */
	unsigned char modeid;
	/*!\brief ָ���� */
	unsigned char cmd;
	/*!\brief ���ݳ��� */
	unsigned short len;

}PHeader;

//typedef struct typeProtocolInvoke
//{
//	/*!
//	\brief	D1��麯��,�Ƿ�Ϊ��ȷ��D1Э�飬����ǣ����������ĸ�����ָ�롣ͬʱ���±��ṹȫ�еĳ�Ա����
//	\details
//	\return  bool true/false ����Ч��D1Э��/������Ч��D1Э��
//	\param unsigned char* inputData ��������
//	\param unsigned short datalength  ���ݳ���
//	*/
//	bool(*IsValidProtocol)(unsigned char * pdata, unsigned int length);
//
//	//D1���ݽ���������
//	bool(*ProtocolProcess)(unsigned char* pdata, unsigned int length);
//
//	/*!
//	\brief	����ǰD1���ݵ����ݴ��Ϊ�ֽ���
//	\details
//	\return  void
//	\param D1Hheader* rcvHeader  �����Э��ͷ
//	\param unsigned char payload[] ������Ч����
//	\param unsigned short payloadLength ��Ч���ݳ���
//	\param unsigned char* result ����������
//	param  unsigned short*  resultLength ���������ݳ���
//	*/
//	void(*PackProtocol)(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
//
//
//	/*!
//	\brief	���ͷ���(���result��ʶΪ0xFF����result��ʶΪ0)
//	\details
//	\return  void
//	\param D1Hheader* rcvHeader  �����Э��ͷ
//	\param unsigned char payload[] ������Ч����
//	\param unsigned short payloadLength ��Ч���ݳ���
//	\param unsigned char* result ����������
//	param  unsigned short*  resultLength ���������ݳ���
//	*/
//	void(*PackResponse)(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
//
//	/*!\brief D1Э��ͷ */
//	PHeader* rcvHeader;           //D1
//	/*!\brief ��Ч���� */
//	unsigned char* payload;         //��Ч����        
//	/*!\brief ��Ч���ݳ��� */
//	unsigned short payloadLength;   //��Ч���ݳ���  
//
//}ProtocolInvoke;

class CommunicationProtocol
{
public:
	CommunicationProtocol();
	~CommunicationProtocol();
	// ͨ��Э���ʼ��
	void Init(CommunicationProtocol*  d1Invoke, void(*cmdProcess)(PHeader*, unsigned char*, unsigned int));
	// ͨ��Э��У�鲢����
	bool ProtocolProcess(unsigned char* pdata, unsigned int length);

	// ͨ��Э����֤
	bool IsValidProtocol(unsigned char * pdata, unsigned int length);

public:
	void PackProtocol(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
	void PackResponse(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
	unsigned char XOR(unsigned char *pdata, unsigned int length);

	/*!\brief D1Э��ͷ */
	PHeader* rcvHeader;           //D1
	/*!\brief ��Ч���� */
	unsigned char* payload;         //��Ч����        
	/*!\brief ��Ч���ݳ��� */
	unsigned short payloadLength;   //��Ч���ݳ���  
private:
	void HeaderInit(PHeader * pheader);
};


