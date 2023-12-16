#include <iostream>
#include "loginstate.h"

using std::cin;
using std::string;

char s[35];
char passwd[35];
char username[35];
char isbn[25];
string op;
int val;

bool proceed_line();

int main() {
  //while (true) {
    try {
      Account ac((ID&)("123") , "2005", "lfk", 1);
      as.insert_account(ac);
      Account ac1((ID&)("124") , "2003", "lyc", 3);
      as.insert_account(ac1);
      login(ac1.user_id,ac1.password);
      select((ISBN&)"4536");
      modify_book((Author&)("lfk"));
      modify_book(23.5);
      modify_book((BookName&)"maijubaodian");
      modify_book((KeyWord&)"math|physics");
      as.print();
      bs.print();
      modify_book((ISBN&)"2546");
      select((ISBN&)"1234");
      modify_book((Author&)("lfk"));
      modify_book(23.5);
      modify_book((BookName&)"maicaibaodian");
      modify_book((KeyWord&)"math|chemistry");
      as.print();
      bs.print();
      bs.find_book((Author&)"lfk");
      bs.find_book((KeyWord&)"physics");

    } catch (ErrorException &ex) {
      std::cout << ex.getMessage();
    }
  //}
  return 0;
}

bool proceed_line() {
  bool flag=false;
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
    flag=true;
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
  return flag;
}