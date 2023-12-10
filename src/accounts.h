#ifndef BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H
#define BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstring>
#include <stack>
#include "books.h"

using std::cout;
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::stack;
using std::unordered_map;
using std::map;
using std::pair;

const int block_len = 250; // length of block
const int block_num = 250; // number of blocks

class AccountSys;

class ID {
public:
  char id[35] = "";

  ID() = default;

  explicit ID(char _key[]) {
    strcpy(id, _key);
  }

  bool operator==(const ID &rhs) const {
    return (strcmp(id, rhs.id) == 0);
  }

  bool operator<(const ID &rhs) const {
    return (strcmp(id, rhs.id) < 0);
  }

  ID &operator=(const ID &rhs) {
    if (&rhs == this) return *this;
    strcpy(id, rhs.id);
    return *this;
  }

};

class Account {

public:

  ID user_id;
  char password[35];
  char user_name[35];
  int privilege;

  friend class AccountSys;

};

class Node {
public:
  ID first = ID();
  int size = 0;
  int pos = 0;
  Account data[block_len];

  Node() = default;

  Node(ID _first, int _pos) {
    first = _first;
    pos = _pos;
    size = 0;
  }

  void print() {
    cout << first.id << " " << size << " " << pos << ":\n";
    if (pos < 3) {
      cout << "   null\n";
    }
    for (int i = 0; i < size; ++i) {
      cout << "   " << data[i].user_id.id << " " << data[i].password;
      cout << " " << data[i].user_name << " ";
      cout << data[i].privilege << '\n';
    }
  }

  void insert(const Account &ac) {
    bool inserted = false;
    for (int i = size; i > 0; i--) {
      if (ac.user_id < data[i - 1].user_id) data[i] = data[i - 1];
      if (data[i - 1].user_id < ac.user_id) {
        data[i] = ac;
        inserted = true;
        break;
      }
    }
    if (!inserted) data[0] = ac;
    size++;
    first = data[0].user_id;
  }

  void remove(Account ac) {
    bool found = false;
    for (int i = 0; i < size; i++) {
      if (!found && data[i].user_id == ac.user_id) found = true;
      if (found) data[i] = data[i + 1];
    }
    if (found) size--;
    first = data[0].user_id;
  }

  Account find(const ID &id) {
    int l=0,r=size;
    int mid;
    while(l+1<r) {
      mid=(l+r)>>1;
      if(id<data[mid].user_id) {
        r=mid;
      } else l=mid;
    }
    if(data[l].user_id==id) return data[l];
    else return {};
  }

};


class AccountSys {
public:
  string main_name;
  fstream file_main, file_aux;
  //main: Account; aux: map
  map<ID, int> list; //map the ID to the pos in main
  //read from aux
  stack<pair<ID, books>> login_list;
  int lengthofnodes = 0;
  int lengthoflist=0;
  // read from aux
  //aux: first--lengthofnodes; second--lengthoflist
  //main: account data


  AccountSys(const string &FN = "") {
    if (FN != "") main_name = FN;
    file_main.open(main_name, std::ios::in);
    file_aux.open(main_name + "_aux", std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_aux.close();
      init_main();
      char s[35]="",e[35]="zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
      ID st(s),ed(e);
      list.insert(pair<ID,int>(st,1));
      list.insert(pair<ID,int>(ed,2));
      Node head(st,1),tail(ed,2);
      append_main(head);append_main(tail);
    } else {
      file_main.close();
      file_aux.close();
      read_aux();
    }
  }

  ~AccountSys() {
    write_aux();
  }

  void init_main() {
    file_main.open(main_name, fstream::out | fstream::binary);
    file_main.close();
  }
  void write_aux(){
    file_aux.open(main_name + "_aux", fstream::out | fstream::binary);
    file_aux.write(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.write(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    for (auto i: list) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<ID, int>));
    }
    file_aux.close();
  }
  void read_aux() {
    list.clear();
    pair<ID, int> tmp{};
    file_aux.open(main_name + "_aux", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.read(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    for (int i = 0; i < lengthoflist; i++) {
      file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<ID, int>));
      list.insert(tmp);
    }
    file_aux.close();
  }

  void append_main(Node &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(Node));
    file_main.close();
    ++lengthofnodes;
    ++lengthoflist;
  }

  //main[pos]=t, 1-based
  void write_main(Node &t,const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp((pos - 1) * sizeof(Node));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }
  //t=main[pos],1-based
  void read_main(Node &t,const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg((pos - 1) * sizeof(Node));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }

  void print() {
    cout << "[lengthofnodes=" << lengthofnodes << ", lengthoflist=" << lengthoflist << "]\n";
    Node n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      cout << i.first.id << " " << i.second << '\n';
    }
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(Node));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(Node));
      n.print();
    }
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
    if (login_list.empty()) return pair<ID, books>{};
    auto tmp = login_list.top();
    login_list.pop();
    return tmp;
  }

};

#endif //BOOKSTORE_2023_LI_MIKE_ACCOUNTS_H
