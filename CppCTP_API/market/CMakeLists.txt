    cmake_minimum_required(VERSION 2.8)
    #project name
    project(quant_ctp_md)

    #set compiler for c++ language
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

    #source directory
    set(SOURCE_FILES main.cpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h MdSpi.cpp MdSpi.h)

    #set extern libraries
    set(LIBRARIES libthostmduserapi.so)

    #set extern libraries
    #set(LIBRARIES libthostmduserapi.so libthosttraderapi.so)

    #add execute file
    add_executable(quant_ctp_md ${SOURCE_FILES})

    #add link library
    target_link_libraries(quant_ctp_md ${LIBRARIES})