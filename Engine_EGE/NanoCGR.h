
//NanoCGR.h
//
//

#pragma once

#ifndef _WINSOCK2API_
#define _WINSOCK2API_
//#define _WINSOCKAPI_ 
#endif

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")		// Socket编程需用的动态链接库

class NanoCGR {
public:
	NanoCGR();
	~NanoCGR();

	int Init();

	int Send(const char * str);

	static DWORD WINAPI SendMessageThread(LPVOID IpParameter);
	static DWORD WINAPI ReceiveMessageThread(LPVOID IpParameter);
};