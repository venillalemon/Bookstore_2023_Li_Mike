#ifndef CODE_PARSER_H
#define CODE_PARSER_H
#pragma once

#include <regex>
#include "loginstate.h"

using std::regex;
using std::regex_match;
using std::smatch;

void parse_Command(const string &input) {

  if (input == "print") {
    print();
    return;
  }

  regex su_regex(R"(su (\w+)( (\w+))?)");
  smatch match;
  if (regex_match(input, match, su_regex)) {
    string userID = match.str(1);
    std::string password = match.str(3);
    ID user_id(userID.c_str());
    //cout << user_id << " " << password << '\n';
    login(user_id, password.c_str());
    return;
  }

  if (input == "logout") {
    logout();
    return;
  }

  regex register_regex(R"(register (\w+) (\w+) ([^\s]+))");
  smatch register_match;
  if (regex_match(input, register_match, register_regex)) {
    string userID = register_match.str(1);
    string password = register_match.str(2);
    string username = register_match.str(3);
    ID user_id(userID.c_str());
    //cout << user_id << " " << password << " " << username << '\n';
    reg(user_id, password.c_str(), username.c_str());
    return;
  }

  regex useradd_regex(R"(useradd (\w+) (\w+) (\d) ([^\s]+))");
  smatch useradd_match;
  if (regex_match(input, useradd_match, useradd_regex)) {
    string userID = useradd_match.str(1);
    string password = useradd_match.str(2);
    string privilege = useradd_match.str(3);
    string username = useradd_match.str(4);
    ID user_id(userID.c_str());
    useradd(user_id, password.c_str(), username.c_str(), std::stoi(privilege));
    //cout << user_id << " " << password << " " << privilege << " " << username << '\n';
    return;
  }

  regex passwd_regex(R"(passwd (\w+)( (\w+))? (\w+))");
  smatch passwd_match;
  if (regex_match(input, passwd_match, passwd_regex)) {
    string userID = passwd_match.str(1);
    string old_password = passwd_match.str(3);
    string new_password = passwd_match.str(4);
    ID user_id(userID.c_str());
    resetpasswd(user_id, new_password.c_str(),old_password.c_str());
    //cout << user_id << " " << old_password << " " << new_password << '\n';
    return;
  }

  regex delete_regex(R"(delete (\w+))");
  smatch delete_match;
  if (regex_match(input, delete_match, delete_regex)) {
    string userID = delete_match.str(1);
    ID user_id(userID.c_str());
    delete_account(user_id);
    //cout << user_id << '\n';
    return;
  }

  regex show_regex(
          R"(^show( (-ISBN=[\x20-\x7E]+|-name="[^"\s]+"|-author="[^"\s]+"|-keyword="[^"\s]+"))?)");
  smatch show_match;
  if (regex_match(input, show_match, show_regex)) {
    string command = show_match.str(2);
    if (command.empty()) {
      //show();
      cout << "show\n";
    } else {
      if (command[1] == 'I') {
        string ISBN = command.substr(6);
        //show_ISBN(ISBN);
        cout << "show_ISBN " << ISBN << '\n';
      } else if (command[1] == 'n') {
        string name = command.substr(7, command.size() - 8);
        //show_name(name);
        cout << "show_name " << name << '\n';
      } else if (command[1] == 'a') {
        string author = command.substr(9, command.size() - 10);
        //show_author(author);
        cout << "show_author " << author << '\n';
      } else if (command[1] == 'k') {
        string keyword = command.substr(10, command.size() - 11);
        //show_keyword(keyword);
        cout << "show_keyword " << keyword << '\n';
      }
    }
    return;
  }

  regex buy_regex(R"(buy (\w+) (\d+))");
  smatch buy_match;
  if (regex_match(input, buy_match, buy_regex)) {
    string ISBN = buy_match.str(1);
    string quantity = buy_match.str(2);
    //buy((ISBN &) ISBN, std::stoi(quantity));
    cout << ISBN << " " << quantity << '\n';
    return;
  }

  regex select_regex(R"(select (\w+))");
  smatch select_match;
  if (regex_match(input, select_match, select_regex)) {
    string ISBN = select_match.str(1);
    //select((ISBN &) ISBN);
    cout << ISBN << '\n';
    return;
  }

  regex import_regex(R"(import (\d+) (\d+(.\d+)?))");
  smatch import_match;
  if (regex_match(input, import_match, import_regex)) {
    string quantity = import_match.str(1);
    string cost = import_match.str(2);
    //import(std::stoi(quantity), std::stod(cost));
    cout << quantity << " " << cost << '\n';
    return;
  }

  regex modify_regex(
          R"(modify( (-ISBN=[^\s]+|-name="[^"\s]+"|-author="[^"\s]+"|-keyword="[^"\s]+"|-price=\d+(.\d+)?))+)");
  smatch modify_match;
  if (regex_match(input, modify_match, modify_regex)) {
    string tok = input.substr(7);
    //cout << tok << "\n";
    smatch tok_match;
    regex pattern(R"((-(ISBN|name|author|keyword|price))=([^\s]+))");
    string::const_iterator citer = tok.cbegin();
    while (regex_search(citer, tok.cend(), tok_match, pattern)) {
      citer = tok_match[0].second;
      string command = tok_match.str(2);
      string value = tok_match.str(3);
      cout << command << " " << value << "\n";
      if (command == "ISBN") {
        //modify_book((ISBN&)value);
      } else if (command == "name") {
        value = value.substr(1, value.size() - 2);
        cout << value;
        //modify_book((BookName&)value);
      } else if (command == "author") {
        value = value.substr(1, value.size() - 2);
        cout << value;
        //modify_book((Author&)value);
      } else if (command == "keyword") {
        value = value.substr(1, value.size() - 2);
        cout << value;
        //modify_book((KeyWord&)value);
      } else if (command == "price") {
        //modify_book(std::stod(value));
      }
    }
    return;
  }

  regex show_finance_regex(R"(show finance( (\d+))?)");
  smatch show_finance_match;
  if (regex_match(input, show_finance_match, show_finance_regex)) {
    string time = show_finance_match.str(2);
    //show_finance(std::stoi(time));
    cout << time << '\n';
    return;
  }

  if (input == "log") {
    //log();
    cout << "log\n";
    return;
  }

  if (input == "report finance") {
    fs.print();
    cout << "finance\n";
    return;
  }

  if (input == "report employee") {
    //as.print();
    cout << "employee\n";
    return;
  }

  error("Invalid\n");

}

#endif //CODE_PARSER_H
