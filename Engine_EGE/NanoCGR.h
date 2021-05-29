
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

class CharString {
public:
	static bool CharString::is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
	static int base64_decode(const char*  encoded_string, int in_len, char * res) {
		static const char * base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		int ret = 0;

		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++) {
					unsigned char t = (unsigned char)-1;//string::npos (unsigned char)base64_chars.find(char_array_4[i]);
					for (int j = strlen(base64_chars) - 1; j >= 0; j--) {
						if (char_array_4[i] == base64_chars[j]) {
							t = j;
							break;
						}
					}
					char_array_4[i] = t;
				}

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					res[ret++] = char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;

			for (j = 0; j < 4; j++) {
				unsigned char t = (unsigned char)-1;//string::npos (unsigned char)base64_chars.find(char_array_4[j]);
				for (int k = strlen(base64_chars) - 1; k >= 0; k--) {
					if (char_array_4[j] == base64_chars[k]) {
						t = k;
						break;
					}
				}
				char_array_4[j] = t;
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) res[ret++] = char_array_3[j];
		}
		res[ret] = 0;
		return ret;
	}
	static int base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len, char * res) {
		static const char * base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		int ret = 0;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					res[ret++] = base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				res[ret++] = base64_chars[char_array_4[j]];

			while ((i++ < 3))
				res[ret++] = '=';

		}
		res[ret] = 0;
		return ret;
	}
	///////////////////////////////////////////////////////
	template <typename T>
	static int EncodeProtocolString(char * str, T val) {
		int size = sizeof(val);
		*((T*)str) = val;
		return size;
	}
	template <typename T>
	static int DecodeProtocolString(char *str, T& val) {
		int size = sizeof(val);
		val = *((T*)str);
		return size;
	}

	//可变参数宏的递归扩展
#define FL_EXPAND(x) x

#define FL_INTERNAL_ARG_COUNT_PRIVATE(\
	 _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, \
	_10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
	_20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
	_30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
	_40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
	_50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
	_60, _61, _62, _63, _64, N, ...) N

#define FL_ARG_COUNT(...) FL_EXPAND(FL_INTERNAL_ARG_COUNT_PRIVATE(0, ##__VA_ARGS__,\
	64, 63, 62, 61, 60, \
	59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
	49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
	39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
	 9,  8,  7,  6,  5,  4,  3,  2,  1,  0))

#define FL_CONCAT_(l, r) l ## r

#define FL_CONCAT(l, r) FL_CONCAT_(l, r)

#define FL_DOARG0(s,f,a,o)
#define FL_DOARG1(s,f,a,v1,v2,v3,v,...) f(a,v,v1,v2,v3)
#define FL_DOARG2(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG1(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG3(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG2(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG4(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG3(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG5(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG4(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG6(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG5(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG7(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG6(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG8(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG7(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG9(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG8(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG10(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG9(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG11(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG10(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG12(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG11(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG13(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG12(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG14(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG13(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG15(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG14(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG16(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG15(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG17(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG16(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG18(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG17(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG19(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG18(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG20(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG19(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG21(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG20(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG22(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG21(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG23(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG22(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG24(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG23(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG25(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG24(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG26(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG25(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG27(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG26(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG28(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG27(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG29(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG28(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG30(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG29(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG31(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG30(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG32(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG31(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG33(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG32(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG34(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG33(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG35(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG34(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG36(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG35(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG37(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG36(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG38(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG37(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG39(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG38(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG40(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG39(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG41(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG40(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG42(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG41(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG43(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG42(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG44(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG43(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG45(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG44(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG46(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG45(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG47(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG46(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG48(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG47(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG49(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG48(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG50(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG49(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG51(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG50(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG52(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG51(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG53(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG52(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG54(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG53(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG55(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG54(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG56(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG55(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG57(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG56(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG58(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG57(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG59(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG58(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG60(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG59(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG61(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG60(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG62(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG61(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG63(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG62(s,f,a,v1,v2,v3,__VA_ARGS__))
#define FL_DOARG64(s,f,a,v1,v2,v3,v,...) FL_DOARG1(s,f,a,v1,v2,v3,v) s FL_EXPAND(FL_DOARG63(s,f,a,v1,v2,v3,__VA_ARGS__))

#define FL_FOREACH_(sepatator,fun,funarg,funarg1,funarg2,funarg3,...) \
		FL_EXPAND(FL_CONCAT(FL_DOARG,FL_ARG_COUNT(__VA_ARGS__))(sepatator,fun,funarg,funarg1,funarg2,funarg3,__VA_ARGS__))
#define FL_FOREACH(sepatator,fun,funarg,...) \
		FL_EXPAND(FL_FOREACH_(sepatator,fun,funarg,,,,__VA_ARGS__))

#define enum_elem(p,n,...) p##n,

#define FL_DEF_ENUM(clsName,...)\
enum _##clsName{\
	FL_EXPAND(FL_FOREACH(,enum_elem,clsName##_,__VA_ARGS__))\
	clsName##_LAST_ID \
}clsName;

#define call_elem(p, n,...) p(n),

#define encode_protocol(p, n, str, offset, size) ;offset += p::EncodeProtocolString(str + offset, n);if (offset >= size) offset = size;
#define EncodeProtocol(st, str, offset, size, ...) \
	FL_EXPAND(FL_FOREACH_(, encode_protocol, st, str, offset, size, __VA_ARGS__))

#define decode_protocol(p, n, str, offset, size) ;offset += p::DecodeProtocolString(str + offset, n);if (offset >= size) offset = size;
#define DecodeProtocol(st, str, offset, size, ...) \
	FL_EXPAND(FL_FOREACH_(, decode_protocol, st, str, offset, size, __VA_ARGS__))
	/////////////////////////////////////////////////////
};

typedef enum NanoCGR_Protocol_tag {
	Nano_Login,
	Nano_Logout,
	Nano_Position
} NanoCGR_Protocol;