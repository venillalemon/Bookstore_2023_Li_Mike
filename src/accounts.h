#ifndef _ACCOUNTS_H
#define _ACCOUNTS_H
#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstring>
#include <stack>

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
  char password[35]{};
  char user_name[35]{};
  int privilege{};

  Account()=default;

  Account(ID &id,char p[35],char u[35],int pr) {
    user_id=id;
    strcpy(password,p);
    strcpy(user_name,u);
    privilege=pr;
  }

};

class AccountNode {

public:
  ID first = ID();
  int size = 0;
  int pos = 0;
  Account data[block_len];

  AccountNode() = default;

  AccountNode(ID _first, int _pos) {
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
      if (ac.user_id == data[i - 1].user_id) {
        cout << "account already exists\n";
        return;
      }
    }
    if (!inserted) data[0] = ac;
    size++;
    first = data[0].user_id;
  }

  void remove(const ID &id) {
    bool found = false;
    for (int i = 0; i < size; i++) {
      if (!found && data[i].user_id == id) found = true;
      if (found) data[i] = data[i + 1];
    }
    if (found) size--;
    else cout << "account does not exist\n";
    first = data[0].user_id;
  }

  Account find(const ID &id) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (id < data[mid].user_id) {
        r = mid;
      } else l = mid;
    }
    if (data[l].user_id == id) return data[l];
    else {
      cout << "not found\n";
      return {};
    }
  }

};


class AccountSys {
public:
  string main_name;
  fstream file_main, file_aux;
  //main: Account; aux: map
  map<ID, int> list; //map the ID to the pos in main
  //read from aux

  int lengthofnodes = 0;
  int lengthoflist = 0;
  // read from aux
  //aux: first--lengthofnodes; second--lengthoflist
  //main: account data


  explicit AccountSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    file_aux.open(main_name + "_aux", std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_aux.close();
      init_main();
      char s[35] = "", e[35] = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz",r[35]="root";
      ID st(s), ed(e),rt(r);
      list.insert(pair<ID, int>(st, 1));
      list.insert(pair<ID, int>(ed, 2));
      AccountNode head(st, 1), tail(ed, 2);
      append_main(head);
      append_main(tail);
      // root account for manager
      list.insert(pair<ID, int>(rt, 3));
      AccountNode first(rt,3);
      first.data[0]=Account{rt,"sjtu","root",7};
      first.size=1;
      append_main(first);
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

  void write_aux() {
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

  void append_main(AccountNode &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(AccountNode));
    file_main.close();
    ++lengthofnodes;
    ++lengthoflist;
  }

  //main[pos]=t, 1-based
  void write_main(AccountNode &t, const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp((pos - 1) * sizeof(AccountNode));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(AccountNode));
    file_main.close();
  }

  //t=main[pos],1-based
  void read_main(AccountNode &t, const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg((pos - 1) * sizeof(AccountNode));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(AccountNode));
    file_main.close();
  }

  void print() {
    cout << "[lengthofnodes=" << lengthofnodes << ", lengthoflist=" << lengthoflist << "]\n";
    AccountNode n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      cout << i.first.id << " " << i.second << '\n';
    }
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(AccountNode));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(AccountNode));
      n.print();
    }
    file_main.close();
  }

  // input id return account
  Account user(const ID &id) {
    auto it = list.upper_bound(id);
    it--;
    AccountNode tmp;
    read_main(tmp, (*it).second);
    return tmp.find(id);
  }

  //find the first node that > the given key, then insert the given key before it
  void insert_account(const Account &ac) {
    Account tmp = user(ac.user_id);
    if (tmp.user_id == ac.user_id) {
      cout << "id exists\n";
      return;
    }
    auto it = list.lower_bound(ac.user_id);
    auto last = it;
    it--;

    if (it == list.begin()) {
      if (lengthoflist == 2) {
        first_node(ac);
      } else {
        AccountNode next_node;
        int pos = (*last).second;
        read_main(next_node, pos);
        list.erase(next_node.first);
        next_node.insert(ac);
        write_main(next_node, pos);
        list.insert(pair<ID, int>(next_node.first, pos));
        if (next_node.size >= block_len - 20) {
          divide_node(pos);
        }
      }
    } else {
      AccountNode next_node;
      read_main(next_node, (*it).second);
      next_node.insert(ac);
      write_main(next_node, (*it).second);
      if (next_node.size >= block_len - 20) {
        divide_node((*it).second);
      }
    }
    //print();
  }

  //divide the node at pos into 2 parts, the other part is at the tail
  void divide_node(int pos) {
    AccountNode node;
    read_main(node, pos);

    AccountNode new_node(node.data[node.size / 2].user_id, lengthofnodes + 1);

    for (int i = node.size / 2; i < node.size; i++) {
      new_node.data[i - node.size / 2] = node.data[i];
      new_node.size++;
    }
    node.size /= 2;
    list.insert(pair<ID, int>(new_node.first, lengthofnodes + 1));
    write_main(node, pos);
    append_main(new_node);
  }

  //insert the AccountNode before the AccountNode at pos
  void first_node(const Account &ac) {
    AccountNode new_node(ac.user_id, lengthofnodes + 1);
    new_node.size = 1;
    list.insert(pair<ID, int>(ac.user_id, lengthofnodes + 1));
    append_main(new_node);
  }

  void remove_account(const ID &id) {
    auto del = list.upper_bound(id);
    del--;
    AccountNode node;
    int pos = (*del).second;
    if (pos != 1 && pos != 2) {
      read_main(node, pos);
      list.erase(node.first);
      node.remove(id);
      write_main(node, pos);
      if (node.size != 0) list.insert(pair<ID, int>(node.first, node.pos));
      else {
        lengthoflist--;
      }
    }
  }



};

#endif
