#ifndef _BOOKS_H
#define _BOOKS_H
#pragma once

#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <map>
#include <cstring>

using std::cout;
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::stack;
using std::unordered_map;
using std::map;
using std::pair;


const int block_len_book = 250;

class ISBN {
public:
  char id[25];

  ISBN() = default;

  explicit ISBN(char _key[]) {
    strcpy(id, _key);
  }

  ISBN &operator=(const ISBN &rhs) {
    if (this == &rhs) return *this;
    strcpy(id, rhs.id);
    return *this;
  }

  bool operator==(const ISBN &rhs) const {
    return (strcmp(id, rhs.id) == 0);
  }

  bool operator<(const ISBN &rhs) const {
    return (strcmp(id, rhs.id) < 0);
  }
};

extern stack<pair<ID, ISBN>> login_list;

class Book {
public:
  ISBN isbn{};
  char name[70]{};
  char author[70]{};
  char key_word[70]{};
  int price = 0;
  int storage = 0;
public:
  Book() = default;

  Book(ISBN &_isbn, char n[70], char a[70], char k[70], int p) {
    isbn = _isbn;
    strcpy(name, n);
    strcpy(author, a);
    strcpy(key_word, k);
    price = p;
  }
};

class BookNode {
public:
  ISBN first{};
  int size = 0;
  int pos = 0;
  Book data[block_len_book];

  BookNode() = default;

  BookNode(const ISBN &i, int p) {
    first = i;
    pos = p;
    size = 0;
  }

  void print() {
    cout << first.id << " " << size << " " << pos << ":\n";
    if (pos < 3) {
      cout << "   null\n";
    }
    for (int i = 0; i < size; ++i) {
      cout << "   " << data[i].isbn.id << " " << data[i].name;
      cout << " " << data[i].author << " ";
      cout << data[i].key_word << '\n';
    }
  }

  void insert(const Book &bo) {
    bool inserted = false;
    for (int i = size; i > 0; i--) {
      if (bo.isbn < data[i - 1].isbn) data[i] = data[i - 1];
      if (data[i - 1].isbn < bo.isbn) {
        data[i] = bo;
        inserted = true;
        break;
      }
      if (bo.isbn == data[i - 1].isbn) {
        cout << "account already exists\n";
        return;
      }
    }
    if (!inserted) data[0] = bo;
    size++;
    first = data[0].isbn;
  }

  Book find(const ISBN &id) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (id < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (data[l].isbn == id) return data[l];
    else {
      cout << "not found\n";
      return {};
    }
  }

  void modify(const ISBN &id, int change_storage) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (id < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (!(data[l].isbn == id)) {
      cout << "not found to modify\n";
      return;
    } else {
      if (data[l].storage + change_storage >= 0) data[l].storage += change_storage;
      else {
        cout << "not enough to buy\n";
        return;
      }
    }
  }

};

class BookSys {

public:
  string main_name;
  fstream file_main, file_aux;
  //main: Account; aux: map
  map<ISBN, int> list; //map the ID to the pos in main
  //read from aux
  int lengthofnodes = 0;
  int lengthoflist = 0;
  // read from aux
  //aux: first--lengthofnodes; second--lengthoflist
  //main: account data

  explicit BookSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    file_aux.open(main_name + "_aux", std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_aux.close();
      init_main();
      char s[25] = "", e[25] = "zzzzzzzzzzzzzzzzzzzzzzzz";
      ISBN st(s), ed(e);
      list.insert(pair<ISBN, int>(st, 1));
      list.insert(pair<ISBN, int>(ed, 2));
      BookNode head(st, 1), tail(ed, 2);
      append_main(head);
      append_main(tail);

    } else {
      file_main.close();
      file_aux.close();
      read_aux();
    }
  }

  ~BookSys() {
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
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<ISBN, int>));
    }
    file_aux.close();
  }

  void read_aux() {
    list.clear();
    pair<ISBN, int> tmp{};
    file_aux.open(main_name + "_aux", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.read(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    for (int i = 0; i < lengthoflist; i++) {
      file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<ISBN, int>));
      list.insert(tmp);
    }
    file_aux.close();
  }

  void append_main(BookNode &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(BookNode));
    file_main.close();
    ++lengthofnodes;
    ++lengthoflist;
  }

  //main[pos]=t, 1-based
  void write_main(BookNode &t, const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp((pos - 1) * sizeof(BookNode));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(BookNode));
    file_main.close();
  }

  //t=main[pos],1-based
  void read_main(BookNode &t, const int pos) {
    if (pos > lengthofnodes) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg((pos - 1) * sizeof(BookNode));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(BookNode));
    file_main.close();
  }

  void print() {
    cout << "[lengthofnodes=" << lengthofnodes << ", lengthoflist=" << lengthoflist << "]\n";
    BookNode n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      cout << i.first.id << " " << i.second << '\n';
    }
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(BookNode));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(BookNode));
      n.print();
    }
    file_main.close();
  }

  Book bookinfo(const ISBN &isbn) {
    auto it = list.upper_bound(isbn);
    it--;
    BookNode tmp;
    read_main(tmp, (*it).second);
    return tmp.find(isbn);
  }

  void buy(int quantity) {
    auto it = list.upper_bound(login_list.top().second);
    it--;
    BookNode tmp;
    read_main(tmp, (*it).second);
    tmp.modify(login_list.top().second, -quantity);
    write_main(tmp, (*it).second);
  }

};


#endif
