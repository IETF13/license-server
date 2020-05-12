#include<iostream>
#include<map>
using namespace std;

bool accountJudge(const string account,const string password)
{
    map<string,string> ACCOUNTS;
    ACCOUNTS["admin"]="password";
    ACCOUNTS["ADMIN"]="password";
    
    if(ACCOUNTS[account].compare(password)==0)
        return true;
    return false;
}

bool login()
{
    string account;
    string password;
    cout<<"please enter user account"<<endl;
    cin>>account;
    cout<<"please enter password"<<endl;
    cin>>password;
    if(accountJudge(account, password))
        return true;
    return false;
}


