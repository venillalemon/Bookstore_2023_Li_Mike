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

  regex su_regex(R"(su (\w+)( (\w+))?( )?)");
  smatch match;
  if (regex_match(input, match, su_regex)) {
    //cout << user_id << " " << password << '\n';
    login(ID(match.str(1).c_str()), match.str(3).c_str());
    return;
  }

  if (input == "logout") {
    logout();
    return;
  }

  regex register_regex(R"(register (\w+) (\w+) ([^\s]+)( )?)");
  smatch register_match;
  if (regex_match(input, register_match, register_regex)) {
    //cout << user_id << " " << password << " " << username << '\n';
    reg(ID(register_match.str(1).c_str()), register_match.str(2).c_str(), register_match.str(3).c_str());
    //login(user_id,password.c_str());
    return;
  }

  regex useradd_regex(R"(useradd (\w+) (\w+) (\d) ([^\s]+))");
  smatch useradd_match;
  if (regex_match(input, useradd_match, useradd_regex)) {
    useradd(ID(useradd_match.str(1).c_str()), useradd_match.str(2).c_str(), useradd_match.str(4).c_str(), std::stoi(useradd_match.str(3)));
    //login(user_id,password.c_str());
    //cout << user_id << " " << password << " " << privilege << " " << username << '\n';
    return;
  }

  regex passwd_regex(R"(passwd (\w+)( (\w+))? (\w+))");
  smatch passwd_match;
  if (regex_match(input, passwd_match, passwd_regex)) {
    resetpasswd(ID(passwd_match.str(1).c_str()), passwd_match.str(4).c_str(), passwd_match.str(3).c_str());
    //cout << user_id << " " << old_password << " " << new_password << '\n';
    return;
  }

  regex delete_regex(R"(delete (\w+))");
  smatch delete_match;
  if (regex_match(input, delete_match, delete_regex)) {
    //cout << user_id << '\n';
    delete_account(ID(delete_match.str(1).c_str()));
    return;
  }

  regex show_regex(
          R"(^show( (-ISBN=[\x20-\x7E]+|-name="[^"\s]+"|-author="[^"\s]+"|-keyword="[^"\s]+"))?)");
  smatch show_match;
  if (regex_match(input, show_match, show_regex)) {
    string command = show_match.str(2);
    if (command.empty()) {
      bs.show();
      //cout << "show\n";
    } else {
      if (command[1] == 'I') {
        find_book(ISBN(command.substr(6).c_str()));
        //cout << "show_ISBN " << isbn << '\n';
      } else if (command[1] == 'n') {
        find_book(BookName(command.substr(7, command.size() - 8).c_str()));
        //cout << "show_name " << book_name << '\n';
      } else if (command[1] == 'a') {
        find_book(Author(command.substr(9, command.size() - 10).c_str()));
        //cout << "show_author " << auth << '\n';
      } else if (command[1] == 'k') {
        find_book(KeyWord(command.substr(10, command.size() - 11).c_str()));
        //cout << "show_keyword " << key_word << '\n';
      }
    }
    return;
  }

  regex buy_regex(R"(buy (\S+) (\d+)( )?)");
  smatch buy_match;
  if (regex_match(input, buy_match, buy_regex)) {
    buy(ISBN(buy_match.str(1).c_str()), std::stoi(buy_match.str(2)));
    //cout << isbn << " " << quantity << '\n';
    return;
  }

  regex select_regex(R"(select (\S+))");
  smatch select_match;
  if (regex_match(input, select_match, select_regex)) {
    select(ISBN(select_match.str(1).c_str()));
    //cout << isbn << '\n';
    return;
  }

  regex import_regex(R"(import (\d+) (\d+(.\d+)?))");
  smatch import_match;
  if (regex_match(input, import_match, import_regex)) {
    import(std::stoi(import_match.str(1)), std::stod(import_match.str(2)));
    //cout << quantity << " " << cost << '\n';
    return;
  }

  regex modify_regex(
          R"(modify( (-ISBN=[^\s]+|-name="[^"\s]+"|-author="[^"\s]+"|-keyword="[^"\s]+"|-price=\d+(.\d+)?))+)");
  smatch modify_match;
  if (regex_match(input, modify_match, modify_regex)) {
    ISBN isbn{};
    BookName book_name{};
    Author auth{};
    KeyWord key_word{};
    double price = -1;
    string tok = input.substr(7);
    //cout << tok << "\n";
    smatch tok_match;
    regex pattern(R"((-(ISBN|name|author|keyword|price))=([^\s]+))");
    string::const_iterator citer = tok.cbegin();
    while (regex_search(citer, tok.cend(), tok_match, pattern)) {
      citer = tok_match[0].second;
      string command = tok_match.str(2);
      string value = tok_match.str(3);
      //cout << command << " " << value << "\n";
      if (command == "ISBN") {
        isbn = ISBN(value.c_str());
      } else if (command == "name") {
        value = value.substr(1, value.size() - 2);
        book_name = BookName(value.c_str());
      } else if (command == "author") {
        value = value.substr(1, value.size() - 2);
        auth = Author(value.c_str());
      } else if (command == "keyword") {
        value = value.substr(1, value.size() - 2);
        key_word = KeyWord(value.c_str());
      } else if (command == "price") {
        price = std::stod(value);
      }
    }
    if (key_word != KeyWord{}) {
      vector<KeyWord> kl;
      char key_list[70];
      strcpy(key_list, key_word.id);
      char t[70];
      char *token = strtok(key_list, "|");
      while (token != nullptr) {
        strcpy(t, token);
        for (auto i: kl) {
          if (i == KeyWord(t)) error("modify: repeated keyword\n");
        }
        kl.emplace_back(t);
        token = strtok(nullptr, "|");
      }
      modify_book(key_word);
    }
    if (price != -1) modify_book(price);
    if (isbn != ISBN{}) modify_book(isbn);
    if (book_name != BookName{}) modify_book(book_name);
    if (auth != Author{}) modify_book(auth);

    return;
  }

  regex show_finance_regex(R"(show finance( (\d+))?)");
  smatch show_finance_match;
  if (regex_match(input, show_finance_match, show_finance_regex)) {
    if (show_finance_match.str(2).empty()) show_finance();
    else show_finance(std::stoi(show_finance_match.str(2)));
    return;
  }

  if (input == "log") {
    //log();
    //cout << "log\n";
    return;
  }

  if (input == "report finance") {
    fs.print();
    //cout << "finance\n";
    return;
  }

  if (input == "report employee") {
    //as.print();
    //cout << "employee\n";
    return;
  }

  error("Invalid: no match\n");

}

#endif //CODE_PARSER_H
