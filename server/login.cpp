#include "login.h"

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
    string input_account;
    string input_password;
    string type;

    cout<<"please enter user account"<<endl;
    cin>> input_account;

    cout<<"please enter password"<<endl;
    cin>> input_password;

    cout << "please enter license type" << endl;
    cin >> type;
    
    if(accountJudge(input_account, input_password))
        return true;
    return false;
}


