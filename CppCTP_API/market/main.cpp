#include <iostream>
#include <string.h>
#include "ThostFtdcMdApi.h"
#include "MdSpi.h"

using namespace std;
using std::map;
using std::string;

int main() {
    CThostFtdcMdApi *mdapi = CThostFtdcMdApi::CreateFtdcMdApi("/home/quant/Desktop/demo/");
    MdSpi *mdspi = new MdSpi(mdapi);
    //和前置机连接
	/************************************************************************/
	/* 标准CTP：

		第一组：
		Trade Front：180.168.146.187:10000，
		Market Front：180.168.146.187:10010；【电信】
		第二套：
		交易前置：180.168.146.187:10030，
		行情前置：180.168.146.187:10031；【7x24】
		*/
	/************************************************************************/
    //mdapi->RegisterFront("tcp://180.168.146.187:10010"); //标准
    //mdapi->RegisterFront("tcp://180.168.146.187:10031"); //24H
    //std::cout << "1\n";
    //mdapi->Init();

	mdspi->Connect("tcp://180.168.146.187:10010"); //Standard
	//mdspi->Connect("tcp://180.168.146.187:10031"); //24H
	mdspi->Login("9999", "058176", "669822");
	//订阅合约所以数量为3
	string array[] = { "cu1608", "cu1609", "zn1608", "zn1609" };
	cout << "total string size is:" << sizeof(array) / sizeof(string) << endl;
	int size = sizeof(array) / sizeof(string);
	char **instrumentID = new char *[size];
	int i;
	for (i = 0; i < size; i++) {
		const char *charResult = array[i].c_str();
		instrumentID[i] = new char[strlen(charResult) + 1];
		strcpy(instrumentID[i], charResult);
		//cout << instrumentID[i] << endl;
		//usleep(500000);
	}

	//cout << instrumentID << endl;

	mdspi->SubMarketData(instrumentID, size);
	//usleep(5000000);
	//mdspi->UnSubscribeMarketData(instrumentID, 3);
	//usleep(5000000);
	//mdspi->Logout("9999", "058176");
	mdspi->Join();
    //mdapi->Release();
}
