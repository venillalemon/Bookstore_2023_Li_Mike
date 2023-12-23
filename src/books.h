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
#include "blocklist.h"

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
    struct hash<m_string<length>> {
      size_t operator()(const m_string<length> &x) const {
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
  //read from aux
  int book_num = 0;

  BlockList<ISBN, 50> ib_list;
  BlockList<BookName, 50> bn_list;
  BlockList<Author, 50> au_list;
  BlockList<KeyWord, 100> kw_list;

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
    ib_list.init(main_name + "_isbn");
    bn_list.init(main_name + "_bn");
    au_list.init(main_name + "_au");
    kw_list.init(main_name + "_kw");
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

    ib_list.write_list();
    bn_list.write_list();
    au_list.write_list();
    kw_list.write_list();
  }

  void read_aux() {
    file_aux.open(main_name + "_num", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&book_num), sizeof(int));
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
    ib_list.print();
    cout << "BookName:\n";
    bn_list.print();
    cout << "Author:\n";
    au_list.print();
    cout << "KeyWord:\n";
    kw_list.print();
  }

  void show() {
    auto v=ib_list.all_books();
    Book b;
    for(auto pos:v){
      read_main(b,pos);
      b.show();
    }
  }

  Book bookinfo(const ISBN &isbn) {
    auto it = ib_list.get_data(isbn);
    if (it.empty()) return {};
    Book tmp;
    read_main(tmp, it[0]);
    return tmp;
  }

  void find_book(const ISBN &isbn) {
    Book tmp = bookinfo(isbn);
    if (tmp.isbn == isbn) tmp.show();
    else cout << '\n';
  }

  void find_book(const BookName &book_name) {
    Book tmp;

    vector<int> i = bn_list.get_data(book_name);
    vector<Book> vf;
    for (auto it: i) {
      read_main(tmp, it);
      vf.push_back(tmp);
    }
    if (vf.empty()) cout << '\n';
    else {
      sort(vf.begin(), vf.end());
      for (const auto &t: vf) {
        t.show();
      }
    }
  }

  void find_book(const Author &author) {
    Book tmp;
    vector<int> a = au_list.get_data(author);
    vector<Book> af;
    for (auto it: a) {
      read_main(tmp, it);
      af.push_back(tmp);
    }
    if (af.empty()) cout << '\n';
    else {
      sort(af.begin(), af.end());
      for (const auto &t: af) {
        t.show();
      }
    }
  }

  void find_book(const KeyWord &key_word) {
    Book tmp;
    vector<int> k = kw_list.get_data(key_word);
    vector<Book> kf;
    for (auto it: k) {
      read_main(tmp, it);
      kf.push_back(tmp);
    }
    if (kf.empty()) cout << '\n';
    else {
      sort(kf.begin(), kf.end());
      for (const auto &t: kf) {
        t.show();
      }
    }
  }

  void insert_book(Book bo) {
    auto it = ib_list.get_data(bo.isbn);
    if (!it.empty()) {
      error("insert book: ISBN exists\n");
      return;
    }
    append_main(bo);
    ib_list.insert_pair(bo.isbn, book_num);
    bn_list.insert_pair(bo.name, book_num);
    au_list.insert_pair(bo.author, book_num);
    insert_key_word(bo.key_word.id, book_num);
    //print();
  }

  void insert_key_word(const char key_word[70], int pos) {
    char key_list[70];
    strcpy(key_list, key_word);
    char t[70];
    char *token = strtok(key_list, "|");
    KeyWord key_word_;
    while (token != nullptr) {
      strcpy(t, token);
      key_word_ = KeyWord(t);
      kw_list.insert_pair(key_word_, pos);
      token = strtok(nullptr, "|");
    }
  }

  void remove_from_kw(const KeyWord &key_word, int pos) {
    char key_list[70];
    strcpy(key_list, key_word.id);
    char *token = strtok(key_list, "|");
    char t[70];
    KeyWord key_word_;
    while (token != nullptr) {
      strcpy(t, token);
      key_word_ = KeyWord(t);
      kw_list.remove_pair(key_word_, pos);
      token = strtok(nullptr, "|");
    }
  }

  void modify_book(const ISBN &isbn, ISBN &new_isbn) {
    auto mod = ib_list.get_data(isbn);
    auto it = ib_list.get_data(new_isbn);
    if (!it.empty()) {
      error("modify book: ISBN exists\n");
      return;
    }
    if (mod.empty()) {
      error("modify book: book not found\n");
      return;
    }

    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);

      ISBN i = node.isbn;
      node.isbn = new_isbn;
      write_main(node, mod[0]);

      ib_list.remove_pair(i, mod[0]);
      ib_list.insert_pair(new_isbn, mod[0]);
    } else {
      error("modify: repeated ISBN\n");
    }
  }

  void modify_book(const ISBN &isbn, BookName &name) {
    auto mod = ib_list.get_data(isbn);

    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);

      BookName n = node.name;
      node.name = name;
      write_main(node, mod[0]);

      bn_list.remove_pair(n, mod[0]);
      bn_list.insert_pair(name, mod[0]);
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, Author &author) {
    auto mod = ib_list.get_data(isbn);

    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);

      Author a = node.author;
      node.author = author;
      write_main(node, mod[0]);

      au_list.remove_pair(a, mod[0]);
      au_list.insert_pair(author, mod[0]);
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, const KeyWord &key_word) {
    auto mod = ib_list.get_data(isbn);

    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);

      KeyWord k = node.key_word;
      node.key_word = key_word;
      write_main(node, mod[0]);

      remove_from_kw(k, mod[0]);
      insert_key_word(key_word.id, mod[0]);
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, double price) {
    auto mod = ib_list.get_data(isbn);

    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);
      node.price = price;
      write_main(node, mod[0]);
    } else {
      error("not found book to modify\n");
    }
  }

  void import_book(const ISBN &isbn, int quantity) {
    auto mod = ib_list.get_data(isbn);
    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);
      node.storage += quantity;
      write_main(node, mod[0]);
    } else {
      error("not found book to import\n");
    }
  }

  double buy_book(const ISBN &isbn, int quantity) {
    auto mod = ib_list.get_data(isbn);
    if (mod.size() == 1) {
      Book node;
      read_main(node, mod[0]);
      if (node.storage < quantity) {
        error("buy book: not enough storage\n");
        return -1;
      }
      node.storage -= quantity;
      write_main(node, mod[0]);
      return quantity * node.price;
    } else {
      error("not found book to buy\n");
      return -1;
    }
  }

};

#endif
