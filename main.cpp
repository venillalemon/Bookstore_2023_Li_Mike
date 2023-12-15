#include <iostream>
#include "loginstate.h"

using std::cin;
using std::string;

int main() {

  char s[35];
  char passwd[35];
  char username[35];
  char isbn[25];
  string op;
  int val;
  while (true) {
    try {
      cin >> op;
      if (op == "su") {
        cin >> s >> passwd;
        ID id(s);
        login(id, passwd);
      } else if (op == "logout") {
        logout();
      } else if (op == "register") {
        cin >> s >> passwd >> username;
        ID id(s);
        Account ac(id, passwd, username, 1);
        as.insert_account(ac);
      } else if (op == "useradd") {
        cin >> s >> passwd >> val >> username;
        ID id(s);
        Account ac(id, passwd, username, val);
        as.insert_account(ac);
      } else if (op == "delete") {
        cin >> s;
        ID id(s);
        as.remove_account(id);
      } else if (op == "print") {
        as.print();
        bs.print();
      } else if (op == "quit") {
        break;
      } else if (op == "find") {
        cin >> s;
        ID id(s);
        auto tmp = as.user(id);
        cout << tmp.user_id.id << " " << tmp.password << " " << tmp.user_name << " " << tmp.privilege << '\n';
      } else if (op == "select") {
        cin >> isbn;
        select(ISBN(isbn));
      } else if(op=="import") {
        int quantity;double totalcost;
        cin>>quantity>>totalcost;
        import(quantity,totalcost);
      } else if(op=="buy") {
        int quantity;
        cin>>isbn>>quantity;
        buy(ISBN(isbn),quantity);
      }
    } catch (ErrorException &ex) {
      std::cout << ex.getMessage();
    }
  }
  return 0;
}