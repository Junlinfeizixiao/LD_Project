#include <iostream>
#include <map>
#include <string>
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
#include <sys/epoll.h>
#include <signal.h>
#include <json/json.h>
#include "mysql.hpp"
typedef enum _MsgType  //消息类型
{
	EN_MSG_LOGIN = 1,
	EN_MSG_REGISTER, 
	EN_MSG_CHAT,
	EN_MSG_OFFLINE,
	EN_MSG_ACK
}EnMsgType;


void* ReadThread(void *arg)
{
	int clientfd = (long)arg; 
	UserData user;	
	user.UserInit();
	while(true)
	{
		char recvbuf[1024] = {0};
		int size = 0;

		Json::Reader reader;//XXX    //
		Json::Value root;	//XXX
		Json::Value response;//XXX
		size = recv(clientfd,recvbuf,1024,0);
		if(size <= 0)
		{
			cout<<"errno:"<<errno<<endl;
			cout<<"client connect fail!"<<endl;
			user.StateDel(clientfd);
			close(clientfd);
			return NULL;
		}	
		cout<<"recvbuf:"<<recvbuf<<endl;
		if(reader.parse(recvbuf,root)) //用来解析字符串 XXX   把recvbuf中的内容解析到root中
		{	
			int msgtype = root["msgtype"].asInt();     
			switch(msgtype)
			{
				case EN_MSG_LOGIN:
				{
					response["msgtype"] = EN_MSG_ACK;
					string name = root["name"].asString();
					string pwd = root["pwd"].asString();		
					if(user.UserLogin(name,pwd))
					{
						response["ackcode"] = "ok";
						user.StateAdd(name,clientfd);
						user.StateShow();
					}
					else
					{
						response["ackcode"] = "error";
					}
					cout<<"response:"<<response.toStyledString().c_str()<<endl;
					send(clientfd,response.toStyledString().c_str(),strlen(response.toStyledString().c_str())+1,0);
				
					break;
				}
				case EN_MSG_REGISTER:
				{	
					response["msgtype"] = EN_MSG_ACK;
					string name = root["name"].asString();
					string pwd = root["pwd"].asString();
					string phone = root["phone"].asString();
					if(!user.UserCheck(name))
					{
						user.UserAdd(name,pwd,phone);
						response["ackcode"] = "ok";
					}
					else
					{
						response["ackcode"] = "error";
					}
					cout<<"response:"<<response.toStyledString().c_str()    <<endl;
				    send(clientfd,response.toStyledString().c_str(),strlen(response.toStyledString().c_str())+1,0);	
					break;
				}
				case EN_MSG_CHAT:
				{	
					response["msgtype"] = EN_MSG_CHAT;
					string name = root["from"].asString();
					string msg = root["msg"].asString();
					int Linefd = 0;
					if(strstr(recvbuf,"\"to\" :") == 0)
					{
						cout<<"QQ群"<<endl;
						
						response["from"] = name;
						response["msg"] = msg;
						
						int len = user.StateGetLength();
						cout<<"len:"<<len<<endl;
						for(int i = 0; i < len; i++)
						{
							Linefd = user.StateGetFd(i);
							cout<<"Linefd:"<<Linefd<<endl;
							if(Linefd != user.StateCheck(name))
							{ 
								send(Linefd,response.toStyledString().c_str(),strlen    (response.toStyledString().c_str())+1,0);
							}
						}
					}
					else
					{
						string obj = root["to"].asString();				
						Linefd = user.StateCheck(obj);
						cout<<Linefd<<endl;
						response["from"] = name;
						if(Linefd != 0)
						{
							response["msg"] = msg;
						}
						else
						{
							string msg = "the object is not online";
							Linefd = clientfd;
							response["msg"] = msg; 
						}
						send(Linefd,response.toStyledString().c_str(),strlen(response.toStyledString().c_str())+1,0);
					}
					break;
				}
				case EN_MSG_OFFLINE:
				{
					response["msgtype"] = EN_MSG_OFFLINE;
					string msg = root["msg"].asString();
					if(strcmp(msg.c_str(),"/quit") == 0)
					{
						user.StateDel(clientfd);
						cout<<clientfd<<" is offline"<<endl;
					}
				   break;
				}
			}
		}
	}
}	
void ProcListenfd(evutil_socket_t fd,short ,void *arg)
{
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	
	long connfd = accept(fd,(struct sockaddr*)&caddr,&len);
	cout<<"one client connetc server!client info:"<<inet_ntoa(caddr.sin_addr)<<" "<<ntohs(caddr.sin_port)<<endl;
	
	pthread_t tid;
	pthread_create(&tid,NULL,ReadThread,(void*) connfd);
}
int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int res = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);

	res = listen(sockfd,20);
	assert(res != -1);
	
	struct event_base* base = event_init();
	struct event* listen_event = event_new(base,sockfd,EV_READ | EV_PERSIST,ProcListenfd,NULL);  //监听永久
	event_add(listen_event,NULL);

	cout<<"server start!"<<endl;
	event_base_dispatch(base);
	event_free(listen_event);
	event_base_free(base);	
	
	return 0;
}
