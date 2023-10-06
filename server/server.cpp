#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include "filesystem.h"
#include "sstream"
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#define BUFF_SIZE 1024
#define USERS_FOLDER_NAME "users"

using namespace std;

void check_sending(short& packet_size, SOCKET clientConn, SOCKET servSock);
void check_receiving(short& packet_size, SOCKET clientConn, SOCKET servSock);
void send_message(string message, SOCKET clientConn, SOCKET servSock);
vector<string> split_string(string input);

int main()
{
	setlocale(LC_ALL, "ru");
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
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(servSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	// Server socket binding
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

	erState = bind(servSock, (SOCKADDR*)&servInfo, sizeof(servInfo));
	if (erState)
	{
		cout << "Error socket binding to server info. Errrr # " << WSAGetLastError() << endl;
		closesocket(servSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to server info is OK" << endl;

	//Starting to listen to any Clients
	erState = listen(servSock, SOMAXCONN);
	if (erState)
	{
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(servSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening ..." << endl;
		try {
			createDirectory(USERS_FOLDER_NAME);
		}
		catch (const std::exception& e) {
			cout << "Create folder error: " << e.what() << std::endl;
		}
	}

	//Client socket creation and acception in case of connection
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);
	SOCKET clientConn = accept(servSock, (SOCKADDR*)&clientInfo, &clientInfo_size);
	if (clientConn == INVALID_SOCKET)
	{
		cout << "Client is detected, but can't be connected to server. Error # " << WSAGetLastError() << endl;
		closesocket(servSock);
		closesocket(clientConn);
		WSACleanup();
		return 1;
	}

	char clientIP[22];
	inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);
	cout << "Connected client " << clientIP << endl;

	path client_path(string(USERS_FOLDER_NAME) + "/" + string(clientIP));
	createDirectory(client_path);
	cout << "Folder of a client is: " << client_path << endl;

	//Exchange text data between Server and Client. Disconnection if a client send "xxx"

	const int packageMessageSize = 1024;
	vector <char> servBuff(packageMessageSize);							// Creation of buffers for sending and receiving data

	short packet_size = 0;	
	
	

	while (true)
	{
		servBuff.resize(packageMessageSize);
		packet_size = recv(clientConn, servBuff.data(), servBuff.size(), 0);
		check_receiving(packet_size, clientConn, servSock);

		servBuff.resize(packet_size);
		string clear_message(servBuff.data(), servBuff.size());
		
		cout << "Client's message: " << servBuff.data() << endl;
		servBuff.clear();

		vector<string> tokens = split_string(clear_message);
	/*	for (int i = 0; i < tokens.size(); i++) {
			cout << tokens[i] << endl;
		}*/
		
		if (clear_message == "exit") {
			shutdown(clientConn, SD_BOTH);
			closesocket(servSock);
			closesocket(clientConn);
			WSACleanup();
			return 0;
		}
		else
		{
			if (tokens.size() < 2) {
				string message("argument #2 is empty.");
				send_message(message, clientConn, servSock);
				continue;
			}

			if (tokens[0] == "createDirectory")
			{
				path p = client_path / tokens[1];
				try {
					create_directories(p);

					string message("directory created.");
					send_message(message, clientConn, servSock);
				} catch (const std::exception& e) {
					string message("create directory error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "deleteDirectory")
			{
				path p = client_path / tokens[1];
				try {
					remove_all(p);

					string message("directory deleted.");
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("delete directory error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "createFile")
			{
				path p = client_path / tokens[1];
				try {
					createFile(p);

					string message("file created.");
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("file create error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "deleteFile")
			{
				path p = client_path / tokens[1];
				try {
					deleteFile(p);

					string message("file deleted.");
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("file delete error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "setText")
			{
				if (tokens.size() < 3) {
					string message("argument #3 not found.");
					send_message(message, clientConn, servSock);
					continue;
				}
				path p = client_path / tokens[1];
				try {
					string message = setToFile(p, tokens[2]);
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("setText error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "addText")
			{
				if (tokens.size() < 3) {
					string message("argument #3 not found.");
					send_message(message, clientConn, servSock);
					continue;
				}
				path p = client_path / tokens[1];
				try {
					string message = addToFile(p, tokens[2]);
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("addText error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "cd")
			{
				path p = client_path / tokens[1];
				try {
					string message = cd(p);
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("cd error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else if (tokens[0] == "tree")
			{
				path p = client_path / tokens[1];
				try {
					string message = "\n" + printTree(p, 1);
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
				catch (const std::exception& e) {
					string message("tree error: " + string(e.what()));
					cout << message << endl;
					send_message(message, clientConn, servSock);
				}
			}
			else {
				string message(clear_message + " not exist.");
				send_message(message, clientConn, servSock);
			}
		}
	}

	closesocket(servSock);
	closesocket(clientConn);
	WSACleanup();

	return 0;

}

vector<string> split_string(string input) {
	istringstream iss(input);
	vector<string> tokens;

	string token;
	while (iss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

void send_message(string message, SOCKET clientConn, SOCKET servSock) {
	short packet_size = send(clientConn, message.data(), message.size(), 0);
	check_sending(packet_size, clientConn, servSock);
}

void check_sending(short& packet_size, SOCKET clientConn, SOCKET servSock)
{
	if (packet_size == SOCKET_ERROR) {
		cout << "Can't send message to Client. Error # " << WSAGetLastError() << endl;
		closesocket(servSock);
		closesocket(clientConn);
		WSACleanup();
		exit(1);
	}
}

void check_receiving(short& packet_size, SOCKET clientConn, SOCKET servSock)
{
	if (packet_size == SOCKET_ERROR) {
		cout << "Can't receive message from Client. Error # " << WSAGetLastError() << endl;
		closesocket(servSock);
		closesocket(clientConn);
		WSACleanup();
		exit(1);
	}
}
