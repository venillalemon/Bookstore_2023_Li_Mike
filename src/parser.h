#ifndef CODE_PARSER_H
#define CODE_PARSER_H
#pragma once

#include <regex>
#include <sstream>
#include "loginstate.h"

using std::regex;
using std::regex_match;
using std::smatch;
using std::stringstream;

typedef m_string<100> Command;

void parse_Command(const string &input) {

  regex inc_regex(R"([\x00-\x19])");
  regex ID_regex(R"(\w+)");//for userid, password
  regex name_regex(R"(\S+)");//for username, book name, author, keyword
  regex show_regex(R"(-ISBN=[\x21-\x7E]+|-name="[^"\s]+"|-author="[^"\s]+"|-keyword="[^"\s]+")");

  smatch match;
  if (regex_search(input, match, inc_regex)) {
    error("Invalid: invisible char\n");
  }

  stringstream ss(input);

  if (input == "print") {
    print();
    return;
  }

  string op;
  ss >> op;
  if (ss.fail()) error("not enough token\n");
  if (op == "logout") {
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    logout();
    return;
  } else if (op == "log") {
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    //log();
    return;
  } else if (op == "report") {
    ss >> op;
    if (ss.fail()) error("not enough token\n");
    if (op == "finance") {
      ss >> op;

      if (!ss.fail()) error("exceeded token\n");
      if (curPrivilege() < 7) error("report finance: low privilege\n");
      fs.show();
      return;
    } else if (op == "employee") {
      ss >> op;
      if (!ss.fail()) error("exceeded token\n");
      //as.print();
      //cout << "employee\n";
      return;
    }
  } else if (op == "delete") {
    ss >> op;
    if (ss.fail()) error("not enough token\n");
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid ID\n");
      return;
    }
    ID id(op.c_str());
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    delete_account(id);
    return;
  } else if (op == "select") {
    ss >> op;
    if (ss.fail()) error("not enough token\n");
    if (!regex_match(op, match, name_regex)) {
      error("Invalid ID\n");
      return;
    }
    ISBN isbn(op.c_str());
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    select(ISBN(isbn));
    //cout << isbn << '\n';
    return;
  } else if (op == "buy") {
    ss >> op;
    if (ss.fail()) error("not enough token\n");
    if (!regex_match(op, match, name_regex)) {
      error("Invalid ISBN\n");
      return;
    }
    ISBN isbn(op.c_str());
    string amount;
    ss >> amount;
    if (ss.fail()) error("not enough token\n");
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    if (std::stod(amount) != std::stoi(amount)) error("buy: invalid quantity\n");
    // can throw a std::invalid_argument if amount="4.5"
    buy(isbn, std::stoi(amount));
    return;
  } else if (op == "import") {
    string amount, cost;
    ss >> amount >> cost;
    if (ss.fail()) error("not enough token\n");
    ss >> op;
    if (!ss.fail()) error("exceeded token\n");
    int a = std::stoi(amount);
    double c = std::stod(cost);
    if (std::stod(amount) != a) error("import: invalid quantity\n");
    // can throw a std::invalid_argument if amount="4.5"
    import(a, c);
    return;
  } else if (op == "su") {
    ss >> op;
    if (ss.fail()) error("not enough token\n");
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid ID\n");
      return;
    }
    ID id(op.c_str());
    ss >> op;
    string passwd;
    if (!ss.fail()) {
      passwd = op;
      if (!regex_match(op, match, ID_regex)) {
        error("Invalid password\n");
        return;
      }
      ss >> op;
      if (!ss.fail()) error("exceeded token\n");
      login(id, passwd.c_str());
      return;
    } else {
      passwd = "";
      login(id);
      return;
    }
  } else if (op == "register") {
    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid ID\n");
      return;
    }
    ID id(op.c_str());

    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid password\n");
      return;
    }
    string password = op;

    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, name_regex)) {
      error("Invalid name\n");
      return;
    }
    string name = op;
    ss >> op;
    if (!ss.fail()) {
      error("exceeded token\n");
      return;
    }
    reg(id, password.c_str(), name.c_str());
    return;
  } else if (op == "useradd") {
    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid ID\n");
      return;
    }
    ID id(op.c_str());

    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, ID_regex)) {
      error("Invalid password\n");
      return;
    }
    string password = op;

    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    int pri = std::stoi(op);
    if (!(pri == 3 || pri == 1 || pri == 7)) {
      error("useradd: wrong privilege\n");
      return;
    }

    ss >> op;
    if (ss.fail()) {
      error("not enough token\n");
      return;
    }
    if (!regex_match(op, match, name_regex)) {
      error("Invalid username\n");
      return;
    }
    string name = op;

    ss >> op;
    if (!ss.fail()) {
      error("exceeded token\n");
      return;
    }
    useradd(id, password.c_str(), name.c_str(), pri);
    return;
  } else if (op == "passwd") {
    ss >> op;
    vector<string> arr;
    while (!ss.fail()) {
      if (!regex_match(op, match, ID_regex)) {
        error("Invalid ID/password\n");
        return;
      }
      arr.push_back(op);
      ss >> op;
    }
    if (arr.size() == 2) {
      resetpasswd(ID(arr[0].c_str()), arr[1].c_str());
    } else if (arr.size() == 3) {
      resetpasswd(ID(arr[0].c_str()), arr[2].c_str(), arr[1].c_str());
    } else {
      error("Invalid: wrong number of token\n");
    }
    return;
  } else if (op == "show") {
    ss >> op;
    if (ss.fail()) {
      bs.show();
      return;
    } else {
      if (op == "finance") {
        ss >> op;
        if (ss.fail()) {
          show_finance();
          return;
        } else {
          ss >> op;
          if (!ss.fail()) error("exceeded token\n");
          if (std::stoi(op) != std::stod(op))error("show finance: invalid number\n");
          show_finance(std::stoi(op));
          return;
        }
      } else {
        string command = op;
        if (!regex_match(command, match, show_regex)) {
          error("Invalid: wrong command\n");
          return;
        }
        if (command[1] == 'I') {
          if (command.substr(6).size() > 20)error("modify: ISBN too long\n");
          find_book(ISBN(command.substr(6).c_str()));
        } else if (command[1] == 'n') {
          if (command.substr(7, command.size() - 8).size() > 60)error("modify: name too long\n");
          find_book(BookName(command.substr(7, command.size() - 8).c_str()));
        } else if (command[1] == 'a') {
          if (command.substr(9, command.size() - 10).size() > 60)error("modify: name too long\n");
          find_book(Author(command.substr(9, command.size() - 10).c_str()));
        } else if (command[1] == 'k') {
          if (command.substr(10, command.size() - 11).size() > 60)error("modify: name too long\n");
          find_book(KeyWord(command.substr(10, command.size() - 11).c_str()));
        }
        return;
      }
    }
  } else if (op == "modify") {
    ISBN isbn{};
    BookName book_name{};
    Author auth{};
    KeyWord key_word{};
    double price = -1;
    regex pattern(R"(-(ISBN|name|author|keyword|price)=([^\s]+))");
    ss >> op;
    while (!ss.fail()) {
      if (!regex_match(op, match, pattern)) {
        error("Invalid: wrong command\n");
        return;
      }
      string command = match.str(1);
      string value = match.str(2);
      //cout << command << " " << value << "\n";
      if (command == "ISBN") {
        if (isbn != ISBN{}) error("modify: repeated ISBN\n");
        if (value.size() > 20)error("modify: ISBN too long\n");
        isbn = ISBN(value.c_str());
      } else if (command == "name") {
        if (book_name != BookName{}) error("modify: repeated name\n");
        if (value.size() <= 2) error("modify: wrong command\n");
        if (value[0] != '"' || value[value.size() - 1] != '"')error("modify: wrong command\n");
        value = value.substr(1, value.size() - 2);
        if (value.size() > 60)error("modify: name too long\n");
        if (value.find('"') != string::npos)error("modify: wrong command\n");
        book_name = BookName(value.c_str());
      } else if (command == "author") {
        if (auth != Author{}) error("modify: repeated author\n");
        if (value.size() <= 2) error("modify: wrong command\n");
        if (value[0] != '"' || value[value.size() - 1] != '"')error("modify: wrong command\n");
        value = value.substr(1, value.size() - 2);
        if (value.size() > 60)error("modify: name too long\n");
        if (value.find('"') != string::npos)error("modify: wrong command\n");
        auth = Author(value.c_str());
      } else if (command == "keyword") {
        if (key_word != KeyWord{}) error("modify: repeated keyword\n");
        if (value.size() <= 2)error("modify: wrong command\n");
        if (value[0] != '"' || value[value.size() - 1] != '"')error("modify: wrong command\n");
        value = value.substr(1, value.size() - 2);
        if (value.size() > 60)error("modify: name too long\n");
        if (value.find('"') != string::npos)error("modify: wrong command\n");
        key_word = KeyWord(value.c_str());
      } else if (command == "price") {
        if (std::stod(value) < 0) error("modify: wrong price\n");
        if (price != -1) error("modify: repeated price\n");
        if (value == ".") error("modify: wrong price\n");
        price = std::stod(value);
      }
      ss >> op;
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

  error("Invalid: no match\n");

}

#endif //CODE_PARSER_H
