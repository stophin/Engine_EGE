// NanoCGR.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "NanoCGR.h"
#include "extern/Loading.h"
extern World world;
extern ResourceManager resm;

SOCKET sockClient;		// ���ӳɹ�����׽���
HANDLE bufferMutex;		// �����ܻ���ɹ�����ͨ�ŵ��ź������
const int DefaultPort = 9005;//9005

NanoCGR::NanoCGR()
{
}

NanoCGR::~NanoCGR() {

}

int NanoCGR::Init() {
	// ����socket��̬���ӿ�(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// ��ṹ�����ڽ���Wjndows Socket�Ľṹ��Ϣ��
	wVersionRequested = MAKEWORD(2, 2);	// ����2.2�汾��WinSock��
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {	// ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // ���汾���Ƿ���ȷ
		WSACleanup();
		return -1;
	}

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockClient
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET) {
		//printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// ���׽���sockClient��Զ����������
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// ��һ����������Ҫ�������Ӳ������׽���
	// �ڶ����������趨����Ҫ���ӵĵ�ַ��Ϣ
	// ��������������ַ�ĳ���
	SOCKADDR_IN addrSrv;
	const char * addr = "127.0.0.1";
	//const char * addr = "192.168.91.129";
	addrSrv.sin_addr.S_un.S_addr = inet_addr(addr);		// ���ػ�·��ַ��127.0.0.1; 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(DefaultPort);
	while (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))){
		// �����û�����Ϸ�������Ҫ������
		//cout << "����������ʧ�ܣ��Ƿ��������ӣ���Y/N):";
		//char choice;
		//while (cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N')))){
		//	cout << "�����������������:";
		//	cin.sync();
		//	cin.clear();
		//}
		//if (choice == 'Y'){
		//	continue;
		//}
		//else{
		//	cout << "�˳�ϵͳ��...";
		//	system("pause");
		//	return 0;
		//}
		return 0;
	}
	//cin.sync();
	//cout << "���ͻ�����׼���������û���ֱ�����������������������Ϣ��\n";

	//send(sockClient, "\nAttention: A Client has enter...\n", 200, 0);

	bufferMutex = CreateSemaphore(NULL, 1, 1, NULL);

	//HANDLE sendThread = CreateThread(NULL, 0, NanoCGR::SendMessageThread, NULL, 0, NULL);
	HANDLE receiveThread = CreateThread(NULL, 0, NanoCGR::ReceiveMessageThread, NULL, 0, NULL);


	//WaitForSingleObject(sendThread, INFINITE);  // �ȴ��߳̽���
	//WaitForSingleObject(receiveThread, INFINITE);  // �ȴ��߳̽���
	//closesocket(sockClient);
	//CloseHandle(sendThread);
	CloseHandle(receiveThread);
	//CloseHandle(bufferMutex);
	//WSACleanup();	// ��ֹ���׽��ֿ��ʹ��

	//printf("End linking...\n");
	//printf("\n");
	//system("pause");
	return 0;
}

int NanoCGR::Send(const char * str) {
	if (NULL == str) {
		return -1;
	};
	WaitForSingleObject(bufferMutex, INFINITE);		// P����Դδ��ռ�ã� 
	send(sockClient, str, strlen(str), 0);
	ReleaseSemaphore(bufferMutex, 1, NULL);		// V����Դռ����ϣ� 

	return 0;
}


DWORD WINAPI NanoCGR::SendMessageThread(LPVOID IpParameter)
{
	while (1){
		string talk;
		getline(cin, talk);
		WaitForSingleObject(bufferMutex, INFINITE);		// P����Դδ��ռ�ã�  
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
		send(sockClient, talk.c_str(), 200, 0);	// ������Ϣ
		ReleaseSemaphore(bufferMutex, 1, NULL);		// V����Դռ����ϣ� 
	}
	return 0;
}

void constServer(const char * str);

DWORD WINAPI NanoCGR::ReceiveMessageThread(LPVOID IpParameter)
{
	while (1){
		char recvBuf[300];
		int ret = recv(sockClient, recvBuf, 200, 0);
		WaitForSingleObject(bufferMutex, INFINITE);		// P����Դδ��ռ�ã�  

		//printf("%s Says: %s", "Server", recvBuf);		// ������Ϣ
		recvBuf[ret] = 0;
		constServer(recvBuf);

		ReleaseSemaphore(bufferMutex, 1, NULL);		// V����Դռ����ϣ� 
	}

	closesocket(sockClient);
	CloseHandle(bufferMutex);
	WSACleanup();	// ��ֹ���׽��ֿ��ʹ��

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



