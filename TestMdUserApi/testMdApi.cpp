// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include "MdSpi.h"

// UserApi����
CThostFtdcMdApi* pUserApi;

// ���ò���
char FRONT_ADDR[] = "tcp://27.115.78.150:26213";		// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "1007";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "00000797";			// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "123456";			// �û�����
char *ppInstrumentID[] = {"cu1505", "cu1506"};			// ���鶩���б� WRONG{"cu1205", "cu1206"};
int iInstrumentID = 2;									// ���鶩������

// ������
int iRequestID = 0;

void main(void)
{	
	// ��ʼ��UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						// ע���¼���
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();

	pUserApi->Join();
//	pUserApi->Release();
}