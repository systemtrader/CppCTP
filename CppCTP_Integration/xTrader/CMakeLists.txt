    cmake_minimum_required(VERSION 2.8)
    #project name
    project(quant_ctp_xTrader)

    #set compiler for c++ language
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -D_GLIBCXX_USE_NANOSLEEP")

    #source directory
    set(SOURCE_FILES main.cpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h MdSpi.cpp MdSpi.h TdSpi.cpp TdSpi.h)

    #set extern libraries
    set(LIBRARIES libthostmduserapi.so libthosttraderapi.so)

    #set extern libraries
    #set(LIBRARIES libthostmduserapi.so libthosttraderapi.so)

    #add execute file
    add_executable(quant_ctp_XTrader ${SOURCE_FILES})

    #add link library
    target_link_libraries(quant_ctp_XTrader ${LIBRARIES})