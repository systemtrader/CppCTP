//
// Created by quant on 6/7/16.
//


#include <iostream>
#include <cstring>
#include "MdSpi.h"

using namespace std;

#define DEBUG
#ifdef DEBUG
#define USER_PRINT(x) std::cout << "[DEBUG] - " << __DATE__ << ", " << __TIME__<< "  " << __FILE__ << ", Line - " << __LINE__ << endl; std::cout << #x << " = " << x << std::endl;
#else
#define USER_PRINT(x)
#endif

const string BROKER_ID = "9999";
const string USER_ID = "058176";
const string PASS = "669822";
struct timespec outtime = {3, 0};


//初始化构造函数
MdSpi::MdSpi(CThostFtdcMdApi *mdapi) {
	sem_init(&connect_sem, 0, 0);
	sem_init(&login_sem, 0, 0);
	sem_init(&logout_sem, 0, 0);
	sem_init(&submarket_sem, 0, 0);
	sem_init(&unsubmarket_sem, 0, 0);
	this->isLogged = false;
	this->isFirstTimeLogged = true;
    this->mdapi = mdapi;
	//注册事件处理对象
	mdapi->RegisterSpi(this);
    this->loginRequestID = 10;
}

//等待线程结束
void MdSpi::Join() {
	USER_PRINT("MdSpi::Join()")
	this->mdapi->Join();
}

//协程控制
int MdSpi::controlTimeOut(sem_t *t, int timeout) {
	/*协程开始*/
	struct timeval now;
	struct timespec outtime;
	gettimeofday(&now, NULL);
	//cout << now.tv_sec << " " << (now.tv_usec) << "\n";
	timeraddMS(&now, timeout);
	outtime.tv_sec = now.tv_sec;
	outtime.tv_nsec = now.tv_usec * 1000;
	//cout << outtime.tv_sec << " " << (outtime.tv_nsec) << "\n";
	int ret = sem_timedwait(t, &outtime);
	int value;
	sem_getvalue(t, &value);
	cout << "value = " << value << endl;
	cout << "ret = " << ret << endl;
	/*协程结束*/
	return ret;
}

//增加毫秒
void MdSpi::timeraddMS(struct timeval *now_time, int ms) {
	now_time->tv_usec += ms * 1000;
	if (now_time->tv_usec >= 1000000) {
		now_time->tv_sec += now_time->tv_usec / 1000000;
		now_time->tv_usec %= 1000000;
	}
}

//连接
void MdSpi::Connect(char *frontAddress) {
	USER_PRINT("MdSpi::Connect")
	this->mdapi->RegisterFront(frontAddress); //24H
	this->mdapi->Init();
	int ret = this->controlTimeOut(&connect_sem);

	if (ret == -1) {
		USER_PRINT("MdSpi::Connect TimeOut!")
	}
}

//响应连接
void MdSpi::OnFrontConnected() {
	USER_PRINT("MdSpi::OnFrontConnected")
	if (this->isFirstTimeLogged) {
		sem_post(&connect_sem);
	} else {

		sem_init(&login_sem, 0, 1);

		const char *BrokerID = this->BrokerID.c_str();
		char *r_BrokerID = new char[strlen(BrokerID) + 1];
		strcpy(r_BrokerID, BrokerID);

		const char *UserID = this->UserID.c_str();
		char *r_UserID = new char[strlen(UserID) + 1];
		strcpy(r_UserID, BrokerID);

		const char *Password = this->Password.c_str();
		char *r_Password = new char[strlen(Password) + 1];
		strcpy(r_Password, BrokerID);
		
		this->Login(r_BrokerID, r_UserID, r_Password);
	}
	
	//this->Login("9999", "058176", "669822");
}

//登录
void MdSpi::Login(char *BrokerID, char *UserID, char *Password) {
	USER_PRINT("MdSpi::Login")
	this->BrokerID = BrokerID;
	this->UserID = UserID;
	this->Password = Password;
	loginField = new CThostFtdcReqUserLoginField();
	strcpy(loginField->BrokerID, BrokerID);
	strcpy(loginField->UserID, UserID);
	strcpy(loginField->Password, Password);
	this->mdapi->ReqUserLogin(loginField, this->loginRequestID);
	
	int ret = this->controlTimeOut(&login_sem);
	if (ret == -1) {
		USER_PRINT("MdSpi::Login TimeOut!")
	}
}

//响应登录
void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
                           int nRequestID, bool bIsLast) {
	USER_PRINT("MdSpi::OnRspUserLogin")
	USER_PRINT(bIsLast)
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		cout << "交易日：" << this->mdapi->GetTradingDay() << endl;
		this->isLogged = true;
		sem_post(&login_sem);
		if (this->isFirstTimeLogged == false) {
			sem_init(&submarket_sem, 0, 1);
			this->SubMarketData(this->ppInstrumentID, this->nCount);
		}
	}
    
}

//返回数据是否报错
bool MdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo) {
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

//登出
void MdSpi::Logout(char *BrokerID, char *UserID) {
	USER_PRINT("MdSpi::Logout")
	logoutField = new CThostFtdcUserLogoutField();
	strcpy(logoutField->BrokerID, BrokerID);
	strcpy(logoutField->UserID, UserID);
	this->mdapi->ReqUserLogout(logoutField, this->loginRequestID);
	int ret = this->controlTimeOut(&logout_sem);
	if (ret == -1) {
		USER_PRINT("MdSpi::Logout TimeOut!")
	}
}

//响应登出
void MdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast) {
	USER_PRINT("MdSpi::OnRspUserLogout")
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		USER_PRINT("OnRspUserLogout")
		this->isLogged = false;
		sem_post(&logout_sem);
	}
}

//订阅行情
void MdSpi::SubMarketData(char *ppInstrumentID[], int nCount) {
	USER_PRINT("MdSpi::SubMarketData")
	if (this->isLogged) {
		USER_PRINT("SubMarketData")
		this->ppInstrumentID = ppInstrumentID;
		this->nCount = nCount;
		this->mdapi->SubscribeMarketData(ppInstrumentID, nCount);
		int ret = this->controlTimeOut(&submarket_sem);
		if (ret == -1) {
			USER_PRINT("MdSpi::SubMarketData TimeOut!")
		}
	} else {
		USER_PRINT("Please Login First!")
	}
}

//订阅行情应答
void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT(bIsLast)
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		USER_PRINT("OnRspSubMarketData")
		cout << "订阅行情应答" << endl;
		cout << "合约代码:" << pSpecificInstrument->InstrumentID << endl;
		cout << "应答信息:" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
		USER_PRINT("sem_post(&submarket_sem) was called!")
		sem_post(&submarket_sem);

	}
}

//取消订阅行情
void MdSpi::UnSubscribeMarketData(char *ppInstrumentID[], int nCount) {
	USER_PRINT("MdSpi::UnSubscribeMarketData")
	if (this->isLogged) {
		USER_PRINT("UnSubscribeMarketData")
			this->mdapi->UnSubscribeMarketData(ppInstrumentID, nCount);
		int ret = this->controlTimeOut(&unsubmarket_sem);
		if (ret == -1) {
			USER_PRINT("MdSpi::UnSubscribeMarketData TimeOut!")
		}
	}
	else {
		USER_PRINT("Please Login First!")
	}
}

//得到BrokerID
string MdSpi::getBrokerID() {
	return this->BrokerID;
}

//得到UserID
string MdSpi::getUserID() {
	return this->UserID;
}

//得到Password
string MdSpi::getPassword() {
	return this->Password;
}

//取消订阅行情应答
void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT(bIsLast)
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		USER_PRINT("OnRspUnSubMarketData")
		cout << "取消订阅行情应答" << endl;
		cout << "取消合约代码:" << pSpecificInstrument->InstrumentID << endl;
		cout << "取消应答信息:" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
		USER_PRINT("sem_post(&unsubmarket_sem) was called!")
		sem_post(&unsubmarket_sem);
	}
}

//深度行情接收
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	USER_PRINT("OnRtnDepthMarketData")
    cout << "===========================================" << endl;
    cout << "深度行情" << endl;
    cout << "交易日:" << pDepthMarketData->TradingDay << endl
    << "合约代码:" << pDepthMarketData->InstrumentID << endl
    << "最新价:" << pDepthMarketData->LastPrice << endl
    //<< "上次结算价:" << pDepthMarketData->PreSettlementPrice << endl
    //<< "昨收盘:" << pDepthMarketData->PreClosePrice << endl
    //<< "数量:" << pDepthMarketData->Volume << endl
    //<< "昨持仓量:" << pDepthMarketData->PreOpenInterest << endl
    << "最后修改时间" << pDepthMarketData->UpdateTime << endl
    << "最后修改毫秒" << pDepthMarketData->UpdateMillisec << endl;
    //<< "申买价一：" << pDepthMarketData->BidPrice1 << endl
    //<< "申买量一:" << pDepthMarketData->BidVolume1 << endl
    //<< "申卖价一:" << pDepthMarketData->AskPrice1 << endl
    //<< "申卖量一:" << pDepthMarketData->AskVolume1 << endl
    //<< "今收盘价:" << pDepthMarketData->ClosePrice << endl
    //<< "当日均价:" << pDepthMarketData->AveragePrice << endl
    //<< "本次结算价格:" << pDepthMarketData->SettlementPrice << endl
    //<< "成交金额:" << pDepthMarketData->Turnover << endl
    //<< "持仓量:" << pDepthMarketData->OpenInterest << endl;
}

//通信断开
void MdSpi::OnFrontDisconnected(int nReason) {
	USER_PRINT("MdSpi::OnFrontDisconnected")
	USER_PRINT(nReason)
	this->isFirstTimeLogged = false;
}
