#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

#include "TdSpi.h"
using namespace std;
using std::string;

#define DEBUG
#ifdef DEBUG
#define USER_PRINT(x) std::cout << "DEBUG - " << __DATE__ << ", " << __TIME__<< ", " << __FILE__ << ", Line - " << __LINE__ << endl; std::cout << #x << " = " << x << std::endl;
#else
#define USER_PRINT(x)
#endif

//实盘账号
//const string USER_ID = "83601689";
//const string PASS = "270338";
//const string BROKER = "9999";
string tradingDate;

//模拟账号
const string USER_ID = "058176";
const string PASS = "669822";
const string BROKER = "9999";


//协程控制
int TdSpi::controlTimeOut(sem_t *t, int timeout) {
	/*协程开始*/
	//struct timeval now;
	//struct timespec outtime;
	//gettimeofday(&now, NULL);
	////cout << now.tv_sec << " " << (now.tv_usec) << "\n";
	//timeraddMS(&now, timeout);
	//outtime.tv_sec = now.tv_sec;
	//outtime.tv_nsec = now.tv_usec * 1000;
	//cout << outtime.tv_sec << " " << (outtime.tv_nsec) << "\n";
	//int ret = sem_timedwait(t, &outtime);
	int ret = 0;
	int value = 0;
	//sem_getvalue(t, &value);
	//cout << "value = " << value << endl;
	//cout << "ret = " << ret << endl;
	/*协程结束*/
	return ret;
}

//增加毫秒
void TdSpi::timeraddMS(struct timeval *now_time, int ms) {
	now_time->tv_usec += ms * 1000;
	if (now_time->tv_usec >= 1000000) {
		now_time->tv_sec += now_time->tv_usec / 1000000;
		now_time->tv_usec %= 1000000;
	}
}

//构造函数
TdSpi::TdSpi(CThostFtdcTraderApi *tdapi) {
	USER_PRINT("TdSpi::TdSpi")
	/*sem_init(&connect_sem, 0, 1);
	sem_init(&login_sem, 0, 1);
	sem_init(&logout_sem, 0, 1);
	sem_init(&sem_ReqQrySettlementInfoConfirm, 0, 1);
	sem_init(&sem_ReqQrySettlementInfo, 0, 1);
	sem_init(&sem_ReqSettlementInfoConfirm, 0, 1);*/
	this->isLogged = false;
	this->isFirstTimeLogged = true;
	this->loginRequestID = 10;
    this->tdapi = tdapi;
	this->isConfirmSettlement = false;
}

//建立连接
void TdSpi::Connect(char *frontAddress) {
	USER_PRINT("TdSpi::Connect")
	this->tdapi->RegisterFront(frontAddress); //24H
	//注册事件处理对象
	this->tdapi->RegisterSpi(this);
	//订阅共有流和私有流
	this->tdapi->SubscribePublicTopic(THOST_TERT_RESUME);
	this->tdapi->SubscribePrivateTopic(THOST_TERT_RESUME);
	this->tdapi->Init();

	int ret = this->controlTimeOut(&connect_sem);
	if (ret == -1) {
		USER_PRINT("MdSpi::Connect TimeOut!")
	}
}

//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TdSpi::OnFrontConnected() {
	USER_PRINT("TdSpi::OnFrontConnected")
	if (this->isFirstTimeLogged) {
		//sem_post(&connect_sem);
		//this->Login("0187", "86001525", "206029");
	}
	else {
		//sem_init(&login_sem, 0, 1);
		
		//this->Login(this->c_BrokerID, this->c_UserID, this->c_Password);
	}
}

//登录
void TdSpi::Login(char *arg_BrokerID, char *arg_UserID, char *arg_Password) {
	USER_PRINT("TdSpi::Login")

	this->BrokerID = arg_BrokerID;
	this->UserID = arg_UserID;
	this->Password = arg_Password;

	loginField = new CThostFtdcReqUserLoginField();
	strcpy(loginField->BrokerID, arg_BrokerID);
	strcpy(loginField->UserID, arg_UserID);
	strcpy(loginField->Password, arg_Password);
	tdapi->ReqUserLogin(loginField, this->loginRequestID);

	int ret = this->controlTimeOut(&login_sem);

	if (ret == -1) {
		USER_PRINT("TdSpi::Login TimeOut!")
	}
	delete loginField;
}

///登录请求响应
void TdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	USER_PRINT("MdSpi::OnRspUserLogin")
	USER_PRINT(bIsLast)
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		this->isLogged = true;
		//sem_post(&login_sem);
		cout << "=================================================================================" << endl;
		cout << "||TdAPI 交易日:" << this->tdapi->GetTradingDay() << ", ";
		///交易日
		//cout << "CThostFtdcRspUserLoginField 交易日:" << pRspUserLogin->TradingDay << ", ";
		///登录成功时间
		cout << "登录成功时间:" << pRspUserLogin->LoginTime << ", ";
		///经纪公司代码
		cout << "经纪公司代码:" << pRspUserLogin->BrokerID << ", ";
		///用户代码
		cout << "用户代码:" << pRspUserLogin->UserID << endl;
		///交易系统名称
		cout << "||交易系统名称:" << pRspUserLogin->SystemName << ", ";
		///前置编号
		cout << "前置编号:" << pRspUserLogin->FrontID << ", ";
		///会话编号
		cout << "会话编号:" << pRspUserLogin->SessionID << ", ";
		///最大报单引用
		cout << "最大报单引用:" << pRspUserLogin->MaxOrderRef << ", ";
		///上期所时间
		cout << "上期所时间:" << pRspUserLogin->SHFETime << endl;
		///大商所时间
		cout << "||大商所时间:" << pRspUserLogin->DCETime << ", ";
		///郑商所时间
		cout << "郑商所时间:" << pRspUserLogin->CZCETime << ", ";
		///中金所时间
		cout << "中金所时间" << pRspUserLogin->FFEXTime << ", ";
		///能源中心时间
		cout << "能源中心时间" << pRspUserLogin->INETime << endl;
		cout << "=================================================================================" << endl;
	}
}

//查询交易结算确认
void TdSpi::QrySettlementInfoConfirm(char *BrokerID, char *InvestorID, int nRequestID) {
	USER_PRINT("TdSpi::QrySettlementInfoConfirm")

	CThostFtdcQrySettlementInfoConfirmField *qrySettlementField = new CThostFtdcQrySettlementInfoConfirmField();

	strcpy(qrySettlementField->BrokerID, BrokerID);
	strcpy(qrySettlementField->InvestorID, InvestorID);
	sleep(1);
	this->tdapi->ReqQrySettlementInfoConfirm(qrySettlementField, nRequestID);

	int ret = this->controlTimeOut(&sem_ReqQrySettlementInfoConfirm);
	if (ret == -1) {
		USER_PRINT("TdSpi::QrySettlementInfoConfirm TimeOut!")
	}
	delete qrySettlementField;
}


//请求查询结算信息确认响应
void TdSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQrySettlementInfoConfirm");
	USER_PRINT(bIsLast)
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		//sem_post(&sem_ReqQrySettlementInfoConfirm);
		if ((!pSettlementInfoConfirm)) { //如果未确认过，pSettlementInfoConfirm为空
			USER_PRINT("pSettlementInfoConfirm is null");
			if (this->getIsConfirmSettlement()) {
				USER_PRINT("Already Confirm!");
				cout << "|==确认结算信息==|" << endl;
				cout << "|确认日期" << this->getCharTradingDate() << "|" << endl;
				cout << "|交易时间" << this->tdapi->GetTradingDay() << "|" << endl;
				cout << "|================|" << endl;
			}
			else {
				this->QrySettlementInfo(const_cast<char *>(this->getBrokerID().c_str()), const_cast<char *>(this->getUserID().c_str()), this->getRequestID());
			}
		} else {
			USER_PRINT("Already Confirm!")
			cout << "|==确认结算信息==|" << endl;
			cout << "|经纪公司代码" << pSettlementInfoConfirm->BrokerID  << "|" << endl;
			cout << "|投资者代码" << pSettlementInfoConfirm->InvestorID << "|" << endl;
			cout << "|确认日期" << pSettlementInfoConfirm->ConfirmDate << "|" << endl;
			cout << "|确认时间" << pSettlementInfoConfirm->ConfirmTime << "|" << endl;
			cout << "|交易时间" << this->tdapi->GetTradingDay() << "|" << endl;
			cout << "|================|" << endl;

			if (bIsLast) {
				
				//查询交易所
				//this->QryExchange();
				
				//查询合约
				//this->QryInstrument();
			}
			

			//std::chrono::milliseconds sleepDuration(15 * 1000);
			//sleep(1);
			

		}
		{ //执行其他操作
#if 0
			//今天已经确认
			USER_PRINT("Today Has Confirmed!")
			//CThostFtdcQryTradingAccountField *account = new CThostFtdcQryTradingAccountField();
			//strcpy(account->BrokerID, BROKER.c_str());
			//strcpy(account->InvestorID, USER_ID.c_str());
			//this->tdapi->ReqQryTradingAccount(account, 999);


			CThostFtdcQryTradingAccountField *account = new CThostFtdcQryTradingAccountField();
			strcpy(account->BrokerID, BROKER.c_str());
			strcpy(account->InvestorID, USER_ID.c_str());
			tdapi->ReqQryTradingAccount(account, 999);

			//查询请求查询成交
			std::chrono::milliseconds sleepDuration(5 * 1000);
			//std::this_thread::sleep_for(sleepDuration);
			CThostFtdcQryTradeField *a = new CThostFtdcQryTradeField();
			strcpy(a->BrokerID, BROKER.c_str());
			strcpy(a->InvestorID, USER_ID.c_str());
			strcpy(a->InstrumentID, "cu1409");
			strcpy(a->TradeTimeStart, "20140101");
			strcpy(a->TradeTimeEnd, "20140720");
			tdapi->ReqQryTrade(a, 10);

			//请求查询投资者持仓明细
			std::chrono::milliseconds sleepDuration2(1 * 1000);
			std::this_thread::sleep_for(sleepDuration2);
			CThostFtdcQryInvestorPositionField *b = new CThostFtdcQryInvestorPositionField();
			strcpy(b->BrokerID, BROKER.c_str());
			strcpy(b->InvestorID, USER_ID.c_str());
			strcpy(b->InstrumentID, "");
			int result = tdapi->ReqQryInvestorPosition(b, 10);
			cout << result << endl;

			////休息两秒再发
			//std::chrono::milliseconds sleepDuration(1*1000);
			//std::this_thread::sleep_for(sleepDuration);
			//cout << "X.X" << endl;
			//int result=tdapi->ReqQryInvestorPosition(a, 3);
			//cout << "result:" << result << endl;
#endif
		}
	}
}

//查询结算信息
void TdSpi::QrySettlementInfo(char *BrokerID, char *InvestorID, int nRequestID) {
	USER_PRINT("TdSpi::QrySettlementInfo")
	cout << "broker ID" << BrokerID << endl;
	cout << "InvestorID" << InvestorID << endl;
	CThostFtdcQrySettlementInfoField *pQrySettlementInfo = new CThostFtdcQrySettlementInfoField();
	strcpy(pQrySettlementInfo->BrokerID, BrokerID);
	strcpy(pQrySettlementInfo->InvestorID, InvestorID);
	//strcpy(pQrySettlementInfo->TradingDay, "");
	sleep(1);
	this->tdapi->ReqQrySettlementInfo(pQrySettlementInfo, nRequestID);
	int ret = this->controlTimeOut(&sem_ReqQrySettlementInfo);
	if (ret == -1) {
		USER_PRINT("TdSpi::QrySettlementInfo TimeOut!")
	}
	delete pQrySettlementInfo;
}

//请求查询投资者结算结果响应
void TdSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                   CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQrySettlementInfo");
	cout << "bIsLast = " << bIsLast;
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		//sem_post(&sem_ReqQrySettlementInfo);
		if (pSettlementInfo) {
			///交易日
			cout << "交易日:" << pSettlementInfo->TradingDay << endl;
			///结算编号
			cout << "结算编号:" << pSettlementInfo->SettlementID << endl;
			///经纪公司代码
			cout << "经纪公司代码:" << pSettlementInfo->BrokerID << endl;
			///投资者代码
			cout << "投资者代码:" << pSettlementInfo->InvestorID << endl;
			///序号
			cout << "序号:" << pSettlementInfo->SequenceNo << endl;
			///消息正文
			cout << "消息正文:" << pSettlementInfo->Content << endl;
		}
	}
	if (bIsLast) {
		//确认投资者结算结果
		this->ConfirmSettlementInfo(const_cast<char *>(this->getBrokerID().c_str()), const_cast<char *>(this->getUserID().c_str()), this->getCharTradingDate(), this->getRequestID());
	}
}

//确认结算结果
void TdSpi::ConfirmSettlementInfo(char *BrokerID, char *InvestorID, char *TradingDay, int nRequestID) {
	USER_PRINT("TdSpi::ConfirmSettlementInfo");
	CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm = new CThostFtdcSettlementInfoConfirmField();

	strcpy(pSettlementInfoConfirm->BrokerID, BrokerID);
	strcpy(pSettlementInfoConfirm->InvestorID, InvestorID);
	cout << this->getCharTradingDate() << endl;
	strcpy(pSettlementInfoConfirm->ConfirmDate, this->getCharTradingDate());

	sleep(1);
	this->tdapi->ReqSettlementInfoConfirm(pSettlementInfoConfirm, nRequestID);

	int ret = this->controlTimeOut(&sem_ReqSettlementInfoConfirm);
	if (ret == -1) {
		USER_PRINT("TdSpi::ConfirmSettlementInfo")
	}
	delete pSettlementInfoConfirm;
}

//投资者结算结果确认响应
void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                       CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	USER_PRINT("TdSpi::OnRspSettlementInfoConfirm");
	USER_PRINT(bIsLast);
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		//sem_post(&sem_ReqSettlementInfoConfirm);
		if (pSettlementInfoConfirm) {
			///经纪公司代码
			cout << "经纪公司代码" << pSettlementInfoConfirm->BrokerID << endl;
			///投资者代码
			cout << "投资者代码" << pSettlementInfoConfirm->InvestorID << endl;
			///确认日期
			cout << "确认日期" << pSettlementInfoConfirm->ConfirmDate << endl;
			///确认时间
			cout << "确认时间" << pSettlementInfoConfirm->ConfirmTime << endl;
			string today_date = this->tdapi->GetTradingDay();
			string confirm_date = pSettlementInfoConfirm->ConfirmDate;
			USER_PRINT(today_date);
			USER_PRINT(confirm_date);
			if (today_date == confirm_date) {
				USER_PRINT("today_date == confirm_date");
				this->setIsConfirmSettlement(true);
			}
			else {
				USER_PRINT("today_date != confirm_date");
				this->setIsConfirmSettlement(false);
			}
		}
	}
	if (bIsLast) {
		this->QrySettlementInfoConfirm(const_cast<char *>(this->getBrokerID().c_str()), const_cast<char *>(this->getUserID().c_str()), this->getRequestID());
	}
}

//查询交易所
void TdSpi::QryExchange() {
	USER_PRINT("TdSpi::QryExchange");
	CThostFtdcQryExchangeField *pQryExchange = new CThostFtdcQryExchangeField();
	strcpy(pQryExchange->ExchangeID, "");
	this->tdapi->ReqQryExchange(pQryExchange, this->loginRequestID);
	delete pQryExchange;
}

//响应查询交易所
void TdSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryExchange")
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		///交易所代码
		cout << "交易所代码" << pExchange->ExchangeID << endl;
		///交易所名称
		cout << "交易所名称" << pExchange->ExchangeName << endl;
		///交易所属性
		cout << "交易所属性" << pExchange->ExchangeProperty << endl;
	}
}

//查询合约
void TdSpi::QryInstrument(string exchangeid, string instrumentid) {
	USER_PRINT("TdSpi::QryInstrument Begin")
	CThostFtdcQryInstrumentField *pQryInstrument = new CThostFtdcQryInstrumentField();
	strcpy(pQryInstrument->ExchangeID, exchangeid.c_str());
	strcpy(pQryInstrument->InstrumentID, instrumentid.c_str());
	sleep(1);
	this->tdapi->ReqQryInstrument(pQryInstrument, this->loginRequestID + 1);
	USER_PRINT("TdSpi::QryInstrument End")
	delete pQryInstrument;
}

//响应查询合约
void TdSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	//USER_PRINT("TdSpi::OnRspQryInstrument")
	cout << "isLast" << bIsLast << endl;
	if ((!this->IsErrorRspInfo(pRspInfo))) {
		///合约代码
		cout << "合约代码:" << pInstrument->InstrumentID << ", ";
		///交易所代码
		cout << "交易所代码:" << pInstrument->ExchangeID << ", ";
		///合约名称
		cout << "合约名称:" << pInstrument->InstrumentName << ", ";
		///合约在交易所的代码
		cout << "合约在交易所的代码:" << pInstrument->ExchangeInstID << ", ";
		///产品代码
		cout << "产品代码:" << pInstrument->ProductID << ", ";
		///产品类型
		cout << "产品类型:" << pInstrument->ProductClass << endl;
		/*///交割年份
		cout << "交割年份" << pInstrument->DeliveryYear << endl;
		///交割月
		cout << "交割月" << pInstrument->DeliveryMonth << endl;
		///市价单最大下单量
		cout << "市价单最大下单量" << pInstrument->MaxMarketOrderVolume << endl;
		///市价单最小下单量
		cout << "市价单最小下单量" << pInstrument->MinMarketOrderVolume << endl;
		///限价单最大下单量
		cout << "限价单最大下单量" << pInstrument->MaxLimitOrderVolume << endl;
		///限价单最小下单量
		cout << "限价单最小下单量" << pInstrument->MinLimitOrderVolume << endl;
		///合约数量乘数
		cout << "合约数量乘数" << pInstrument->VolumeMultiple << endl;
		///最小变动价位
		cout << "最小变动价位" << pInstrument->PriceTick << endl;
		///创建日
		cout << "创建日" << pInstrument->CreateDate << endl;
		///上市日
		cout << "上市日" << pInstrument->OpenDate << endl;
		///到期日
		cout << "到期日" << pInstrument->ExpireDate << endl;
		///开始交割日
		cout << "开始交割日" << pInstrument->StartDelivDate << endl;
		///结束交割日
		cout << "结束交割日" << pInstrument->EndDelivDate << endl;
		///合约生命周期状态
		cout << "合约生命周期状态" << pInstrument->InstLifePhase << endl;
		///当前是否交易
		cout << "当前是否交易" << pInstrument->IsTrading << endl;
		///持仓类型
		cout << "持仓类型" << pInstrument->PositionType << endl;
		///持仓日期类型
		cout << "持仓日期类型" << pInstrument->PositionDateType << endl;
		///多头保证金率
		cout << "多头保证金率" << pInstrument->LongMarginRatio << endl;
		///空头保证金率
		cout << "空头保证金率" << pInstrument->ShortMarginRatio << endl;
		///是否使用大额单边保证金算法
		cout << "是否使用大额单边保证金算法" << pInstrument->MaxMarginSideAlgorithm << endl;
		///基础商品代码
		cout << "基础商品代码" << pInstrument->UnderlyingInstrID << endl;
		///执行价
		cout << "执行价" << pInstrument->StrikePrice << endl;
		///期权类型
		cout << "期权类型" << pInstrument->OptionsType << endl;
		///合约基础商品乘数
		cout << "合约基础商品乘数" << pInstrument->UnderlyingMultiple << endl;
		///组合类型
		cout << "组合类型" << pInstrument->CombinationType << endl;*/
	}
}

///合约交易状态通知
void TdSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) {
	USER_PRINT("TdSpi::OnRtnInstrumentStatus");
	if (pInstrumentStatus) {
		///交易所代码
		cout << "交易所代码" << pInstrumentStatus->ExchangeID << endl;
		///合约在交易所的代码
		cout << "合约在交易所的代码" << pInstrumentStatus->ExchangeInstID << endl;
		///结算组代码
		cout << "结算组代码" << pInstrumentStatus->SettlementGroupID << endl;
		///合约代码
		cout << "合约代码" << pInstrumentStatus->InstrumentID << endl;
		///合约交易状态
		cout << "合约交易状态" << pInstrumentStatus->InstrumentStatus << endl;
		///交易阶段编号
		cout << "交易阶段编号" << pInstrumentStatus->TradingSegmentSN << endl;
		///进入本状态时间
		cout << "进入本状态时间" << pInstrumentStatus->EnterTime << endl;
		///进入本状态原因
		cout << "进入本状态原因" << pInstrumentStatus->EnterReason << endl;
	}
}

//查询报单
void TdSpi::QryOrder() {
	USER_PRINT("TdSpi::QryOrder")
	CThostFtdcQryOrderField *pQryOrder = new CThostFtdcQryOrderField();
	//strcpy(pQryOrder->BrokerID, const_cast<char *>(this->getBrokerID().c_str()));
	//strcpy(pQryOrder->InvestorID, const_cast<char *>(this->getUserID().c_str()));
	int error_no = this->tdapi->ReqQryOrder(pQryOrder, this->getRequestID());
	cout << "error_no = " << error_no << endl;
	delete pQryOrder;
}

//响应查询报单;
void TdSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryOrder")
	if (!pOrder) {
		cout << "None Order Return!" << endl;
	}
	if (!this->IsErrorRspInfo(pRspInfo)) {
		if (pOrder) {
			cout << "=================================================================================" << endl;
			///经纪公司代码
			cout << "||经纪公司代码:" << pOrder->BrokerID << ", ";
			///投资者代码
			cout << "投资者代码:" << pOrder->InvestorID << ", ";
			///合约代码
			cout << "合约代码:" << pOrder->InstrumentID << ", ";
			///报单引用
			cout << "报单引用:" << pOrder->OrderRef << ", ";
			///用户代码
			cout << "用户代码:" << pOrder->UserID << endl;
			///报单价格条件
			cout << "||报单价格条件:" << pOrder->OrderPriceType << ", ";
			///买卖方向
			cout << "买卖方向:" << pOrder->Direction << ", ";
			///组合开平标志
			cout << "组合开平标志:" << pOrder->CombOffsetFlag << ", ";
			///组合投机套保标志
			cout << "组合投机套保标志:" << pOrder->CombHedgeFlag << ", ";
			///价格
			cout << "价格:" << pOrder->LimitPrice << endl;
			///数量
			cout << "||数量:" << pOrder->VolumeTotalOriginal << ", ";
			///有效期类型
			cout << "有效期类型:" << pOrder->TimeCondition << ", ";
			///GTD日期
			//cout << "GTD日期:" << pOrder->GTDDate << ", ";
			///成交量类型
			cout << "成交量类型:" << pOrder->VolumeCondition << ", ";
			///最小成交量
			cout << "最小成交量:" << pOrder->MinVolume << endl;
			///触发条件
			cout << "||触发条件:" << pOrder->ContingentCondition << ", ";
			///止损价
			cout << "止损价:" << pOrder->StopPrice << ", ";
			///强平原因
			cout << "强平原因:" << pOrder->ForceCloseReason << ", ";
			///自动挂起标志
			cout << "自动挂起标志:" << pOrder->IsAutoSuspend << ", ";
			///业务单元
			cout << "业务单元:" << pOrder->BusinessUnit << endl;
			///请求编号
			cout << "||请求编号:" << pOrder->RequestID << ", ";
			///本地报单编号
			cout << "本地报单编号:" << pOrder->OrderLocalID << ", ";
			///交易所代码
			cout << "交易所代码:" << pOrder->ExchangeID << ", ";
			///会员代码
			cout << "会员代码:" << pOrder->ParticipantID << ", ";
			///客户代码
			cout << "客户代码:" << pOrder->ClientID << endl;
			///合约在交易所的代码
			cout << "||合约在交易所的代码:" << pOrder->ExchangeInstID << ", ";
			///交易所交易员代码
			cout << "交易所交易员代码:" << pOrder->TraderID << ", ";
			///安装编号
			cout << "安装编号:" << pOrder->InstallID << ", ";
			///报单提交状态
			cout << "报单提交状态:" << pOrder->OrderSubmitStatus << ", ";
			///报单提示序号
			cout << "报单提示序号:" << pOrder->NotifySequence << endl;
			///交易日
			cout << "||交易日:" << pOrder->TradingDay << ", ";
			///结算编号
			cout << "结算编号:" << pOrder->SettlementID << ", ";
			///报单编号
			cout << "报单编号:" << pOrder->OrderSysID << ", ";
			///报单来源
			cout << "报单来源:" << pOrder->OrderSource << ", ";
			///报单状态
			cout << "报单状态:" << pOrder->OrderStatus << endl;
			///报单类型
			cout << "||报单类型:" << pOrder->OrderType << ", ";
			///今成交数量
			cout << "今成交数量:" << pOrder->VolumeTraded << ", ";
			///剩余数量
			cout << "剩余数量:" << pOrder->VolumeTotal << ", ";
			///报单日期
			cout << "报单日期:" << pOrder->InsertDate << ", ";
			///委托时间
			cout << "委托时间:" << pOrder->InsertTime << endl;
			///激活时间
			cout << "||激活时间:" << pOrder->ActiveTime << ", ";
			///挂起时间
			cout << "挂起时间:" << pOrder->SuspendTime << ", ";
			///最后修改时间
			cout << "最后修改时间:" << pOrder->UpdateTime << ", ";
			///撤销时间
			cout << "撤销时间:" << pOrder->CancelTime << ", ";
			///最后修改交易所交易员代码
			cout << "最后修改交易所交易员代码:" << pOrder->ActiveTraderID << endl;
			///结算会员编号
			cout << "||结算会员编号:" << pOrder->ClearingPartID << ", ";
			///序号
			cout << "序号:" << pOrder->SequenceNo << ", ";
			///前置编号
			cout << "前置编号:" << pOrder->FrontID << ", ";
			///会话编号
			cout << "会话编号:" << pOrder->SessionID << ", ";
			///用户端产品信息
			cout << "用户端产品信息:" << pOrder->UserProductInfo << endl;
			///状态信息
			cout << "||状态信息:" << pOrder->StatusMsg << ", ";
			///用户强评标志
			cout << "用户强评标志:" << pOrder->UserForceClose << ", ";
			///操作用户代码
			cout << "操作用户代码:" << pOrder->ActiveUserID << ", ";
			///经纪公司报单编号
			cout << "经纪公司报单编号:" << pOrder->BrokerOrderSeq << ", ";
			///相关报单
			cout << "相关报单:" << pOrder->RelativeOrderSysID << endl;
			///郑商所成交数量
			cout << "||郑商所成交数量:" << pOrder->ZCETotalTradedVolume << ", ";
			///互换单标志
			cout << "互换单标志:" << pOrder->IsSwapOrder << endl;
			cout << "=================================================================================" << endl;
		} else {
			cout << "none order return!" << endl;
		}
		
	}

}

//查询保证金率
void TdSpi::QryInstrumentMarginRate() {
	CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate = new CThostFtdcQryInstrumentMarginRateField();

	this->tdapi->ReqQryInstrumentMarginRate(pQryInstrumentMarginRate, this->getRequestID());
	delete pQryInstrumentMarginRate;
}

///请求查询合约保证金率响应
void TdSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryInstrumentMarginRate")
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		if (pInstrumentMarginRate) {
			cout << "=================================================================================" << endl;
			///合约代码
			cout << "||合约代码:" << pInstrumentMarginRate->InstrumentID << ", ";
			///投资者范围
			cout << "投资者范围:" << pInstrumentMarginRate->InvestorRange << ", ";
			///经纪公司代码
			cout << "经纪公司代码:" << pInstrumentMarginRate->BrokerID << ", ";
			///投资者代码
			cout << "投资者代码:" << pInstrumentMarginRate->InvestorID << ", ";
			///投机套保标志
			cout << "投机套保标志:" << pInstrumentMarginRate->HedgeFlag << endl;
			///多头保证金率
			cout << "||多头保证金率:" << pInstrumentMarginRate->LongMarginRatioByMoney << ", ";
			///多头保证金费
			cout << "多头保证金费:" << pInstrumentMarginRate->LongMarginRatioByVolume << ", ";
			///空头保证金率
			cout << "空头保证金率:" << pInstrumentMarginRate->ShortMarginRatioByMoney << ", ";
			///空头保证金费
			cout << "空头保证金费:" << pInstrumentMarginRate->ShortMarginRatioByVolume << ", ";
			///是否相对交易所收取
			cout << "是否相对交易所收取:" << pInstrumentMarginRate->IsRelative << endl;
			cout << "=================================================================================" << endl;
		}
	}
}


//查询手续费
void TdSpi::QryInstrumentCommissionRate() {
	CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate = new CThostFtdcQryInstrumentCommissionRateField();
	this->tdapi->ReqQryInstrumentCommissionRate(pQryInstrumentCommissionRate, this->getRequestID());
	delete pQryInstrumentCommissionRate;
}

///请求查询合约手续费率响应
void TdSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryInstrumentCommissionRate")
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		if (pInstrumentCommissionRate) {
			///合约代码
			cout << "合约代码:" << pInstrumentCommissionRate->InstrumentID << endl;
			///投资者范围
			cout << "投资者范围:" << pInstrumentCommissionRate->InvestorRange << endl;
			///经纪公司代码
			cout << "经纪公司代码:" << pInstrumentCommissionRate->BrokerID << endl;
			///投资者代码
			cout << "投资者代码:" << pInstrumentCommissionRate->InvestorID << endl;
			///开仓手续费率
			cout << "开仓手续费率:" << pInstrumentCommissionRate->OpenRatioByMoney << endl;
			///开仓手续费
			cout << "开仓手续费:" << pInstrumentCommissionRate->OpenRatioByVolume << endl;
			///平仓手续费率
			cout << "平仓手续费率:" << pInstrumentCommissionRate->CloseRatioByMoney << endl;
			///平仓手续费
			cout << "平仓手续费:" << pInstrumentCommissionRate->CloseRatioByVolume << endl;
			///平今手续费率
			cout << "平今手续费率:" << pInstrumentCommissionRate->CloseTodayRatioByMoney << endl;
			///平今手续费
			cout << "平今手续费:" << pInstrumentCommissionRate->CloseTodayRatioByVolume << endl;
		}
	}
}

//查询投资者
void TdSpi::QryInvestor() {
	USER_PRINT("TdSpi::QryInvestor")
	CThostFtdcQryInvestorField *pQryInvestor = new CThostFtdcQryInvestorField();
	this->tdapi->ReqQryInvestor(pQryInvestor, this->getRequestID());
	delete pQryInvestor;
}

//查询投资者响应
void TdSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryInvestor")
	if (!this->IsErrorRspInfo(pRspInfo)) {
		if (pInvestor) {
			cout << "=================================================================================" << endl;
			///投资者代码
			cout << "||投资者代码:" << pInvestor->InvestorID << ", ";
			///经纪公司代码
			cout << "经纪公司代码:" << pInvestor->BrokerID << ", ";
			///投资者分组代码
			cout << "投资者分组代码:" << pInvestor->InvestorGroupID << ", ";
			///投资者名称
			cout << "投资者名称:" << pInvestor->InvestorName << ", ";
			///证件类型
			cout << "证件类型:" << pInvestor->IdentifiedCardType << endl;
			///证件号码
			cout << "||证件号码:" << pInvestor->IdentifiedCardNo << ", ";
			///是否活跃
			cout << "是否活跃:" << pInvestor->IsActive << ", ";
			///联系电话
			cout << "联系电话:" << pInvestor->Telephone << ", ";
			///通讯地址
			cout << "通讯地址:" << pInvestor->Address << ", ";
			///开户日期
			cout << "开户日期:" << pInvestor->OpenDate << endl;
			///手机
			cout << "||手机:" << pInvestor->Mobile << ", ";
			///手续费率模板代码
			cout << "手续费率模板代码:" << pInvestor->CommModelID << ", ";
			///保证金率模板代码
			cout << "保证金率模板代码:" << pInvestor->MarginModelID << endl;
			cout << "=================================================================================" << endl;
		}
	}
}

//查询投资者持仓
void TdSpi::QryInvestorPosition() {
	USER_PRINT("TdSpi::QryInvestorPosition");
	CThostFtdcQryInvestorPositionField *pQryInvestorPosition = new CThostFtdcQryInvestorPositionField();
	this->tdapi->ReqQryInvestorPosition(pQryInvestorPosition, this->getRequestID());
	delete pQryInvestorPosition;
}

//请求查询投资者持仓响应
void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryInvestorPosition");
	if (!this->IsErrorRspInfo(pRspInfo)) {
		if (pInvestorPosition) {
			cout << "=================================================================================" << endl;
			///合约代码
			cout << "||合约代码:" << pInvestorPosition->InstrumentID << ", ";
			///经纪公司代码
			cout << "经纪公司代码:" << pInvestorPosition->BrokerID << ", ";
			///投资者代码
			cout << "投资者代码:" << pInvestorPosition->InvestorID << ", ";
			///持仓多空方向
			cout << "持仓多空方向:" << pInvestorPosition->PosiDirection << ", ";
			///投机套保标志
			cout << "投机套保标志:" << pInvestorPosition->HedgeFlag << endl;
			///持仓日期
			cout << "||持仓日期:" << pInvestorPosition->PositionDate << ", ";
			///上日持仓
			cout << "上日持仓:" << pInvestorPosition->YdPosition << ", ";
			///今日持仓
			cout << "今日持仓:" << pInvestorPosition->Position << ", ";
			///多头冻结
			cout << "多头冻结:" << pInvestorPosition->LongFrozen << ", ";
			///空头冻结
			cout << "空头冻结:" << pInvestorPosition->ShortFrozen << endl;
			///开仓冻结金额
			cout << "||开仓冻结金额:" << pInvestorPosition->LongFrozenAmount << ", ";
			///开仓冻结金额
			cout << "开仓冻结金额:" << pInvestorPosition->ShortFrozenAmount << ", ";
			///开仓量
			cout << "开仓量:" << pInvestorPosition->OpenVolume << ", ";
			///平仓量
			cout << "平仓量:" << pInvestorPosition->CloseVolume << ", ";
			///开仓金额
			cout << "开仓金额:" << pInvestorPosition->OpenAmount << endl;
			///平仓金额
			cout << "||平仓金额:" << pInvestorPosition->CloseAmount << ", ";
			///持仓成本
			cout << "持仓成本:" << pInvestorPosition->PositionCost << ", ";
			///上次占用的保证金
			cout << "上次占用的保证金:" << pInvestorPosition->PreMargin << ", ";
			///占用的保证金
			cout << "占用的保证金:" << pInvestorPosition->UseMargin << ", ";
			///冻结的保证金
			cout << "冻结的保证金:" << pInvestorPosition->FrozenMargin << endl;
			///冻结的资金
			cout << "||冻结的资金:" << pInvestorPosition->FrozenCash << ", ";
			///冻结的手续费
			cout << "冻结的手续费:" << pInvestorPosition->FrozenCommission << ", ";
			///资金差额
			cout << "资金差额:" << pInvestorPosition->CashIn << ", ";
			///手续费
			cout << "手续费:" << pInvestorPosition->Commission << ", ";
			///平仓盈亏
			cout << "平仓盈亏:" << pInvestorPosition->CloseProfit << endl;
			///持仓盈亏
			cout << "||持仓盈亏:" << pInvestorPosition->PositionProfit << ", ";
			///上次结算价
			cout << "上次结算价:" << pInvestorPosition->PreSettlementPrice << ", ";
			///本次结算价
			cout << "本次结算价:" << pInvestorPosition->SettlementPrice << ", ";
			///交易日
			cout << "交易日:" << pInvestorPosition->TradingDay << ", ";
			///结算编号
			cout << "结算编号:" << pInvestorPosition->SettlementID << endl;
			///开仓成本
			cout << "||开仓成本:" << pInvestorPosition->OpenCost << ", ";
			///交易所保证金
			cout << "交易所保证金:" << pInvestorPosition->ExchangeMargin << ", ";
			///组合成交形成的持仓
			cout << "组合成交形成的持仓:" << pInvestorPosition->CombPosition << ", ";
			///组合多头冻结
			cout << "组合多头冻结:" << pInvestorPosition->CombLongFrozen << ", ";
			///组合空头冻结
			cout << "组合空头冻结:" << pInvestorPosition->CombShortFrozen << endl;
			///逐日盯市平仓盈亏
			cout << "||逐日盯市平仓盈亏:" << pInvestorPosition->CloseProfitByDate << ", ";
			///逐笔对冲平仓盈亏
			cout << "逐笔对冲平仓盈亏:" << pInvestorPosition->CloseProfitByTrade << ", ";
			///今日持仓
			cout << "今日持仓:" << pInvestorPosition->TodayPosition << ", ";
			///保证金率
			cout << "保证金率:" << pInvestorPosition->MarginRateByMoney << ", ";
			///保证金率(按手数)
			cout << "保证金率(按手数):" << pInvestorPosition->MarginRateByVolume << endl;
			///执行冻结
			cout << "||执行冻结:" << pInvestorPosition->StrikeFrozen << ", ";
			///执行冻结金额
			cout << "执行冻结金额:" << pInvestorPosition->StrikeFrozenAmount << ", ";
			///放弃执行冻结
			cout << "放弃执行冻结:" << pInvestorPosition->AbandonFrozen << endl;
			cout << "=================================================================================" << endl;
		}
	}
}

//查询账号资金
void TdSpi::QryTradingAccount() {
	CThostFtdcQryTradingAccountField *pQryTradingAccount = new CThostFtdcQryTradingAccountField();
	this->tdapi->ReqQryTradingAccount(pQryTradingAccount, this->getRequestID());
	delete pQryTradingAccount;
}

//查询账号资金响应
void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryTradingAccount")
	if (!this->IsErrorRspInfo(pRspInfo)) {
		if (pTradingAccount) {
			cout << "=================================================================================" << endl;
			///经纪公司代码
			cout << "||经纪公司代码:" << pTradingAccount->BrokerID << ", ";
			///投资者帐号
			cout << "投资者帐号:" << pTradingAccount->AccountID << ", ";
			///上次质押金额
			cout << "上次质押金额:" << pTradingAccount->PreMortgage << ", ";
			///上次信用额度
			cout << "上次信用额度:" << pTradingAccount->PreCredit << ", ";
			///上次存款额
			cout << "上次存款额:" << pTradingAccount->PreDeposit << endl;
			///上次结算准备金
			cout << "||上次结算准备金:" << pTradingAccount->PreBalance << ", ";
			///上次占用的保证金
			cout << "上次占用的保证金:" << pTradingAccount->PreMargin << ", ";
			///利息基数
			cout << "利息基数:" << pTradingAccount->InterestBase << ", ";
			///利息收入
			cout << "利息收入:" << pTradingAccount->Interest << ", ";
			///入金金额
			cout << "入金金额:" << pTradingAccount->Deposit << endl;
			///出金金额
			cout << "||出金金额:" << pTradingAccount->Withdraw << ", ";
			///冻结的保证金
			cout << "冻结的保证金:" << pTradingAccount->FrozenMargin << ", ";
			///冻结的资金
			cout << "冻结的资金:" << pTradingAccount->FrozenCash << ", ";
			///冻结的手续费
			cout << "冻结的手续费:" << pTradingAccount->FrozenCommission << ", ";
			///当前保证金总额
			cout << "当前保证金总额:" << pTradingAccount->CurrMargin << endl;
			///资金差额
			cout << "||资金差额:" << pTradingAccount->CashIn << ", ";
			///手续费
			cout << "手续费:" << pTradingAccount->Commission << ", ";
			///平仓盈亏
			cout << "平仓盈亏:" << pTradingAccount->CloseProfit << ", ";
			///持仓盈亏
			cout << "持仓盈亏:" << pTradingAccount->PositionProfit << ", ";
			///期货结算准备金
			cout << "期货结算准备金:" << pTradingAccount->Balance << endl;
			///可用资金
			cout << "||可用资金:" << pTradingAccount->Available << ", ";
			///可取资金
			cout << "可取资金:" << pTradingAccount->WithdrawQuota << ", ";
			///基本准备金
			cout << "基本准备金:" << pTradingAccount->Reserve << ", ";
			///交易日
			cout << "交易日:" << pTradingAccount->TradingDay << ", ";
			///结算编号
			cout << "结算编号:" << pTradingAccount->SettlementID << endl;
			///信用额度
			cout << "||信用额度:" << pTradingAccount->Credit << ", ";
			///质押金额
			cout << "质押金额:" << pTradingAccount->Mortgage << ", ";
			///交易所保证金
			cout << "交易所保证金:" << pTradingAccount->ExchangeMargin << ", ";
			///投资者交割保证金
			cout << "投资者交割保证金:" << pTradingAccount->DeliveryMargin << ", ";
			///交易所交割保证金
			cout << "交易所交割保证金:" << pTradingAccount->ExchangeDeliveryMargin << endl;
			///保底期货结算准备金
			cout << "||保底期货结算准备金:" << pTradingAccount->ReserveBalance << ", ";
			///币种代码
			cout << "币种代码:" << pTradingAccount->CurrencyID << ", ";
			///上次货币质入金额
			cout << "上次货币质入金额:" << pTradingAccount->PreFundMortgageIn << ", ";
			///上次货币质出金额
			cout << "上次货币质出金额:" << pTradingAccount->PreFundMortgageOut << ", ";
			///货币质入金额
			cout << "货币质入金额:" << pTradingAccount->FundMortgageIn << endl;
			///货币质出金额
			cout << "||货币质出金额:" << pTradingAccount->FundMortgageOut << ", ";
			///货币质押余额
			cout << "货币质押余额:" << pTradingAccount->FundMortgageAvailable << ", ";
			///可质押货币金额
			cout << "可质押货币金额:" << pTradingAccount->MortgageableFund << ", ";
			///特殊产品占用保证金
			cout << "特殊产品占用保证金:" << pTradingAccount->SpecProductMargin << ", ";
			///特殊产品冻结保证金
			cout << "特殊产品冻结保证金:" << pTradingAccount->SpecProductFrozenMargin << endl;
			///特殊产品手续费
			cout << "||特殊产品手续费:" << pTradingAccount->SpecProductCommission << ", ";
			///特殊产品冻结手续费
			cout << "特殊产品冻结手续费:" << pTradingAccount->SpecProductFrozenCommission << ", ";
			///特殊产品持仓盈亏
			cout << "特殊产品持仓盈亏:" << pTradingAccount->SpecProductPositionProfit << ", ";
			///特殊产品平仓盈亏
			cout << "特殊产品平仓盈亏:" << pTradingAccount->SpecProductCloseProfit << ", ";
			///根据持仓盈亏算法计算的特殊产品持仓盈亏
			cout << "根据持仓盈亏算法计算的特殊产品持仓盈亏:" << pTradingAccount->SpecProductPositionProfitByAlg << endl;
			///特殊产品交易所保证金
			cout << "||特殊产品交易所保证金:" << pTradingAccount->SpecProductExchangeMargin << endl;
			cout << "=================================================================================" << endl;
		}
	}
}

//查询成交单
void TdSpi::QryTrade() {
	USER_PRINT("TdSpi::QryTrade");
	CThostFtdcQryTradeField *pQryTrade = new CThostFtdcQryTradeField();
	this->tdapi->ReqQryTrade(pQryTrade, this->getRequestID());
	delete pQryTrade;
}

//成交通知
void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	USER_PRINT("TdSpi::OnRtnTrade");
	if (pTrade) {
		cout << "=================================================================================" << endl;
		///经纪公司代码
		cout << "||经纪公司代码" << pTrade->BrokerID << ", "
		///投资者代码
		cout << "投资者代码" << pTrade->InvestorID << ", "
		///合约代码
		cout << "合约代码" << pTrade->InstrumentID << ", "
		///报单引用
		cout << "报单引用" << pTrade->OrderRef << ", "
		///用户代码
		cout << "用户代码" << pTrade->UserID << endl;
		///交易所代码
		cout << "||交易所代码" << pTrade->ExchangeID << ", "
		///成交编号
		cout << "成交编号" << pTrade->TradeID << ", "
		///买卖方向
		cout << "买卖方向" << pTrade->Direction << ", "
		///报单编号
		cout << "报单编号" << pTrade->OrderSysID << ", "
		///会员代码
		cout << "会员代码" << pTrade->ParticipantID << endl;
		///客户代码
		cout << "||客户代码" << pTrade->ClientID << ", "
		///交易角色
		cout << "交易角色" << pTrade->TradingRole << ", "
		///合约在交易所的代码
		cout << "合约在交易所的代码" << pTrade->ExchangeInstID << ", "
		///开平标志
		cout << "开平标志" << pTrade->OffsetFlag << ", "
		///投机套保标志
		cout << "投机套保标志" << pTrade->HedgeFlag << endl;
		///价格
		cout << "||价格" << pTrade->Price << ", "
		///数量
		cout << "数量" << pTrade->Volume << ", "
		///成交时期
		cout << "成交时期" << pTrade->TradeDate << ", "
		///成交时间
		cout << "成交时间" << pTrade->TradeTime << ", "
		///成交类型
		cout << "成交类型" << pTrade->TradeType << endl;
		///成交价来源
		cout << "||成交价来源" << pTrade->PriceSource << ", "
		///交易所交易员代码
		cout << "交易所交易员代码" << pTrade->TraderID << ", "
		///本地报单编号
		cout << "本地报单编号" << pTrade->OrderLocalID << ", "
		///结算会员编号
		cout << "结算会员编号" << pTrade->ClearingPartID << endl;
		///业务单元
		cout << "||业务单元" << pTrade->BusinessUnit << ", "
		///序号
		cout << "序号" << pTrade->SequenceNo << ", "
		///交易日
		cout << "交易日" << pTrade->TradingDay << ", "
		///结算编号
		cout << "结算编号" << pTrade->SettlementID << endl;
		///经纪公司报单编号
		cout << "||经纪公司报单编号" << pTrade->BrokerOrderSeq << ", "
		///成交来源
		cout << "成交来源" << pTrade->TradeSource << endl;
		cout << "=================================================================================" << endl;
	}
}

//查询成交单响应
void TdSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspQryTrade");
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		if (pTrade) {
			cout << "=================================================================================" << endl;
			///经纪公司代码
			cout << "||经纪公司代码:" << pTrade->BrokerID << ",";
			///投资者代码
			cout << "投资者代码:" << pTrade->InvestorID << ",";
			///合约代码
			cout << "合约代码:" << pTrade->InstrumentID << ",";
			///报单引用
			cout << "报单引用:" << pTrade->OrderRef << ",";
			///用户代码
			cout << "用户代码:" << pTrade->UserID << endl;
			///交易所代码
			cout << "||交易所代码:" << pTrade->ExchangeID << ",";
			///成交编号
			cout << "成交编号:" << pTrade->TradeID << ",";
			///买卖方向
			cout << "买卖方向:" << pTrade->Direction << ",";
			///报单编号
			cout << "报单编号:" << pTrade->OrderSysID << ",";
			///会员代码
			cout << "会员代码:" << pTrade->ParticipantID << endl;
			///客户代码
			cout << "||客户代码:" << pTrade->ClientID << ",";
			///交易角色
			cout << "交易角色:" << pTrade->TradingRole << ",";
			///合约在交易所的代码
			cout << "合约在交易所的代码:" << pTrade->ExchangeInstID << ",";
			///开平标志
			cout << "开平标志:" << pTrade->OffsetFlag << ",";
			///投机套保标志
			cout << "投机套保标志:" << pTrade->HedgeFlag << endl;
			///价格
			cout << "||价格:" << pTrade->Price << ",";
			///数量
			cout << "数量:" << pTrade->Volume << ",";
			///成交时期
			cout << "成交时期:" << pTrade->TradeDate << ",";
			///成交时间
			cout << "成交时间:" << pTrade->TradeTime << ",";
			///成交类型
			cout << "成交类型:" << pTrade->TradeType << endl;
			///成交价来源
			cout << "||成交价来源:" << pTrade->PriceSource << ",";
			///交易所交易员代码
			cout << "交易所交易员代码:" << pTrade->TraderID << ",";
			///本地报单编号
			cout << "本地报单编号:" << pTrade->OrderLocalID << ",";
			///结算会员编号
			cout << "结算会员编号:" << pTrade->ClearingPartID << ",";
			///业务单元
			cout << "业务单元:" << pTrade->BusinessUnit << endl;
			///序号
			cout << "||序号:" << pTrade->SequenceNo << ",";
			///交易日
			cout << "交易日:" << pTrade->TradingDay << ",";
			///结算编号
			cout << "结算编号:" << pTrade->SettlementID << ",";
			///经纪公司报单编号
			cout << "经纪公司报单编号:" << pTrade->BrokerOrderSeq << ",";
			///成交来源
			cout << "成交来源:" << pTrade->TradeSource << endl;
			cout << "=================================================================================" << endl;
		}
	}
}

//下单
void TdSpi::OrderInsert(char *InstrumentID, char CombOffsetFlag, char Direction, int Volume, double Price, string OrderRef) {
	CThostFtdcInputOrderField *pInputOrder = new CThostFtdcInputOrderField();
	memset(pInputOrder, 0, sizeof(CThostFtdcInputOrderField));
	cout << "***************************" << endl;
	cout << "broker id = " << this->getBrokerID().c_str() << endl;
	cout << "user id = " << this->getUserID().c_str() << endl;
	cout << "instrument id = " << InstrumentID << endl;
	cout << "Order Ref = " << OrderRef << endl;
	cout << "Direction = " << THOST_FTDC_D_Buy << endl;
	cout << "CombOffsetFlag = " << CombOffsetFlag << endl;
	cout << "Price = " << Price << endl;
	cout << "Volume = " << Volume << endl;
	cout << "Request id = " << this->getRequestID() << endl;


	///经纪公司代码
	strcpy(pInputOrder->BrokerID, this->getBrokerID().c_str());

	///投资者代码
	strcpy(pInputOrder->InvestorID, this->getUserID().c_str());

	///合约代码
	//string heyue = "cu1609";
	//strcpy(pInputOrder->InstrumentID, "cu1609");
	//cout << "instrument ID c_str()" << InstrumentID.c_str() << endl;

	std::strcpy(pInputOrder->InstrumentID, InstrumentID);

	//memcpy(pInputOrder->InstrumentID, InstrumentID.c_str(), InstrumentID.size() + 1);

	///报单引用
	strcpy(pInputOrder->OrderRef, OrderRef.c_str());

	///用户代码
	//strcpy(pInputOrder->UserID, this->getUserID().c_str());

	///报单价格条件
	//TThostFtdcOrderPriceTypeType	OrderPriceType; //char 任意价 '1'
	pInputOrder->OrderPriceType = THOST_FTDC_OPT_LimitPrice;

	///买卖方向
	//TThostFtdcDirectionType	Direction; //char 0买1卖
	pInputOrder->Direction = Direction;

	///组合开平标志
	//TThostFtdcCombOffsetFlagType	CombOffsetFlag; //char s[5]
	//strcpy(pInputOrder->CombOffsetFlag, CombOffsetFlag); //组合开平标志 开0平1强平2平今3平昨4
	pInputOrder->CombOffsetFlag[0] = CombOffsetFlag;

	///组合投机套保标志
	//TThostFtdcCombHedgeFlagType	CombHedgeFlag; //char s[5]
	//strcpy(pInputOrder->CombHedgeFlag, "1"); //"1"投机, "2"套利, "3"套保
	pInputOrder->CombHedgeFlag[0] = '1';

	///价格
	//TThostFtdcPriceType	LimitPrice; //double
	pInputOrder->LimitPrice = Price;

	///数量
	//TThostFtdcVolumeType	VolumeTotalOriginal; //int
	pInputOrder->VolumeTotalOriginal = Volume;

	///有效期类型
	//TThostFtdcTimeConditionType	TimeCondition; //char 当日有效：'3'
	pInputOrder->TimeCondition = THOST_FTDC_TC_GFD; //当日有效

	///GTD日期
	//TThostFtdcDateType	GTDDate; //char s[9]
	///成交量类型
	//TThostFtdcVolumeConditionType	VolumeCondition; //char 任何数量 '1'
	pInputOrder->VolumeCondition = THOST_FTDC_VC_AV; //任何数量 '1'

	///最小成交量
	//TThostFtdcVolumeType	MinVolume; //int
	pInputOrder->MinVolume = 1;

	///触发条件
	//TThostFtdcContingentConditionType	ContingentCondition; //char 立即 '1'
	pInputOrder->ContingentCondition = THOST_FTDC_CC_Immediately;

	///止损价
	//TThostFtdcPriceType	StopPrice; //double 置为0
	//pInputOrder->StopPrice = 0;

	///强平原因
	//TThostFtdcForceCloseReasonType	ForceCloseReason; //char 非强平 '0'
	pInputOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

	///自动挂起标志
	//TThostFtdcBoolType	IsAutoSuspend; //bool false
	pInputOrder->IsAutoSuspend = 0;
	///业务单元
	//TThostFtdcBusinessUnitType	BusinessUnit; // char s[21]

	///请求编号
	//TThostFtdcRequestIDType	RequestID; //int
	//pInputOrder->RequestID = this->getRequestID();

	///用户强评标志
	//TThostFtdcBoolType	UserForceClose; //int
	pInputOrder->UserForceClose = 0;

	///互换单标志
	//TThostFtdcBoolType	IsSwapOrder; //bool false
	///交易所代码
	//TThostFtdcExchangeIDType	ExchangeID;
	///投资单元代码
	//TThostFtdcInvestUnitIDType	InvestUnitID;
	///资金账号
	//TThostFtdcAccountIDType	AccountID;
	///币种代码
	//TThostFtdcCurrencyIDType	CurrencyID;
	///交易编码
	//TThostFtdcClientIDType	ClientID;
	///IP地址
	//TThostFtdcIPAddressType	IPAddress;
	///Mac地址
	//TThostFtdcMacAddressType	MacAddress;

	this->tdapi->ReqOrderInsert(pInputOrder, this->getRequestID());

	delete pInputOrder;
}

///报单录入请求响应
void TdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspOrderInsert");
	if (!(this->IsErrorRspInfo(pRspInfo))) {
		if (pInputOrder) {
			cout << "=================================================================================" << endl;
			///经纪公司代码
			cout << "经纪公司代码:" << pInputOrder->BrokerID << ", ";
			///投资者代码
			cout << "投资者代码:" << pInputOrder->InvestorID << ", ";
			///合约代码
			cout << "合约代码:" << pInputOrder->InstrumentID << ", ";
			///报单引用
			cout << "报单引用:" << pInputOrder->OrderRef << ", ";
			///用户代码
			cout << "用户代码:" << pInputOrder->UserID << endl;
			///报单价格条件
			cout << "报单价格条件:" << pInputOrder->OrderPriceType << ", ";
			///买卖方向
			cout << "买卖方向:" << pInputOrder->Direction << ", ";
			///组合开平标志
			cout << "组合开平标志:" << pInputOrder->CombOffsetFlag << ", ";
			///组合投机套保标志
			cout << "组合投机套保标志:" << pInputOrder->CombHedgeFlag << ", ";
			///价格
			cout << "价格:" << pInputOrder->LimitPrice << endl;
			///数量
			cout << "数量:" << pInputOrder->VolumeTotalOriginal << ", ";
			///有效期类型
			cout << "有效期类型:" << pInputOrder->TimeCondition << ", ";
			///GTD日期
			cout << "GTD日期:" << pInputOrder->GTDDate << ", ";
			///成交量类型
			cout << "成交量类型:" << pInputOrder->VolumeCondition << ", ";
			///最小成交量
			cout << "最小成交量:" << pInputOrder->MinVolume << endl;
			///触发条件
			cout << "触发条件:" << pInputOrder->ContingentCondition << ", ";
			///止损价
			cout << "止损价:" << pInputOrder->StopPrice << ", ";
			///强平原因
			cout << "强平原因:" << pInputOrder->ForceCloseReason << ", ";
			///自动挂起标志
			cout << "自动挂起标志:" << pInputOrder->IsAutoSuspend << ", ";
			///业务单元
			cout << "业务单元:" << pInputOrder->BusinessUnit << endl;
			///请求编号
			cout << "请求编号:" << pInputOrder->RequestID << ", ";
			///用户强评标志
			cout << "用户强评标志:" << pInputOrder->UserForceClose << ", ";
			///互换单标志
			cout << "互换单标志:" << pInputOrder->IsSwapOrder << endl;
			cout << "=================================================================================" << endl;
		}
	}
}

//下单响应
void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	USER_PRINT("TdSpi::OnRtnOrder");
	if (pOrder) {
		cout << "=================================================================================" << endl;
		///经纪公司代码
		cout << "||经纪公司代码:" << pOrder->BrokerID << ", ";
		///投资者代码
		cout << "投资者代码:" << pOrder->InvestorID << ", ";
		///合约代码
		cout << "合约代码:" << pOrder->InstrumentID << ", ";
		///报单引用
		cout << "报单引用:" << pOrder->OrderRef << ", ";
		///用户代码
		cout << "用户代码:" << pOrder->UserID << endl;
		///报单价格条件
		cout << "||报单价格条件:" << pOrder->OrderPriceType << ", ";
		///买卖方向
		cout << "买卖方向:" << pOrder->Direction << ", ";
		///组合开平标志
		cout << "组合开平标志:" << pOrder->CombOffsetFlag << ", ";
		///组合投机套保标志
		cout << "组合投机套保标志:" << pOrder->CombHedgeFlag << ", ";
		///价格
		cout << "价格:" << pOrder->LimitPrice << endl;
		///数量
		cout << "||数量:" << pOrder->VolumeTotalOriginal << ", ";
		///有效期类型
		cout << "有效期类型:" << pOrder->TimeCondition << ", ";
		///GTD日期
		cout << "GTD日期:" << pOrder->GTDDate << ", ";
		///成交量类型
		cout << "成交量类型:" << pOrder->VolumeCondition << ", ";
		///最小成交量
		cout << "最小成交量:" << pOrder->MinVolume << endl;
		///触发条件
		cout << "||触发条件:" << pOrder->ContingentCondition << ", ";
		///止损价
		cout << "止损价:" << pOrder->StopPrice << ", ";
		///强平原因
		cout << "强平原因:" << pOrder->ForceCloseReason << ", ";
		///自动挂起标志
		cout << "自动挂起标志:" << pOrder->IsAutoSuspend << ", ";
		///业务单元
		cout << "业务单元:" << pOrder->BusinessUnit << endl;
		///请求编号
		cout << "||请求编号:" << pOrder->RequestID << ", ";
		///本地报单编号
		cout << "本地报单编号:" << pOrder->OrderLocalID << ", ";
		///交易所代码
		cout << "交易所代码:" << pOrder->ExchangeID << ", ";
		///会员代码
		cout << "会员代码:" << pOrder->ParticipantID << ", ";
		///客户代码
		cout << "客户代码:" << pOrder->ClientID << endl;
		///合约在交易所的代码
		cout << "||合约在交易所的代码:" << pOrder->ExchangeInstID << ", ";
		///交易所交易员代码
		cout << "交易所交易员代码:" << pOrder->TraderID << ", ";
		///安装编号
		cout << "安装编号:" << pOrder->InstallID << ", ";
		///报单提交状态
		cout << "报单提交状态:" << pOrder->OrderSubmitStatus << ", ";
		///报单提示序号
		cout << "报单提示序号:" << pOrder->NotifySequence << endl;
		///交易日
		cout << "||交易日:" << pOrder->TradingDay << ", ";
		///结算编号
		cout << "结算编号:" << pOrder->SettlementID << ", ";
		///报单编号
		cout << "报单编号:" << pOrder->OrderSysID << ", ";
		///报单来源
		cout << "报单来源:" << pOrder->OrderSource << ", ";
		///报单状态
		cout << "报单状态:" << pOrder->OrderStatus << endl;
		///报单类型
		cout << "||报单类型:" << pOrder->OrderType << ", ";
		///今成交数量
		cout << "今成交数量:" << pOrder->VolumeTraded << ", ";
		///剩余数量
		cout << "剩余数量:" << pOrder->VolumeTotal << ", ";
		///报单日期
		cout << "报单日期:" << pOrder->InsertDate << ", ";
		///委托时间
		cout << "委托时间:" << pOrder->InsertTime << endl;
		///激活时间
		cout << "||激活时间:" << pOrder->ActiveTime << ", ";
		///挂起时间
		cout << "挂起时间:" << pOrder->SuspendTime << ", ";
		///最后修改时间
		cout << "最后修改时间:" << pOrder->UpdateTime << ", ";
		///撤销时间
		cout << "撤销时间:" << pOrder->CancelTime << ", ";
		///最后修改交易所交易员代码
		cout << "最后修改交易所交易员代码:" << pOrder->ActiveTraderID << endl;
		///结算会员编号
		cout << "||结算会员编号:" << pOrder->ClearingPartID << ", ";
		///序号
		cout << "序号:" << pOrder->SequenceNo << ", ";
		///前置编号
		cout << "前置编号:" << pOrder->FrontID << ", ";
		///会话编号
		cout << "会话编号:" << pOrder->SessionID << ", ";
		///用户端产品信息
		cout << "用户端产品信息:" << pOrder->UserProductInfo << endl;
		///状态信息
		cout << "||状态信息:" << pOrder->StatusMsg << ", ";
		///用户强评标志
		cout << "用户强评标志:" << pOrder->UserForceClose << ", ";
		///操作用户代码
		cout << "操作用户代码:" << pOrder->ActiveUserID << ", ";
		///经纪公司报单编号
		cout << "经纪公司报单编号:" << pOrder->BrokerOrderSeq << ", ";
		///相关报单
		cout << "相关报单:" << pOrder->RelativeOrderSysID << endl;
		///郑商所成交数量
		cout << "||郑商所成交数量:" << pOrder->ZCETotalTradedVolume << ", ";
		///互换单标志
		cout << "互换单标志:" << pOrder->IsSwapOrder << endl;
		cout << "=================================================================================" << endl;
	}
}

//下单错误响应
void TdSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
	USER_PRINT("TdSpi::OnErrRtnOrderInsert");
}

//撤单
void TdSpi::OrderAction(string ExchangeID, string OrderRef, string OrderSysID) {
	CThostFtdcInputOrderActionField *pOrderAction = new CThostFtdcInputOrderActionField();
	strcpy(pOrderAction->BrokerID, this->getBrokerID().c_str());
	strcpy(pOrderAction->InvestorID, this->getUserID().c_str());
	//strcpy(f->InstrumentID, "a1501");
	strcpy(pOrderAction->ExchangeID, ExchangeID.c_str());
	strcpy(pOrderAction->OrderRef, OrderRef.c_str());		//设置报单引用
	strcpy(pOrderAction->OrderSysID, OrderSysID.c_str());
	pOrderAction->ActionFlag = THOST_FTDC_AF_Delete; //删除
	this->tdapi->ReqOrderAction(pOrderAction, this->getRequestID());
	delete pOrderAction;
}

//撤单错误响应
void TdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	USER_PRINT("TdSpi::OnRspOrderAction");
	if (!(this->IsErrorRspInfo(pRspInfo))) {

	}
}

//撤单错误
void TdSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
	USER_PRINT("TdSpi::OnErrRtnOrderAction");
	if (!(this->IsErrorRspInfo(pRspInfo))) {

	}
}





//登出
void TdSpi::Logout(char *BrokerID, char *UserID) {
	USER_PRINT("TdSpi::Logout")
	CThostFtdcUserLogoutField *pUserLogout = new CThostFtdcUserLogoutField();
	strcpy(pUserLogout->BrokerID, BrokerID);
	strcpy(pUserLogout->UserID, UserID);
	this->tdapi->ReqUserLogout(pUserLogout, this->loginRequestID);
	int ret = this->controlTimeOut(&logout_sem);
	if (ret == -1) {
		USER_PRINT("TdSpi::Logout TimeOut!")
	}
}

///登出请求响应
void TdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (bIsLast && !(this->IsErrorRspInfo(pRspInfo))) {
		USER_PRINT("TdSpi::OnRspUserLogout")
		sem_post(&logout_sem);
	}
}

//等待线程结束
void TdSpi::Join() {
	USER_PRINT("TdSpi::Join()")
	this->tdapi->Join();
}



///用户口令更新请求响应
void TdSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
                                    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "回调用户口令更新请求响应OnRspUserPasswordUpdate" << endl;
    if (pRspInfo->ErrorID == 0){
        cout << "更改成功 " << endl
        << "旧密码为:" << pUserPasswordUpdate->OldPassword << endl
        << "新密码为:" << pUserPasswordUpdate->NewPassword << endl;
    }
    else{
        cout << pRspInfo->ErrorID << ends << pRspInfo->ErrorMsg << endl;
    }
}

///请求查询行情响应
void TdSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
                                    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "OnRspQryDepthMarketData" << endl;
    cout << nRequestID << endl;
    if (pDepthMarketData != NULL){
        cout << "-----------------行情数据--------------------" << endl;
        cout << "交易日:" << pDepthMarketData->TradingDay << endl
        << "合约代码:" << pDepthMarketData->InstrumentID << endl
        << "最新价:" << pDepthMarketData->LastPrice << endl
        << "最高价:" << pDepthMarketData->HighestPrice << endl
        << "最低价:" << pDepthMarketData->LowestPrice << endl;
        cout << "-----------------行情数据--------------------" << endl;
    }
}

bool TdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

//得到BrokerID
string TdSpi::getBrokerID() {
	return this->BrokerID;
}

char * TdSpi::getCharBrokerID() {

	return this->c_BrokerID;
}


//得到UserID
string TdSpi::getUserID() {
	return this->UserID;
}

char * TdSpi::getCharUserID() {
	return this->c_UserID;
}

//得到Password
string TdSpi::getPassword() {
	return this->Password;
}

char * TdSpi::getCharPassword() {
	return this->c_Password;
}

//得到requestID
int TdSpi::getRequestID() {
	return this->loginRequestID;
}

//得到交易日期
char * TdSpi::getCharTradingDate() {
	return (const_cast<char *> (this->tdapi->GetTradingDay()));
}

//设置isConfirmSettlement
void TdSpi::setIsConfirmSettlement(bool isConfirmSettlement) {
	this->isConfirmSettlement = isConfirmSettlement;
}

//得到isConfirmSettlement
bool TdSpi::getIsConfirmSettlement() {
	return this->isConfirmSettlement;
}

//释放api
void TdSpi::Release() {
	this->tdapi->Release();
}