#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <json/json.h>
typedef enum _MsgType
{
	EN_MSG_LOGIN = 1,
	EN_MSG_REGISTER,
	EN_MSG_CHAT,
	EN_MSG_OFFINE,
	EN_MSG_ACK
}EnMsgType;

char name[20];
char pwd[20];
char phone[20];

void* ReadThread(void* fd)//读线程函数
{

	int res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(res != 0)
	{
		cout<<"error"<<endl;
	}
	int clienfd = *(int*)fd;
	int size = 0;
	Json::Reader reader;
	Json::Value root;
	char recvbuf[1024]={0};
	while(true)
	{
		size = recv(clienfd,recvbuf,1024,0);
		if(size <= 0)
		{
			cout<<"recv error"<<endl;
			break;
		}
		if(reader.parse(recvbuf,root))
		{
			int msgtype = root["msgtype"].asInt();
			switch(msgtype)
			{
				case EN_MSG_CHAT:
				{
					string name = root["from"].asString();
					string msg = root["msg"].asString();
					cout<<"from"<<" "<<root["from"].asString()<<":"<<root["msg"].asString()<<endl;
				}
				break;
			}
		}
	}
}
bool do_login(int fd)
{
	cout<<"name:";
	cin.getline(name,20);

	cout<<"pwd:";
	cin.getline(pwd,20);
	Json::Value root;
	root["msgtype"] = EN_MSG_LOGIN;
	root["name"] = name;
	root["pwd"] = pwd;

	int size = send(fd,root.toStyledString().c_str(),strlen(root.toStyledString().c_str())+1,0);
	if(size < 0)
	{ 
		cout<<"send error!"<<endl;
		return false;
	}

	char recvbuf[1024] = {0};
	size = recv(fd,recvbuf,1024,0);
	if(size < 0)
	{
		cout<<"recv server login ack fail"<<endl;
		return false;
	}
	Json::Reader reader;
	if(reader.parse(recvbuf,root))
	{
		int msgtype = root["msgtype"].asInt();
		if(msgtype != EN_MSG_ACK)
		{
			cout<<"recv server loginack invaild!"<<endl;
			return false;
		}
		string ackcode = root["ackcode"].asString();
		if(ackcode == "ok")
		{
			return true;
		}
		return false;
	}
	return false;
}
bool do_register(int fd)
{
	cout<<"name:";
	cin.getline(name ,20);

	cout<<"pwd:";
	cin.getline(pwd,20);

	cout<<"phone:";
	cin.getline(phone,20);

    Json::Value root;
    root["msgtype"] = EN_MSG_REGISTER;
    root["name"] = name;
    root["pwd"] = pwd;
	root["phone"] = phone;
	int size = send(fd,root.toStyledString().c_str(),strlen(root.toStyledString().c_str())+1,0);
	if(size < 0)
	{
		cout<<"size error"<<endl;
		return false;
	}
	
	char recvbuf[1024] = {0};
	size = recv(fd,recvbuf,1024,0);
	if(size < 0)
	{
		cout<<"recv server login ack fail"<<endl;
		return false;
	}
	Json::Reader reader;
	if(reader.parse(recvbuf,root))
	{
		int msgtype = root["msgtype"].asInt();
		if(msgtype != EN_MSG_ACK)
		{
			cout<<"recv server register ack invaild!"<<endl;
			return false;
		}
		string ackcode = root["ackcode"].asString();
		if(ackcode == "ok")
		{
			return true;
		}
		return false;
	}
	return false;
}
bool do_quit(int fd)
{
	close(fd);
	return true;
}

void menu()
{
	cout<<"1,login"<<endl;
	cout<<"2,register"<<endl;
	cout<<"3,exit"<<endl;
	cout<<"input:";
}
void run(int fd)
{
	bool bloginsuccess = false;
	while(!bloginsuccess)
	{
		menu();
		char a;
		cin>>a;
		cin.get();
		switch(a)
		{
			case '1':
			{
				if(do_login(fd))
				{
					bloginsuccess = true;
				}
				else
				{
					cout<<"login fail name or pwd is wrong"<<endl;
				}
			}
			break;
			case '2':
			{
				if(do_register(fd))
				{
					cout<<"success!"<<endl;	
				}
				else
				{
					cout<<"fail!"<<endl;
				}	
			}
			continue;
			case '3':
			{
				if(do_quit(fd))
				{
					cout<<"now is exiting!"<<endl;
					exit (0);
				}
				else
				{
					cout<<"quit error"<<endl;
				}
			}
			continue;
			default:
			{
				cout<<"invalid input!"<<endl;
			}
			continue;
		}
	}
	cout<<"welcome to Chat-System!"<<endl;
}
int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int ret = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(ret < 0)
	{
		printf("connect fail\n");
		exit(-1);
	}
Runn:
	run(sockfd);
	pthread_t tid;//读线程
    pthread_create(&tid,NULL,ReadThread,&sockfd);
	int size = 0;
	while(true)	
	{
		char chatbuf[1024] = {0};
		cin.getline(chatbuf,1024);
		if(strcmp(chatbuf,"/quit") == 0)
		{
			Json::Value quit;
			quit["msgtype"] = EN_MSG_OFFINE;
			quit["msg"] = chatbuf;
			size = send(sockfd,quit.toStyledString().c_str(),strlen(quit.toStyledString().c_str())+1,0);
			if(size == -1)
			{
				cout<<"quit error"<<endl;
				continue;					
			}
			pthread_cancel(tid);    //XXX
			goto Runn;
		}
		string parsestr = chatbuf;
		int offset = parsestr.find(':');
		
		Json::Value root;
		root["msgtype"] = EN_MSG_CHAT;
		root["from"] = name;
		if(offset > 0)
		{
			root["to"] = parsestr.substr(0,offset);
		}
		root["msg"] = parsestr.substr(offset+1,parsestr.length()-offset-1);
		size = send(sockfd,root.toStyledString().c_str(),strlen(root.toStyledString().c_str())+1,0);
		if(size == -1)
		{
			cout<<"error"<<errno<<endl;
		}
	}
	close(sockfd);
}
