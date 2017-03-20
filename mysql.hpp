#ifndef _MYSQL_H_
#define _MYSQL_H_

#include <iostream>
using namespace std;
#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*
MYSQL *pcon;
MYSQL_RES *pres;
MYSQL_ROW row; 
*/
/*
pcon = mysql_init((MYSQL*)0);
mysql_real_connect(pcon,"127.0.0.1","root","111111",NULL,3306,NULL,0);
mysql_select_db(pcon,"Chat");
*/

class UserData 
{
public:
	void UserInit();                //初始化
	void UserAdd(string name,string pwd,string phone);  //添加信息
	void UserDel(string name);                 //删除信息
	void UserUpd(string name,string pwd);      //修改信息
	void UserSel();                 //显示信息
	bool UserCheck(string name);
	bool UserLogin(string name,string pwd);
	
	bool StateAdd(string name,int fd);
	bool StateDel(int fd);
	void StateShow();
	int StateCheck(string name);
	int StateGetLength();
	bool StateGetFd();
private:

    char sql[1024];
	char name[20];
	char pwd[20];
	char phone[20];		
	MYSQL*  pcon;
	MYSQL_RES* pres;
    MYSQL_ROW row; 
public:
	int arrfd[1024];
};

#endif
