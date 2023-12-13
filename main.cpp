#include <iostream>
#include "accounts.h"

using std::cin;
using std::string;

int main() {
  AccountSys as("account");
  char s[35];
  char passwd[35];
  char username[35];
  int n;
  cin>>n;
  string op;
  int val;
  while(n--) {
    cin>>op;
    if(op=="su") {
      cin>>s>>passwd;
      ID id(s);
      as.login(id,passwd);
    } else if (op=="logout") {
      as.logout();
    } else if (op=="register") {
      cin>>s>>passwd>>username;
      ID id(s);
      Account ac(id,passwd,username,1);
      as.insert_account(ac);
    } else if (op=="useradd") {
      cin>>s>>passwd>>val>>username;
      ID id(s);
      Account ac(id,passwd,username,val);
      as.insert_account(ac);
    } else if (op=="delete") {
      cin>>s;
      ID id(s);
      as.remove_account(id);
    } else if (op == "print") {
      as.print();
    } else if (op == "quit") {
      break;
    } else if (op=="find") {
      cin>>s;
      ID id(s);
      auto tmp=as.user(id);
      cout<<tmp.user_id.id<<" "<<tmp.password<<" "<<tmp.user_name<<" "<<tmp.privilege<<'\n';
    }
  }
  return 0;
}