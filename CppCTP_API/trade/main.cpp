#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "TdSpi.h"
using namespace std;

using std::map;
using std::string;

void printMenuEN() {
	cout << "|==========================|" << endl;
	cout << "|Please Input Your Choice: |" << endl;
	cout << "|i:Order Insert            |" << endl;
	cout << "|a:Order Action            |" << endl;
	cout << "|b:Break                   |" << endl;
	cout << "|e:Qry Exchange            |" << endl;
	cout << "|s:Qry Instrument          |" << endl;
	cout << "|o:Qry Order               |" << endl;
	cout << "|t:Qry Trading Account     |" << endl;
	cout << "|u:Qry Investor            |" << endl;
	cout << "|d:Qry Trade               |" << endl;
	cout << "|h:Qry Investor Position   |" << endl;
	cout << "|==========================|" << endl;
}

void printMenu() {
	cout << "|==============================|" << endl;
	cout << "|请输入您的操作编号:           |" << endl;
	cout << "|【u】:账户信息查询            |" << endl;
	cout << "|【t】:账户资金查询            |" << endl;
	cout << "|【s】:查询合约信息            |" << endl;
	cout << "|【d】:查询交易信息            |" << endl;
	cout << "|【h】:查询持仓信息            |" << endl;
	cout << "|【i】:报单录入                |" << endl;
	cout << "|【o】:报单查询                |" << endl;
	cout << "|【a】:撤单操作                |" << endl;
	cout << "|【e】:查询交易所代码          |" << endl;
	cout << "|【b】:退出程序                |" << endl;
	cout << "|==============================|" << endl;
}

void printContinue() {
	cout << "|======================|" << endl;
	cout << "|请输入【c】继续操作   |" << endl;
	cout << "|======================|" << endl;
}

void printContinueEN() {
	cout << "|======================|" << endl;
	cout << "|Input 'c' to Continue:|" << endl;
	cout << "|======================|" << endl;
}

int main() {
	CThostFtdcTraderApi *tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/quant/Desktop/demo2/");
	TdSpi *tdspi = new TdSpi(tdapi);

	//注册前置机
	//tdapi->RegisterFront("tcp://180.169.75.19:41205");	//实盘270338
	/**
	 * 标准CTP：
	 第一组：Trade Front：180.168.146.187:10000，Market Front：180.168.146.187:10010；【电信】
	 第二组：Trade Front：180.168.146.187:10001，Market Front：180.168.146.187:10011；【电信】
	 第三组：Trade Front：218.202.237.33 :10002，Market Front：218.202.237.33 :10012；【移动】
	 第二套：
	 交易前置：180.168.146.187:10030，行情前置：180.168.146.187:10031；【7x24】
	 实盘：
	 交易前置地址：101.95.8.190:41205
	 行情前置地址：101.95.8.190:41213
	 实盘账号：86001525/206029
	 */

	tdspi->Connect("tcp://180.168.146.187:10000"); //仿真
	//tdspi->Connect("tcp://180.168.146.187:10030"); //24H
	sleep(1);
	tdspi->Login("9999", "058176", "669822");

	//sleep(2);
	//tdspi->Login("9999", "063802", "123456");
	//tdspi->Login("9999", "063802", "123456");

	/**
	实盘
	*/
	//连接
	//tdspi->Connect("tcp://220.248.44.146:41205");
	//sleep(1);
	//登录
	//tdspi->Login("0187", "86001525", "206029");
	//tdspi->Login("0187", "801859", "162860");
	sleep(1);
	//查询交易结算确认
	tdspi->QrySettlementInfoConfirm(const_cast<char *>(tdspi->getBrokerID().c_str()), const_cast<char *>(tdspi->getUserID().c_str()), tdspi->getRequestID());
	//查询交易所
	sleep(4);
	//tdspi->QryExchange();
	//查询合约
	//sleep(1);
	//tdspi->QryInstrument();

	//查询报单
	//sleep(1);
	//tdspi->QryOrder();

	//sleep(1);
	//cout << "oh?" << endl;

	//查询账户资金
	//sleep(1);
	//tdspi->QryTradingAccount();

	//查询保证金率
	//sleep(1);
	//tdspi->QryInstrumentMarginRate();

	//查询手续费率
	//sleep(1);
	//tdspi->QryInstrumentCommissionRate();

	//查询账户
	//sleep(1);
	//tdspi->QryInvestor();

	//查询账户持仓情况
	//sleep(1);
	//tdspi->QryInvestorPosition();

	//查询成交单
	//sleep(1);
	//tdspi->QryTrade();

	//sleep(1);
	//tdspi->OrderInsert();

	char input_c;
	char innner_input_c;
	/*order insert*/
	string order_InstrumentID;
	char order_CombOffsetFlag;
	char order_Direction;
	int order_Volume;
	double order_Price;
	string order_OrderRef;

	/*order action delete*/
	string action_ExchangeId;
	int action_int_ExchangeId;
	string action_OrderRef;
	string action_OrderSysId;

	/*qry vars*/
	string qry_ExchangeId;
	string qry_InstrumentId;
	int qry_Flag;

	printMenu();
	//测试下单，撤单
	while (1) {
		input_c = getchar();
		if (input_c == 'i') {
			cout << "Order Insert Operation" << endl;
			/************************************************************************/
			/* Code Below  Order Insert                                             */
			/************************************************************************/
			cout << "Please input instrumentID, such as cu1609, zn1701..." << endl;
			cin >> order_InstrumentID;
			cout << "Please input Comboffsetflag" << endl;
			cout << "0:Open 1:Close 2:ForceClose 3:CloseToday 4:CloseYesterday" << endl;
			cin >> order_CombOffsetFlag;
			cout << "Please input Direction" << endl;
			cout << "0:Buy 1:Sell" << endl;
			cin >> order_Direction;
			cout << "Please input Volume, such as 10, 50, 100..." << endl;
			cin >> order_Volume;
			cout << "Please input Price" << endl;
			cin >> order_Price;
			cout << "Please input OrderRef, such as 1, 2, 3 or user specific" << endl;
			cin >> order_OrderRef;

			cout << "order_InstrumentID = " << order_InstrumentID << endl;
			cout << "order_CombOffsetFlag = " << order_CombOffsetFlag << endl;
			cout << "order_Direction = " << order_Direction << endl;
			cout << "order_Volume = " << order_Volume << endl;
			cout << "order_Price = " << order_Price << endl;
			cout << "order_OrderRef = " << order_OrderRef << endl;

			tdspi->OrderInsert(const_cast<char *>(order_InstrumentID.c_str()), order_CombOffsetFlag, order_Direction, order_Volume, order_Price, order_OrderRef);
			/************************************************************************/
			/* Code Finished                                                        */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'a') {
			cout << "Order Action Operation" << endl;
			/************************************************************************/
			/* Code Below  Order Action                                             */
			/************************************************************************/
			cout << "Pleas Input ExchanageID" << endl;
			cout << "1:SHFE 2:DCE 3:CZCE 4:CFFEX" << endl;
			cin >> action_int_ExchangeId;
			cout << "Pleas Input OrderRef" << endl;
			cin >> action_OrderRef;
			cout << "Pleas Input OrderSysId" << endl;
			cin.ignore(0x7fffffff, '\n');
			getline(cin, action_OrderSysId);

			switch (action_int_ExchangeId)
			{
			case 1:action_ExchangeId = "SHFE"; break;
			case 2:action_ExchangeId = "DCE"; break;
			case 3:action_ExchangeId = "CZCE"; break;
			case 4:action_ExchangeId = "CFFEX"; break;
			default:
				action_ExchangeId = "";
				break;
			}

			cout << "action_ExchangeId = " << action_ExchangeId << endl;
			cout << "action_OrderRef = " << action_OrderRef << endl;
			cout << "action_OrderSysId = " << action_OrderSysId << endl;

			tdspi->OrderAction(action_ExchangeId, action_OrderRef, action_OrderSysId);

			/************************************************************************/
			/* Code Finished                                                        */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'b') {
			cout << "拜拜~" << endl;
			break;
		}
		else if (input_c == 's') {
			cout << "Qry Instrument Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/
			cout << "Query All or Query One?" << endl;
			cout << "0:Query All 1:Query One" << endl;
			cin >> qry_Flag;
			if (qry_Flag) {
				cout << "Please Input ExchangeId:" << endl;
				cout << "1:SHFE 2:DCE 3:CZCE 4:CFFEX" << endl;
				cin >> qry_Flag;
				switch (action_int_ExchangeId)
				{
				case 1:qry_ExchangeId = "SHFE"; break;
				case 2:qry_ExchangeId = "DCE"; break;
				case 3:qry_ExchangeId = "CZCE"; break;
				case 4:qry_ExchangeId = "CFFEX"; break;
				default:
					qry_ExchangeId = "";
					break;
				}
				cout << "Please Input InstrumentId:" << endl;
				cin >> qry_InstrumentId;

			}
			else {
				qry_InstrumentId = "";
				qry_ExchangeId = "";
			}

			cout << "qry_ExchangeId = " << qry_ExchangeId << endl;
			cout << "qry_InstrumentId = " << qry_InstrumentId << endl;

			tdspi->QryInstrument(qry_ExchangeId, qry_InstrumentId);

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'e') {
			cout << "Qry Exchange Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryExchange();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'o') {
			cout << "Qry Order Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryOrder();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 't') {
			cout << "Qry Trading Account Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryTradingAccount();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'u') {
			cout << "Qry Investor Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryInvestor();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'd') {
			cout << "Qry Trade Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryTrade();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'h') {
			cout << "Qry Investor Position Operation" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/

			tdspi->QryInvestorPosition();

			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
		else if (input_c == 'w') {
			cout << "" << endl;
			/************************************************************************/
			/* code below here                                                                     */
			/************************************************************************/


			/************************************************************************/
			/* code finished                                                                     */
			/************************************************************************/
			printContinue();
			getchar();
			innner_input_c = getchar();
			if (innner_input_c == 'c') {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
			else {
				input_c = ' ';
				innner_input_c = ' ';
				printMenu();
				continue;
			}
		}
	}
	//sleep(1);
	//tdspi->Join();
	//sleep(1);
	tdspi->Release();
	return 0;
}