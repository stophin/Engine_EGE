// NanoCGR.cpp : 定义控制台应用程序的入口点。
//

#include "NanoCGR.h"
#include "extern/Loading.h"
extern World world;
extern ResourceManager resm;

SOCKET sockClient;		// 连接成功后的套接字
HANDLE bufferMutex;		// 令其能互斥成功正常通信的信号量句柄
const int DefaultPort = 9005;//9005

NanoCGR::NanoCGR()
{
}

NanoCGR::~NanoCGR() {

}

int NanoCGR::Init() {
	// 加载socket动态链接库(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// 这结构是用于接收Wjndows Socket的结构信息的
	wVersionRequested = MAKEWORD(2, 2);	// 请求2.2版本的WinSock库
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {	// 返回值为零的时候是表示成功申请WSAStartup
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // 检查版本号是否正确
		WSACleanup();
		return -1;
	}

	// 创建socket操作，建立流式套接字，返回套接字号sockClient
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET) {
		//printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// 将套接字sockClient与远程主机相连
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// 第一个参数：需要进行连接操作的套接字
	// 第二个参数：设定所需要连接的地址信息
	// 第三个参数：地址的长度
	SOCKADDR_IN addrSrv;
	const char * addr = "127.0.0.1";
	//const char * addr = "192.168.91.129";
	addrSrv.sin_addr.S_un.S_addr = inet_addr(addr);		// 本地回路地址是127.0.0.1; 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(DefaultPort);
	while (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))){
		// 如果还没连接上服务器则要求重连
		//cout << "服务器连接失败，是否重新连接？（Y/N):";
		//char choice;
		//while (cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N')))){
		//	cout << "输入错误，请重新输入:";
		//	cin.sync();
		//	cin.clear();
		//}
		//if (choice == 'Y'){
		//	continue;
		//}
		//else{
		//	cout << "退出系统中...";
		//	system("pause");
		//	return 0;
		//}
		return 0;
	}
	//cin.sync();
	//cout << "本客户端已准备就绪，用户可直接输入文字向服务器反馈信息。\n";

	//send(sockClient, "\nAttention: A Client has enter...\n", 200, 0);

	bufferMutex = CreateSemaphore(NULL, 1, 1, NULL);

	//HANDLE sendThread = CreateThread(NULL, 0, NanoCGR::SendMessageThread, NULL, 0, NULL);
	HANDLE receiveThread = CreateThread(NULL, 0, NanoCGR::ReceiveMessageThread, NULL, 0, NULL);


	//WaitForSingleObject(sendThread, INFINITE);  // 等待线程结束
	//WaitForSingleObject(receiveThread, INFINITE);  // 等待线程结束
	//closesocket(sockClient);
	//CloseHandle(sendThread);
	CloseHandle(receiveThread);
	//CloseHandle(bufferMutex);
	//WSACleanup();	// 终止对套接字库的使用

	//printf("End linking...\n");
	//printf("\n");
	//system("pause");
	return 0;
}

int NanoCGR::Send(const char * str) {
	if (NULL == str) {
		return -1;
	};
	WaitForSingleObject(bufferMutex, INFINITE);		// P（资源未被占用） 
	send(sockClient, str, strlen(str), 0);
	ReleaseSemaphore(bufferMutex, 1, NULL);		// V（资源占用完毕） 

	return 0;
}


DWORD WINAPI NanoCGR::SendMessageThread(LPVOID IpParameter)
{
	while (1){
		string talk;
		getline(cin, talk);
		WaitForSingleObject(bufferMutex, INFINITE);		// P（资源未被占用）  
		if ("quit" == talk){
			talk.push_back('\0');
			send(sockClient, talk.c_str(), 200, 0);
			break;
		}
		else{
			talk.append("\n");
		}
		printf("\nI Say:(\"quit\"to exit):");
		cout << talk;
		send(sockClient, talk.c_str(), 200, 0);	// 发送信息
		ReleaseSemaphore(bufferMutex, 1, NULL);		// V（资源占用完毕） 
	}
	return 0;
}

void constServer(const char * str);

DWORD WINAPI NanoCGR::ReceiveMessageThread(LPVOID IpParameter)
{
	while (1){
		char recvBuf[300];
		int ret = recv(sockClient, recvBuf, 200, 0);
		WaitForSingleObject(bufferMutex, INFINITE);		// P（资源未被占用）  

		//printf("%s Says: %s", "Server", recvBuf);		// 接收信息
		recvBuf[ret] = 0;
		constServer(recvBuf);

		ReleaseSemaphore(bufferMutex, 1, NULL);		// V（资源占用完毕） 
	}

	closesocket(sockClient);
	CloseHandle(bufferMutex);
	WSACleanup();	// 终止对套接字库的使用

	return 0;
}

void constServer(const char *str) {
	char _str[100];
	int len = strlen(str);
	CharString::base64_decode(str, len, _str);
	NanoCGR_Protocol p;
	int offset = 0;
	int size = 100;
	DecodeProtocol(CharString, _str, offset, size, p);
	INT sessionID;
	Roles * role;
	float x, y;
	switch (p) {
	case Nano_Login:
		DecodeProtocol(CharString, _str, offset, size, sessionID);
		printf("Login %d\n", sessionID);

		if (sessionID < 0) {
			sessionID = -sessionID;
			world.focus->uniqueID = sessionID;
			break;
		}

		role = world.roles.getLink(sessionID);
		if (!role) {
			role = Loading::loadRole(resm, 0.5, "./scene/player.txt");
			role->setFlatting(RectF(100, 100, 40, 25), role->tall);
			world.addRole(role, Role_Type::Player);
			//world.focus = role;
			role->uniqueID = sessionID;
			role->following = 0;
		}
		break;
	case Nano_Logout:
		DecodeProtocol(CharString, _str, offset, size, sessionID);
		printf("Logout %d\n", sessionID);

		role = world.roles.getLink(sessionID);
		if (role) {
			role = world.removeRole(role);
			printf("remove %p\n", role);
		}
		break;
	case Nano_Position:
		DecodeProtocol(CharString, _str, offset, size, sessionID, x, y);
		printf("Position %d %.2f %.2f\n", sessionID, x, y);

		role = world.roles.getLink(sessionID);
		if (role) {
			role->moveDelta((x + world.geometry.X - role->flatting.X) / role->flatting.Width, (y + world.geometry.Y - role->flatting.Y) / role->flatting.Height);
		}
		break;
	default:
		printf("Unknown\n");
		break;
	}
}



