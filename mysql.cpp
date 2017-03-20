#include "mysql.hpp"
void UserData::UserInit()
{
	pcon = mysql_init((MYSQL*)0);
	mysql_real_connect(pcon,"127.0.0.1","root","111111",NULL,3306,NULL,0);
	mysql_select_db(pcon,"Chat");	
}
void UserData::UserAdd(string name,string pwd,string phone)
{
	sprintf(sql,"insert into user values('%s','%s','%s')",name.c_str(),pwd.c_str(),phone.c_str());
	
	int ret = mysql_real_query(pcon,sql,strlen(sql));
	if(!ret)
	{
		cout<<"add data success!"<<endl;
	}
	else
	{
		cout<<"error"<<mysql_error(pcon)<<endl;
	}
}

void UserData::UserDel(string name)
{
	sprintf(sql,"delete from user where name = '%s'",name.c_str());
	
	int ret = mysql_real_query(pcon,sql,strlen(sql));
	cout<<"delete success!"<<endl;
}

void UserData::UserUpd(string name,string pwd)
{	
	sprintf(sql,"update user set passwd = '%s' where name = '%s'",name.c_str(),pwd.c_str());
	
	int ret = mysql_real_query(pcon,sql,strlen(sql));
	cout<<"update success!"<<endl;

}

void UserData::UserSel()
{
	sprintf(sql,"select * from user");
    mysql_real_query(pcon,sql,strlen(sql));
	pres = mysql_store_result(pcon);
	while(row = mysql_fetch_row(pres))
	{
		for(int i = 0;i < mysql_num_fields(pres);++i)
		{
			cout<<row[i]<<" ";
		}
		cout<<endl;
	}
}
bool UserData::UserCheck(string name)
{
	cout<<"UserCheck"<<endl;
	sprintf(sql,"select name from user where name = '%s'",name.c_str());
	mysql_real_query(pcon,sql,strlen(sql));
	pres = mysql_store_result(pcon);
	
	while(row = mysql_fetch_row(pres))
	{
		for(int i = 0; i < mysql_num_fields(pres); i++)
		{
			if(strcmp(name.c_str(),row[i]) == 0)
			{
				return true;
			}
		}
	}	
	return false;
}
bool UserData::UserLogin(string name ,string pwd)
{

	cout<<"UserCheck"<<endl;
	sprintf(sql,"select passwd from user where name = '%s'",name.c_str());
	mysql_real_query(pcon,sql,strlen(sql));
	pres = mysql_store_result(pcon);
	
	while(row = mysql_fetch_row(pres))
	{
		for(int i = 0; i < mysql_num_fields(pres); i++)
		{
			if(strcmp(pwd.c_str(),row[i]) == 0)
			{
				return true;
			}
		}
	}	
	return false;
}
bool UserData::StateAdd(string name,int fd)
{
	sprintf(sql,"insert into state values('%s',%d)",name.c_str(),fd);
	
	int ret = mysql_real_query(pcon,sql,strlen(sql));
	if(!ret)
	{
		cout<<"add state success!"<<endl;
	}
	else
	{
		cout<<"error"<<mysql_error(pcon)<<endl;
		return false;
	}
	return true;
}
bool UserData::StateDel(int fd)
{
	int bret = false;
	do
	{
		sprintf(sql,"delete from state where socket = %d",fd);
		mysql_real_query(pcon,sql,strlen(sql));
		bret = true;
	}while(0);

	return bret;
}
void UserData::StateShow()
{
	sprintf(sql,"select * from state");
    mysql_real_query(pcon,sql,strlen(sql));
	pres = mysql_store_result(pcon);
	while(row = mysql_fetch_row(pres))
	{
		for(int i = 0;i < mysql_num_fields(pres);++i)
		{
			cout<<row[i]<<" ";
		}
		cout<<endl;
	}
}
int UserData::StateCheck(string name)
{
	sprintf(sql,"select * from state where name = '%s'",name.c_str());
	mysql_real_query(pcon,sql,strlen(sql));
	pres = mysql_store_result(pcon);
	
	while(row = mysql_fetch_row(pres))
	{
		for(int i = 0; i < mysql_num_fields(pres); i++)
		{
			if(strcmp(name.c_str(),row[i]) == 0)
			{
				return atoi(row[i+1]);
			}
		}
	}	
	return 0;
} 
int UserData::StateGetLength()
{
	 sprintf(sql,"select * from state");
	 mysql_real_query(pcon,sql,strlen(sql));
	 pres = mysql_store_result(pcon);
	 int len =  mysql_num_rows(pres); //得到记录的行数
	 return len;
}	
bool UserData::StateGetFd()
{
	bool bret = false;
	do
	{
		sprintf(sql,"select * from state");
		mysql_real_query(pcon,sql,strlen(sql));
		pres = mysql_store_result(pcon);
		int i = 0;
		while(row = mysql_fetch_row(pres))
		{
			arrfd[i++] = atoi(row[1]);		
			cout<<"arrfd[i]"<<arrfd[i-1]<<endl;;
		}
		bret = true;
	}while(0);
	return bret;
} 
