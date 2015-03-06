#include "MdSpi.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API参数
extern CThostFtdcMdApi* pUserApi;

// 配置参数
extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;

// 请求编号
extern int iRequestID;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

//1) 建立连接后，来到这里登陆
void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//2) 登陆后，来到获取基础数据并订阅行情
void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cerr << "基础数据: F" << pRspUserLogin->FrontID << " S"
			<< pRspUserLogin->SessionID << " M"
			<< pRspUserLogin->MaxOrderRef << " "
			<< pRspUserLogin->SystemName << " "
			<< pRspUserLogin->TradingDay << " "
			<< pRspUserLogin->SHFETime << " "
			<< pRspUserLogin->FFEXTime << " "
			<< endl;
		///获取当前交易日, pUserApi是全局变量
		//cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();	
		// 请求订阅询价,只能订阅郑商所的询价，其他交易所通过traderapi相应接口返回
		SubscribeForQuoteRsp();	
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::SubscribeForQuoteRsp()
{
	int iResult = pUserApi->SubscribeForQuoteRsp(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送询价订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

//3) 订阅行情后，来到这里检查状态
void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubMarketData with Error " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
}

//5) 订阅询价后，来到这里检查状态
void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubForQuoteRsp" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubForQuoteRsp" << endl;
}

//4) 订阅行情后，来到这里收行情
void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cerr << "[" << pDepthMarketData->UpdateTime << "]" << "-----" << pDepthMarketData->InstrumentID << "----" << endl;
	cerr << "LastPrice: " << pDepthMarketData->LastPrice << endl;
	cerr << "AskPrice1: " << pDepthMarketData->AskPrice1 << "  AskVolume1: " << pDepthMarketData->AskVolume1 << endl;
	cerr << "BidPrice1: " << pDepthMarketData->BidPrice1 << "  BidVolume1: " << pDepthMarketData->BidVolume1 << endl;
	cerr << "------------------------------------" << endl;
}

//6) 订阅询价后，来到这里收询价
void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	cerr << "投资者询价：" << pForQuoteRsp->InstrumentID << " " <<  pForQuoteRsp->ForQuoteSysID << endl; 
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}