#include <iostream>
#include <vector>
#include "parser.h"

using std::cin;
using std::string;

char s[35];
char passwd[35];
char username[35];
char isbn[25];
string op;
int val;

bool process_line(const string &);

AccountSys as("account");
BookSys bs("book");
FinanceSys fs("finance");
vector<pair<ID, ISBN>> login_list;

int main() {

  while (true) {
    try {
      std::string input;
      getline(cin, input);
      if (input.empty() || input == "quit" || input == "exit") break;
      parse_Command(input);

      /*Account ac((ID &) ("123"), "2005", "lfk", 1);
      as.insert_account(ac);
      Account ac1((ID &) ("124"), "2003", "lyc", 3);
      as.insert_account(ac1);
      login(ac1.user_id, ac1.password);
      select((ISBN &) "4536");
      modify_book((Author &) ("lfk"));
      modify_book(23.5);
      modify_book((BookName &) "maijubaodian");
      modify_book((KeyWord &) "math|physics");
      print();
      modify_book((ISBN &) "2546");
      select((ISBN &) "1234");
      modify_book((Author &) ("lfk"));
      modify_book(23.5);
      modify_book((BookName &) "maicaibaodian");
      modify_book((KeyWord &) "math|chemistry");
      resetpasswd((ID &) "123", "20050114", "2005");
      print();
      bs.find_book((Author &) "lfk");
      bs.find_book((KeyWord &) "physics");

      login((ID &) "124", "2003");
      select((ISBN &) "1234");
      import(10, 105);
      buy((ISBN &) "1234", 5);*/
      //print();



    } catch (ErrorException &ex) {
      std::cout << ex.getMessage();
    }
  }
  as.write_aux();
  bs.write_aux();
  fs.write_count();
  fs.write_v();
  return 0;
}
