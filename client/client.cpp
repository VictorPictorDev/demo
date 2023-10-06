#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>
//#include<string>

#pragma comment(lib, "ws2_32.lib")
#define BUFF_SIZE 1024
using namespace std;

void check_sending(short& packet_size, SOCKET clientSock);
void check_receiving(short& packet_size, SOCKET clientSock);

int main()
{
	setlocale(LC_ALL, "ru");
	// WinSock initialization
	WSADATA wsData;
	int erState = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (erState)
	{
		cout << "Error WinSock version initialization # " << GetLastError() << endl;
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Server socket initialization
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET)
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Client socket initialization is OK" << endl;

	// Establishing a connection to Server
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_port = htons(1024);

	in_addr ip_to_num;
	erState = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
	if (erState <= 0)
	{
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	servInfo.sin_addr = ip_to_num;

	erState = connect(clientSock, (SOCKADDR*)&servInfo, sizeof(servInfo));
	if (erState)
	{
		cout << "Connection to server is failed. Error # " << WSAGetLastError() << endl;
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Connected to server." << endl;
	}


	//Exchange text data between Server and Client. Disconnection if a Client send "xxx"

	const int packageMessageSize = 1024;
	vector <char>  servBuff(packageMessageSize);								// Creation of buffers for sending and receiving data
	short packet_size = 0;													// The size of sending / receiving packet in bytes

	while (true) {

		cout << "Your (Client) message to Server: ";
		char message[1024];
		gets_s(message); 

		vector <char> clientBuff(message, message + strlen(message));
		string str_message(message);

		if (str_message == "") {
			cout << "cannot enter empty string." << endl;
			continue;
		}

		packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);
		check_sending(packet_size, clientSock);
		clientBuff.clear();

		// Check whether client like to stop chatting 
		if (str_message == "exit") {
			shutdown(clientSock, SD_BOTH);
			closesocket(clientSock);
			WSACleanup();
			return 0;
		}
		else
		{
			if (str_message == "createDirectory")
			{
				servBuff.resize(packageMessageSize);
				packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);
				check_receiving(packet_size, clientSock);
				cout << "Server message: " << servBuff.data() << endl;
				servBuff.clear();

				gets_s(message);
				vector <char> clientBuff(message, message + strlen(message));

				packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);
				check_sending(packet_size, clientSock);
				clientBuff.clear();
			}
			else if (str_message == "deleteDirectory")
			{
				servBuff.resize(packageMessageSize);
				packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);
				check_receiving(packet_size, clientSock);
				cout << "Server message: " << servBuff.data() << endl;
				servBuff.clear();

				gets_s(message);
				vector <char> clientBuff(message, message + strlen(message));

				packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);
				check_sending(packet_size, clientSock);
				clientBuff.clear();
			}
			else if (str_message == "createFile")
			{
				servBuff.resize(packageMessageSize);
				packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);
				check_receiving(packet_size, clientSock);;
				cout << "Server message: " << servBuff.data() << endl;
				servBuff.clear();

				gets_s(message);
				vector <char> clientBuff(message, message + strlen(message));

				packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);
				check_sending(packet_size, clientSock);
				clientBuff.clear();
			}
			else if (str_message == "deleteFile")
			{
				servBuff.resize(packageMessageSize);
				packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);
				check_receiving(packet_size, clientSock);;
				cout << "Server message: " << servBuff.data() << endl;
				servBuff.clear();

				gets_s(message);
				vector <char> clientBuff(message, message + strlen(message));

				packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);
				check_sending(packet_size, clientSock);
				clientBuff.clear();
			}
		}

		servBuff.resize(packageMessageSize);
		packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);
		check_receiving(packet_size, clientSock);
		cout << "Server message: " << servBuff.data() << endl;
		servBuff.clear();

	}

	closesocket(clientSock);
	WSACleanup();

	return 0;
}

void check_sending(short& packet_size, SOCKET clientSock)
{
	if (packet_size == SOCKET_ERROR) {
		cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
		closesocket(clientSock);
		WSACleanup();
		exit(1);
	}
}

void check_receiving(short& packet_size, SOCKET clientSock)
{
	if (packet_size == SOCKET_ERROR)
	{
		cout << "Can't receive message from Server. Error # " << WSAGetLastError() << endl;
		closesocket(clientSock);
		WSACleanup();
		exit(1);
	}
}