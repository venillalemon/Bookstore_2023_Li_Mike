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


const int block_len_book = 100;

template<int length>
class m_string {
public:
  char id[length];

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

  Book(const ISBN &_isbn) {
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
    cout << isbn << '\t' << name << '\t' << author << '\t';
    cout << key_word << '\t';
    cout << std::fixed << std::setprecision(2) << price;
    cout << '\t' << std::setprecision(0) << storage << '\n';
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
      cout << "   " << data[i].isbn << " " << data[i].name;
      cout << " " << data[i].author << " ";
      cout << data[i].key_word << " ";
      cout << data[i].price << " " << data[i].storage << '\n';
    }
  }

  void show() {
    if (pos < 3) {
      return;
    }
    for (int i = 0; i < size; ++i) {
      cout << data[i].isbn << "\t" << data[i].name;
      cout << "\t" << data[i].author << "\t";
      cout << data[i].key_word << "\t";
      cout << std::fixed << std::setprecision(2) << data[i].price;
      cout << "\t" << std::setprecision(0) << data[i].storage << '\n';
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
        error("insert: book already exists\n");
        return;
      }
    }
    if (!inserted) data[0] = bo;
    size++;
    first = data[0].isbn;
  }

  Book remove(const ISBN &id) {
    bool found = false;
    Book tmp;
    for (int i = 0; i < size; i++) {
      if (!found && data[i].isbn == id) {
        found = true;
        tmp = data[i];
      }
      if (found) data[i] = data[i + 1];
    }
    if (found) size--;
    else error("remove: book does not exist\n");
    first = data[0].isbn;
    return tmp;
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
      //error("not found book\n");
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
      error("not found book to modify\n");
      return;
    } else {
      if (data[l].storage + change_storage >= 0) data[l].storage += change_storage;
      else {
        error("not enough to buy\n");
        return;
      }
    }
  }

  BookName modify_book(const ISBN &isbn, const BookName &name) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (isbn < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (data[l].isbn == isbn) {
      BookName tmp = data[l].name;
      data[l].name = name;
      return tmp;
    } else {
      error("not found book to modify\n");
    }
    return {};
  }

  Author modify_book(const ISBN &isbn, const Author &author) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (isbn < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (data[l].isbn == isbn) {
      Author tmp = data[l].author;
      data[l].author = author;
      return tmp;
    } else {
      error("not found book to modify\n");
    }
    return {};
  }

  KeyWord modify_book(const ISBN &isbn, const KeyWord &key_word) {
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (isbn < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (data[l].isbn == isbn) {
      KeyWord tmp = data[l].key_word;
      data[l].key_word = key_word;
      return tmp;
    } else {
      error("not found book to modify\n");
    }
    return {};
  }

  double modify_book(const ISBN& isbn,double price){
    int l = 0, r = size;
    int mid;
    while (l + 1 < r) {
      mid = (l + r) >> 1;
      if (isbn < data[mid].isbn) {
        r = mid;
      } else l = mid;
    }
    if (data[l].isbn == isbn) {
      double tmp = data[l].price;
      data[l].price = price;
      return tmp;
    } else {
      error("not found book to modify\n");
    }
    return -1;
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
  fstream file_bookname, file_author, file_keyword;

  multimap<BookName, ISBN> bn;
  multimap<Author, ISBN> au;
  multimap<KeyWord, ISBN> kw;

  explicit BookSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
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
      read_aux();
    }
  }

  ~BookSys() {
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

    file_aux.open(main_name + "_book_name", fstream::out | fstream::binary);
    for (auto i: bn) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<BookName, ISBN>));
    }
    file_aux.close();

    file_aux.open(main_name + "_author", fstream::out | fstream::binary);
    for (auto i: au) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<Author, ISBN>));
    }
    file_aux.close();

    file_aux.open(main_name + "_key_word", fstream::out | fstream::binary);
    for (auto i: kw) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<KeyWord, ISBN>));
    }
    file_aux.close();
  }

  void read_aux() {
    list.clear();
    pair<ISBN, int> aux{};
    file_aux.open(main_name + "_aux", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.read(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    for (int i = 0; i < lengthoflist; i++) {
      file_aux.read(reinterpret_cast<char *>(&aux), sizeof(pair<ISBN, int>));
      list.insert(aux);
    }
    file_aux.close();

    bn.clear();
    pair<BookName, ISBN> bookname;
    file_aux.open(main_name + "_book_name", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&bookname), sizeof(pair<BookName, ISBN>));
    while (!file_aux.eof()) {
      bn.insert(bookname);
      file_aux.read(reinterpret_cast<char *>(&bookname), sizeof(pair<BookName, ISBN>));
    }
    file_aux.close();

    au.clear();
    pair<Author, ISBN> author;
    file_aux.open(main_name + "_author", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&author), sizeof(pair<Author, ISBN>));
    while (!file_aux.eof()) {
      au.insert(author);
      file_aux.read(reinterpret_cast<char *>(&author), sizeof(pair<Author, ISBN>));
    }
    file_aux.close();

    kw.clear();
    pair<KeyWord, ISBN> keyword;
    file_aux.open(main_name + "_key_word", ifstream::in);
    file_aux.read(reinterpret_cast<char *>(&keyword), sizeof(pair<KeyWord, ISBN>));
    while (!file_aux.eof()) {
      kw.insert(keyword);
      file_aux.read(reinterpret_cast<char *>(&keyword), sizeof(pair<KeyWord, ISBN>));
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
    cout << "Books: [lengthofnodes=" << lengthofnodes << ", lengthoflist=" << lengthoflist << "]\n";
    BookNode n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      cout << i.first << " " << i.second << '\n';
    }
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(BookNode));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(BookNode));
      n.print();
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
    BookNode n;
    file_main.open(main_name, std::ifstream::in);
    for (auto i: list) {
      file_main.seekg((i.second - 1) * sizeof(BookNode));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(BookNode));
      n.show();
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

  void find_book(const ISBN &isbn) {
    if (bookinfo(isbn).isbn == isbn) bookinfo(isbn).show();
    else cout << '\n';
  }

  void find_book(const BookName &book_name) {
    multimap<BookName, ISBN>::iterator it;
    set<ISBN> v;
    for (it = bn.lower_bound(book_name); it != bn.upper_bound(book_name); ++it) {
      v.insert(it->second);
    }
    if (v.empty()) cout << '\n';
    else
      for (const auto &t: v) {
        bookinfo(t).show();
      }
  }

  void find_book(const Author &author) {
    multimap<Author, ISBN>::iterator it;
    set<ISBN> v;
    for (it = au.lower_bound(author); it != au.upper_bound(author); ++it) {
      v.insert(it->second);
    }
    if (v.empty()) cout << '\n';
    else
      for (const auto &t: v) {
        bookinfo(t).show();
      }
  }

  void find_book(const KeyWord &key_word) {
    multimap<KeyWord, ISBN>::iterator it;
    set<ISBN> v;
    for (it = kw.lower_bound(key_word); it != kw.upper_bound(key_word); ++it) {
      v.insert(it->second);
    }
    if (v.empty()) cout << '\n';
    else
      for (const auto &t: v) {
        bookinfo(t).show();
      }
  }

  void insert_book(const Book &bo) {
    Book tmp = bookinfo(bo.isbn);
    if (tmp.isbn == bo.isbn) {
      error("isbn exists\n");
    }
    auto it = list.lower_bound(bo.isbn);
    auto last = it;
    it--;

    if (it == list.begin()) {
      if (lengthoflist == 2) {
        //cout << "first node\n";
        first_node(bo);
      } else {
        BookNode next_node;
        int pos = (*last).second;
        read_main(next_node, pos);
        list.erase(next_node.first);
        next_node.insert(bo);
        write_main(next_node, pos);
        list.insert(pair<ISBN, int>(next_node.first, pos));
        if (next_node.size >= block_len_book - 20) {
          divide_node(pos);
        }
      }
    } else {
      BookNode next_node;
      read_main(next_node, (*it).second);
      next_node.insert(bo);
      write_main(next_node, (*it).second);
      if (next_node.size >= block_len_book - 20) {
        divide_node((*it).second);
      }
    }
    bn.insert({bo.name, bo.isbn});
    au.insert({bo.author, bo.isbn});
    insert_key_word(bo.key_word.id, bo.isbn);
    //print();
  }

  void insert_key_word(const char key_word[70], const ISBN &isbn) {
    char key_list[70];
    strcpy(key_list, key_word);
    char t[70];
    char *token = strtok(key_list, "|");
    while (token != nullptr) {
      strcpy(t, token);
      kw.insert({KeyWord(t), isbn});
      token = strtok(nullptr, "|");
    }
  }

  //divide the node at pos into 2 parts, the other part is at the tail
  void divide_node(int pos) {
    BookNode node;
    read_main(node, pos);

    BookNode new_node(node.data[node.size / 2].isbn, lengthofnodes + 1);

    for (int i = node.size / 2; i < node.size; i++) {
      new_node.data[i - node.size / 2] = node.data[i];
      new_node.size++;
    }
    node.size /= 2;
    list.insert(pair<ISBN, int>(new_node.first, lengthofnodes + 1));
    write_main(node, pos);
    append_main(new_node);
  }

  //insert the AccountNode at the head of the list
  void first_node(const Book &bo) {
    BookNode new_node(bo.isbn, lengthofnodes + 1);
    new_node.size = 1;
    new_node.data[0] = bo;
    list.insert(pair<ISBN, int>(bo.isbn, lengthofnodes + 1));
    append_main(new_node);
  }

  Book remove_book(const ISBN &isbn) {
    auto del = list.upper_bound(isbn);
    del--;
    BookNode node;
    Book tmp;
    int pos = (*del).second;
    if (pos != 1 && pos != 2) {
      read_main(node, pos);
      list.erase(node.first);
      tmp = node.remove(isbn);//the above two lines cannot be reversed
      write_main(node, pos);
      if (node.size != 0) list.insert(pair<ISBN, int>(node.first, node.pos));
      else {
        lengthoflist--;
      }
      remove_from_bn(tmp.name, tmp.isbn);
      remove_from_au(tmp.author, tmp.isbn);
      remove_from_kw(tmp.key_word, tmp.isbn);
    } else {
      error("not found book to remove");
    }
    return tmp;
  }

  void remove_from_bn(const BookName &book_name, const ISBN &isbn) {
    multimap<BookName, ISBN>::iterator it;
    for (it = bn.lower_bound(book_name); it != bn.upper_bound(book_name); ++it) {
      if (it->second == isbn) {
        bn.erase(it);
        return;
      }
    }
  }

  void remove_from_au(const Author &author, const ISBN &isbn) {
    multimap<Author, ISBN>::iterator it;
    for (it = au.lower_bound(author); it != au.upper_bound(author); ++it) {
      if (it->second == isbn) {
        au.erase(it);
        return;
      }
    }
  }

  void remove_from_kw(const KeyWord &key_word, const ISBN &isbn) {
    multimap<KeyWord, ISBN>::iterator it;
    char key_list[70];
    strcpy(key_list, key_word.id);
    char *token = strtok(key_list, "|");
    char t[70];
    while (token != nullptr) {
      strcpy(t, token);
      for (it = kw.lower_bound(KeyWord(t)); it != kw.upper_bound(KeyWord(t)); ++it) {
        if (it->second == isbn) {
          kw.erase(it);
          break;
        }
      }
      token = strtok(nullptr, "|");
    }
  }

  void modify_book(const ISBN &isbn, const BookName &name) {
    auto mod = list.upper_bound(isbn);
    mod--;
    BookNode node;
    if ((*mod).second != 1 && (*mod).second != 2) {
      read_main(node, (*mod).second);

      BookName n = node.modify_book(isbn, name);
      write_main(node, (*mod).second);

      remove_from_bn(n, isbn);
      bn.insert({name, isbn});
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, const Author &author) {
    auto mod = list.upper_bound(isbn);
    mod--;
    BookNode node;
    if ((*mod).second != 1 && (*mod).second != 2) {
      read_main(node, (*mod).second);

      Author a = node.modify_book(isbn, author);
      write_main(node, (*mod).second);

      remove_from_au(a, isbn);
      au.insert({author, isbn});
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN &isbn, const KeyWord &key_word) {
    auto mod = list.upper_bound(isbn);
    mod--;
    BookNode node;
    if ((*mod).second != 1 && (*mod).second != 2) {
      read_main(node, (*mod).second);

      KeyWord k = node.modify_book(isbn, key_word);
      write_main(node, (*mod).second);

      remove_from_kw(k, isbn);
      insert_key_word(key_word.id, isbn);
    } else {
      error("not found book to modify\n");
    }
  }

  void modify_book(const ISBN& isbn,double price){
    auto mod = list.upper_bound(isbn);
    mod--;
    BookNode node;
    if ((*mod).second != 1 && (*mod).second != 2) {
      read_main(node, (*mod).second);
      node.modify_book(isbn, price);
      write_main(node, (*mod).second);
    } else {
      error("not found book to modify\n");
    }
  }

};

#endif
