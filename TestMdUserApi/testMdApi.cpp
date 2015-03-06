// testTraderApi.cpp : 定义控制台应用程序的入口点。
//

#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include "MdSpi.h"

// UserApi对象
CThostFtdcMdApi* pUserApi;

// 配置参数
char FRONT_ADDR[] = "tcp://27.115.78.150:26213";		// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "1007";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "00000797";			// 投资者代码
TThostFtdcPasswordType  PASSWORD = "123456";			// 用户密码
char *ppInstrumentID[] = {"cu1505", "cu1506"};			// 行情订阅列表 WRONG{"cu1205", "cu1206"};
int iInstrumentID = 2;									// 行情订阅数量

// 请求编号
int iRequestID = 0;

void main(void)
{	
	// 初始化UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();

	pUserApi->Join();
//	pUserApi->Release();
}