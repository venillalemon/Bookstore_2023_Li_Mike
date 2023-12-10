#ifndef BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H
#define BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstring>
#include <stack>
#include "books.h"

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::stack;
using std::unordered_map;
using std::map;
using std::pair;

class AccountSys;

struct ID {
  char user_id[35];

  bool operator==(const ID &rhs) const {
    return (strcmp(user_id, rhs.user_id) == 0);
  }

  bool operator<(const ID &rhs) const {
    return (strcmp(user_id, rhs.user_id) < 0);
  }
};

class Account {
private:
  char user_id[35];
  char password[35];
  char user_name[35];
  int privilege;

  friend class AccountSys;
};


class AccountSys {
public:
  string name;
  fstream file_main, file_aux;
  //main: Account; aux: map
  map<ID, int> data;
  stack<pair<ID, books>> login_list;
  int data_length = 0;
  //aux: first--data_length
  //main: account data



  AccountSys(const string &FN = "") {
    if (FN != "") name = FN;
    file_main.open(name, std::ios::in);
    file_aux.open(name + "_aux", std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_aux.close();
      file_aux.open(name + "_aux", fstream::out | fstream::binary);
      file_main.open(name, fstream::out | fstream::binary);
      file_aux.write(reinterpret_cast<char *>(&data_length), sizeof(int));
      file_main.close();
      file_aux.close();
    } else {
      file_main.close();
      file_aux.close();
      read_map();
    }
  }

  ~AccountSys() {
    write_map();
  }

  void read_map() {
    data.clear();
    pair<ID, int> tmp;
    file_aux.open(name + "_aux", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&data_length), sizeof(int));
    for (int i = 0; i < data_length; i++) {
      file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<ID, int>));
      data.insert(tmp);
    }
    file_aux.close();
  }

  void write_map() {
    file_aux.open(name + "_aux", ofstream::out);
    file_aux.write(reinterpret_cast<char *>(&data_length), sizeof(int));
    for (auto i: data) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<ID, int>));
    }
    file_aux.close();
  }

  Account read_data(int pos) {
    Account tmp{};
    file_main.open(name, std::ios::in);
    file_main.seekg((pos - 1) * sizeof(Account));
    file_main.read(reinterpret_cast<char *>(&tmp), sizeof(Account));
    file_main.close();
    return tmp;
  }

  void update_data(Account &tmp, int pos) {
    file_main.open(name, std::ios::in | std::ios::out);
    file_main.seekp((pos - 1) * sizeof(Account));
    file_main.write(reinterpret_cast<char *>(&tmp), sizeof(Account));
    file_main.close();
  }

  Account curUser() {
    return read_data(data[login_list.top().first]);
  }

  void login(const ID &id, const char password[35] = nullptr) {
    if (curUser().privilege != 7) {
      if (password == nullptr) return;
      auto it = data.find(id);
      if (it == data.end()) return;
      if (strcmp(read_data(it->second).password, password) != 0)
        return login_list.emplace(id, books());
    } else {
      auto it = data.find(id);
      if (it == data.end()) return;
      return login_list.emplace(id, books());
    }
  }

  pair<ID, books> logout() {
    auto tmp = login_list.top();
    login_list.pop();
    return tmp;
  }

};

#endif //BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H
