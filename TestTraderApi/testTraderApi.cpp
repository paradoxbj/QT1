// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include "TraderSpi.h"

// UserApi����
CThostFtdcTraderApi* pUserApi;

// ���ò���
char  FRONT_ADDR[] = "tcp://27.115.78.150:26205";		// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "1007";				// ���͹�˾����  2030
TThostFtdcInvestorIDType INVESTOR_ID = "00000797";			// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "123456";			// �û�����
TThostFtdcInstrumentIDType INSTRUMENT_ID = "cu1508";	// ��Լ���� WRONG"cu1408"
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Sell;	// ��������
TThostFtdcPriceType	LIMIT_PRICE = 42310; // 38850;				// �۸�

// ������
int iRequestID = 0;

void main(void)
{
	// ��ʼ��UserApi
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
	CTraderSpi* pUserSpi = new CTraderSpi();
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// ע���¼���
	pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ�������Ƿ�Ӧ����RESUME
	pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽�������Ƿ�Ӧ����RESUME
	pUserApi->RegisterFront(FRONT_ADDR);							// connect
	pUserApi->Init();

	pUserApi->Join();
//	pUserApi->Release();
}