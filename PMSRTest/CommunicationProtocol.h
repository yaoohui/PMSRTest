#pragma once

typedef struct typeHeader
{
	/*!\brief 协议头 */
	unsigned char prefix;
	/*!\brief 发送方标志 */
	unsigned char flag;
	/*!\brief 模块编号 */
	unsigned char modeid;
	/*!\brief 指令码 */
	unsigned char cmd;
	/*!\brief 数据长度 */
	unsigned short len;

}PHeader;

//typedef struct typeProtocolInvoke
//{
//	/*!
//	\brief	D1检查函数,是否为正确的D1协议，如果是，则更新下面的各数据指针。同时更新本结构全中的成员变量
//	\details
//	\return  bool true/false 是有效的D1协议/不是有效在D1协议
//	\param unsigned char* inputData 输入数据
//	\param unsigned short datalength  数据长度
//	*/
//	bool(*IsValidProtocol)(unsigned char * pdata, unsigned int length);
//
//	//D1数据解析函数。
//	bool(*ProtocolProcess)(unsigned char* pdata, unsigned int length);
//
//	/*!
//	\brief	将当前D1数据的内容打包为字节流
//	\details
//	\return  void
//	\param D1Hheader* rcvHeader  输入的协议头
//	\param unsigned char payload[] 输入有效数据
//	\param unsigned short payloadLength 有效数据长度
//	\param unsigned char* result 打包后的数据
//	param  unsigned short*  resultLength 打包后的数据长度
//	*/
//	void(*PackProtocol)(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
//
//
//	/*!
//	\brief	发送反馈(如果result标识为0xFF，则将result标识为0)
//	\details
//	\return  void
//	\param D1Hheader* rcvHeader  输入的协议头
//	\param unsigned char payload[] 输入有效数据
//	\param unsigned short payloadLength 有效数据长度
//	\param unsigned char* result 打包后的数据
//	param  unsigned short*  resultLength 打包后的数据长度
//	*/
//	void(*PackResponse)(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
//
//	/*!\brief D1协议头 */
//	PHeader* rcvHeader;           //D1
//	/*!\brief 有效数据 */
//	unsigned char* payload;         //有效数据        
//	/*!\brief 有效数据长度 */
//	unsigned short payloadLength;   //有效数据长度  
//
//}ProtocolInvoke;

class CommunicationProtocol
{
public:
	CommunicationProtocol();
	~CommunicationProtocol();
	// 通信协议初始化
	void Init(CommunicationProtocol*  d1Invoke, void(*cmdProcess)(PHeader*, unsigned char*, unsigned int));
	// 通信协议校验并处理
	bool ProtocolProcess(unsigned char* pdata, unsigned int length);

	// 通信协议验证
	bool IsValidProtocol(unsigned char * pdata, unsigned int length);

public:
	void PackProtocol(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
	void PackResponse(PHeader * pheader, unsigned char * payload, unsigned int payloadlength, unsigned char * resultdata, unsigned int * resultlength);
	unsigned char XOR(unsigned char *pdata, unsigned int length);

	/*!\brief D1协议头 */
	PHeader* rcvHeader;           //D1
	/*!\brief 有效数据 */
	unsigned char* payload;         //有效数据        
	/*!\brief 有效数据长度 */
	unsigned short payloadLength;   //有效数据长度  
private:
	void HeaderInit(PHeader * pheader);
};


