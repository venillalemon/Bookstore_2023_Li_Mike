#ifndef BOOKS_H
#define BOOKS_H
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <cstring>
#include <iomanip>
#include "error.h"

using std::cout;
using std::ostream;
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::set;
using std::unordered_map;
using std::map;
using std::multimap;
using std::pair;
using std::unordered_multimap;


template<int length>
class m_string {
public:
  char id[length]{};

  m_string() = default;

  explicit m_string(const char _key[]) {
    strcpy(id, _key);
  }

  m_string &operator=(const m_string &rhs) {
    if (this == &rhs) return *this;
    strcpy(id, rhs.id);
    return *this;
  }

  bool operator==(const m_string &rhs) const {
    return (strcmp(id, rhs.id) == 0);
  }

  bool operator!=(const m_string &rhs) const {
    return (strcmp(id, rhs.id) != 0);
  }

  bool operator<(const m_string &rhs) const {
    return (strcmp(id, rhs.id) < 0);
  }
};

namespace std {
    template<int length>
    struct hash<m_string < length>> {
    size_t operator()(const m_string <length> &x) const {
      return hash<string>()(x.id);
    }
};
}

template<int length>
ostream &operator<<(ostream &os, const m_string<length> m) {
  os << m.id;
  return os;
}


typedef m_string<25> ISBN;
typedef m_string<68> BookName;
typedef m_string<69> Author;
typedef m_string<70> KeyWord;

class Book {
public:
  ISBN isbn{};
  BookName name{};
  Author author{};
  KeyWord key_word{};
  double price = 0;
  int storage = 0;
  bool exist = true;
public:
  Book() = default;

  explicit Book(const ISBN &_isbn) {
    isbn = _isbn;
  }

  Book(char i[25], char n[70], char a[70], char k[70], double p) {
    isbn = ISBN(i);
    name = BookName(n);
    author = Author(a);
    key_word = KeyWord(k);
    price = p;
  }

  void show() const {
    //if(!exist) return;
    cout << isbn << '\t' << name << '\t' << author << '\t';
    cout << key_word << '\t';
    cout << std::fixed << std::setprecision(2) << price;
    cout << '\t' << std::setprecision(0) << storage << '\n';
  }

  bool operator<(const Book &rhs) const {
    return isbn < rhs.isbn;
  }
};

class BookSys {

public:
  string main_name;
  fstream file_main, file_aux;
  //main: Account; aux: map
  map<ISBN, int> list; //map the ISBN to the pos in main
  //read from aux
  int book_num = 0;
  // read from aux
  //aux: first--lengthofnodes; second--lengthoflist
  //main: account data

  unordered_multimap<BookName, int> bn;
  unordered_multimap<Author, int> au;
  unordered_multimap<KeyWord, int> kw;

  explicit BookSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      init_main();
    } else {
      file_main.close();
      read_aux();
    }
  }

  ~BookSys() = default;

  void init_main() {
    file_main.open(main_name, fstream::out | fstream::binary);
    file_main.close();
  }

  void write_aux() {
    file_aux.open(main_name + "_num", fstream::out | fstream::binary);
    file_aux.write(reinterpret_cast<char *>(&book_num), sizeof(int));
    file_aux.close();
    file_aux.open(main_name + "_aux", fstream::out | fstream::binary);
    for (auto i: list) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<ISBN, int>));
    }
    file_aux.close();

    file_aux.open(main_name + "_book_name", fstream::out | fstream::binary);
    for (auto i: bn) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<BookName, int>));
    }
    file_aux.close();

    file_aux.open(main_name + "_author", fstream::out | fstream::binary);
    for (auto i: au) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<Author, int>));
    }
    file_aux.close();

    file_aux.open(main_name + "_key_word", fstream::out | fstream::binary);
    for (auto i: kw) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<KeyWord, int>));
    }
    file_aux.close();
  }

  void read_aux() {
    list.clear();
    pair<ISBN, int> aux{};
    file_aux.open(main_name + "_num", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&book_num), sizeof(int));
    file_aux.close();
    file_aux.open(main_name + "_aux", ifstream::in);
    for (int i = 0; i < book_num; i++) {
      file_aux.read(reinterpret_cast<char *>(&aux), sizeof(pair<ISBN, int>));
      list.insert(aux);
    }
    file_aux.close();

    bn.clear();
    pair<BookName,int> bookname;
    file_aux.open(main_name + "_book_name", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&bookname), sizeof(pair<BookName, int>));
    while (!file_aux.eof()) {
      bn.insert(bookname);
      file_aux.read(reinterpret_cast<char *>(&bookname), sizeof(pair<BookName, int>));
    }
    file_aux.close();

    au.clear();
    pair<Author, int> author;
    file_aux.open(main_name + "_author", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&author), sizeof(pair<Author, int>));
    while (!file_aux.eof()) {
      au.insert(author);
      file_aux.read(reinterpret_cast<char *>(&author), sizeof(pair<Author, int>));
    }
    file_aux.close();

    kw.clear();
    pair<KeyWord, int> keyword;
    file_aux.open(main_name + "_key_word", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&keyword), sizeof(pair<KeyWord, int>));
    while (!file_aux.eof()) {
      kw.insert(keyword);
      file_aux.read(reinterpret_cast<char *>(&keyword), sizeof(pair<KeyWord, int>));
    }
    file_aux.close();
  }

  void append_main(Book &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(Book));
    file_main.close();
    ++book_num;
  }

  //main[pos]=t, 1-based
  void write_main(Book &t, const int pos) {
    if (pos > book_num) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp((pos - 1) * sizeof(Book));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(Book));
    file_main.close();
  }

  //t=main[pos],1-based
  void read_main(Book &t, const int pos) {
    if (pos > book_num) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg((pos - 1) * sizeof(Book));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(Book));
    file_main.close();
  }

  void print() {
    cout << "Books: [book_num=" << book_num << "]\n";
    Book n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(Book));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(Book));
      n.show();
    }
    file_main.close();
    cout << "BookName:\n";
    for (auto i: bn) {
      cout << i.first << " " << i.second << '\n';
    }
    cout << "Author:\n";
    for (auto i: au) {
      cout << i.first << " " << i.second << '\n';
    }
    cout << "KeyWord:\n";
    for (auto i: kw) {
      cout << i.first << " " << i.second << '\n';
    }
  }

  void show() {
    Book n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(Book));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(Book));
      n.show();
    }
    file_main.close();
  }

  Book bookinfo(const ISBN &isbn) {
    auto it = list.find(isbn);
    if(it==list.end()) return {};
    Book tmp;
    read_main(tmp, it->second);
    return tmp;
  }

  void find_book(const ISBN &isbn) {
    Book tmp = bookinfo(isbn);
    if (tmp.isbn == isbn) tmp.show();
    else cout << '\n';
  }

  void find_book(const BookName &book_name) {
    vector<Book> v;
    auto range = bn.equal_range(book_name);
    Book tmp;
    for (auto it = range.first; it != range.second; it++) {
      read_main(tmp, it->second);
      v.push_back(tmp);
    }

    if (v.empty()) cout << '\n';
    else {
      sort(v.begin(), v.end());
      for (const auto &t: v) {
        t.show();
      }
    }
  }

  void find_book(const Author &author) {
    vector<Book> v;
    auto range = au.equal_range(author);
    Book tmp;
    for (auto it = range.first; it != range.second; it++) {
      read_main(tmp, it->second);
      v.push_back(tmp);
    }

    if (v.empty()) cout << '\n';
    else {
      sort(v.begin(), v.end());
      for (const auto &t: v) {
        t.show();
      }
    }
  }

  void find_book(const KeyWord &key_word) {
    vector<Book> v;
    auto range = kw.equal_range(key_word);
    Book tmp;
    for (auto it = range.first; it != range.second; it++) {
      read_main(tmp, it->second);
      v.push_back(tmp);
    }

    if (v.empty()) cout << '\n';
    else {
      sort(v.begin(), v.end());
      for (const auto &t: v) {
        t.show();
      }
    }
  }

  void insert_book(Book bo) {
    auto it=list.find(bo.isbn);
    if(it!=list.end()){
      error("insert book: ISBN exists\n");
      return;
    }
    append_main(bo);
    list.insert({bo.isbn, book_num});
    bn.insert({bo.name, book_num});
    au.insert({bo.author, book_num});
    insert_key_word(bo.key_word.id, book_num);
    //print();
  }

  void insert_key_word(const char key_word[70], int pos) {
    char key_list[70];
    strcpy(key_list, key_word);
    char t[70];
    char *token = strtok(key_list, "|");
    while (token != nullptr) {
      strcpy(t, token);
      kw.insert({KeyWord(t), pos});
      token = strtok(nullptr, "|");
    }
  }

  Book remove_book(const ISBN &isbn) {
    auto it = list.find(isbn);
    if (it!=list.end()) {
      Book tmp;
      read_main(tmp, it->second);
      remove_from_bn(tmp.name, it->second);
      remove_from_au(tmp.author, it->second);
      remove_from_kw(tmp.key_word, it->second);
      tmp.exist = false;
      write_main(tmp, it->second);
      list.erase(it);
      return tmp;
    } else {
      error("not found book to remove");
      return {};
    }
  }

  void remove_from_bn(const BookName &book_name, int pos) {
    auto range = bn.equal_range(book_name);
    for (auto it = range.first; it != range.second; it++) {
      if (it->second == pos) {
        bn.erase(it);
        return;
      }
    }
  }

  void remove_from_au(const Author &author, int pos) {
    auto range = au.equal_range(author);
    for (auto it = range.first; it != range.second; it++) {
      if (it->second == pos) {
        au.erase(it);
        return;
      }
    }
  }

  void remove_from_kw(const KeyWord &key_word, int pos) {
    char key_list[70];
    strcpy(key_list, key_word.id);
    char *token = strtok(key_list, "|");
    char t[70];
    while (token != nullptr) {
      strcpy(t, token);
      auto range = kw.equal_range(KeyWord(t));
      for (auto it = range.first; it != range.second; it++) {
        if (it->second == pos) {
          kw.erase(it);
          break;
        }
      }
      token = strtok(nullptr, "|");
    }
  }

  void modify_book(const ISBN &isbn, const BookName &name) {
    auto mod = list.find(isbn);

    if (mod!=list.end()) {
      Book node;
      read_main(node, (*mod).second);

      BookName n = node.name;
      node.name=name;
      write_main(node, (*mod).second);

      remove_from_bn(n, mod->second);
      bn.insert({name, mod->second});
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, const Author &author) {
    auto mod = list.find(isbn);

    if (mod!=list.end()) {
      Book node;
      read_main(node, (*mod).second);

      Author n = node.author;
      node.author=author;
      write_main(node, (*mod).second);

      remove_from_au(n, mod->second);
      au.insert({author, mod->second});
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, const KeyWord &key_word) {
    auto mod = list.find(isbn);

    if (mod!=list.end()) {
      Book node;
      read_main(node, (*mod).second);

      KeyWord k = node.key_word;
      node.key_word=key_word;
      write_main(node, (*mod).second);

      remove_from_kw(k, (*mod).second);
      insert_key_word(key_word.id, (*mod).second);
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, double price) {
    auto mod = list.find(isbn);

    if (mod!=list.end()) {
      Book node;
      read_main(node, (*mod).second);
      node.price=price;
      write_main(node, (*mod).second);
    } else {
      error("not found book to modify\n");
    }
  }

};

#endif
