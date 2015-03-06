#include "MdSpi.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API����
extern CThostFtdcMdApi* pUserApi;

// ���ò���
extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;

// ������
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

//1) �������Ӻ����������½
void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///�û���¼����
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
	cerr << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}
//2) ��½��������ȡ�������ݲ���������
void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cerr << "��������: F" << pRspUserLogin->FrontID << " S"
			<< pRspUserLogin->SessionID << " M"
			<< pRspUserLogin->MaxOrderRef << " "
			<< pRspUserLogin->SystemName << " "
			<< pRspUserLogin->TradingDay << " "
			<< pRspUserLogin->SHFETime << " "
			<< pRspUserLogin->FFEXTime << " "
			<< endl;
		///��ȡ��ǰ������, pUserApi��ȫ�ֱ���
		//cerr << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
		// ����������
		SubscribeMarketData();	
		// ������ѯ��,ֻ�ܶ���֣������ѯ�ۣ�����������ͨ��traderapi��Ӧ�ӿڷ���
		SubscribeForQuoteRsp();	
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> �������鶩������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CMdSpi::SubscribeForQuoteRsp()
{
	int iResult = pUserApi->SubscribeForQuoteRsp(ppInstrumentID, iInstrumentID);
	cerr << "--->>> ����ѯ�۶�������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

//3) �������������������״̬
void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubMarketData with Error " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
}

//5) ����ѯ�ۺ�����������״̬
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

//4) �����������������������
void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cerr << "[" << pDepthMarketData->UpdateTime << "]" << "-----" << pDepthMarketData->InstrumentID << "----" << endl;
	cerr << "LastPrice: " << pDepthMarketData->LastPrice << endl;
	cerr << "AskPrice1: " << pDepthMarketData->AskPrice1 << "  AskVolume1: " << pDepthMarketData->AskVolume1 << endl;
	cerr << "BidPrice1: " << pDepthMarketData->BidPrice1 << "  BidVolume1: " << pDepthMarketData->BidVolume1 << endl;
	cerr << "------------------------------------" << endl;
}

//6) ����ѯ�ۺ�����������ѯ��
void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	cerr << "Ͷ����ѯ�ۣ�" << pForQuoteRsp->InstrumentID << " " <<  pForQuoteRsp->ForQuoteSysID << endl; 
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}