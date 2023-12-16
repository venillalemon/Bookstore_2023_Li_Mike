#ifndef CODE_LOGINSTATE_H
#define CODE_LOGINSTATE_H

#include "accounts.h"
#include "books.h"
#include "finance.h"
#include "error.h"

stack<pair<ID, ISBN>> login_list;
AccountSys as("account");
BookSys bs("book");
FinanceSys fs("finance");

//get the current user
Account curUser() {
  if (login_list.empty()) error("visitor mode");
  ID i = login_list.top().first;
  return as.user(i);
}

Book curBook() {
  if (login_list.empty()) error("import/buy: no user logged in");
  ISBN i = login_list.top().second;
  if (i == ISBN()) error("import/buy: no book selected");
  return bs.bookinfo(i);
}

void print(){
  cout<<"==============as\n";
  as.print();
  cout<<"==============bs\n";
  bs.print();
  cout<<"==============end\n";
}

void login(const ID &id, const char password[35] = nullptr) {
  if (curUser().privilege != 7) {
    if (password == nullptr) {
      error("require password");
    }
    Account i = as.user(id);
    if (!(i.user_id == id)) {
      error("account not found to log in");
    }
    if (strcmp(i.password, password) == 0)
      login_list.emplace(id, ISBN());
    else error("wrong password");
  } else {
    Account i = as.user(id);
    if (!(i.user_id == id)) {
      error("account not found to log in");
    }
    login_list.emplace(id, ISBN());
  }
}

pair<ID, ISBN> logout() {
  if (login_list.empty()) {
    error("logout: no user logged in\n");
  }
  auto tmp = login_list.top();
  login_list.pop();
  cout << tmp.first.id << " " << tmp.second.id << '\n';
  return tmp;
}

void resetpasswd(const ID &id, char _new_p[35], char _password[35] = nullptr) {
  Account tmp = as.user(id);
  if (!(tmp.user_id == id)) {
    error("passwd: account does not exist\n");
  }

  if (curUser().privilege == 7) {
    as.remove_account(id);
    strcpy(tmp.password, _new_p);
    as.insert_account(tmp);
  } else {
    if (strcmp(tmp.password, _password) == 0) {
      as.remove_account(id);
      strcpy(tmp.password, _new_p);
      as.insert_account(tmp);
    } else {
      error("passwd: wrong password\n");
    }
  }

}

void select(const ISBN &isbn) {
  if (curUser().privilege < 3) {
    error("low privilege");
  }
  if (login_list.empty()) { return; }
  auto tmp = login_list.top();
  login_list.pop();
  tmp.second = isbn;
  login_list.push(tmp);
  tmp = login_list.top();
  cout << tmp.first.id << " selects " << tmp.second.id << '\n';
  Book b = bs.bookinfo(isbn);
  if (!(b.isbn == isbn)) {
    bs.insert_book(Book(isbn));
  }
}

void import(int quantity, double tot_cost) {
  if(curUser().privilege<3) error("low privilege\n");
  auto it = bs.list.upper_bound(curBook().isbn);
  // throw an error when no book is selected
  it--;
  BookNode tmp;
  bs.read_main(tmp, (*it).second);
  tmp.modify(login_list.top().second, quantity);
  bs.write_main(tmp, (*it).second);

  FinanceHistory fh{curUser().user_id, curBook().isbn, quantity, tot_cost / quantity, IMPORT};
  fs.add_his(fh);
}

void buy(const ISBN &isbn, int quantity) {
  auto it = bs.list.upper_bound(isbn);
  it--;
  BookNode tmp;
  bs.read_main(tmp, (*it).second);
  tmp.modify(isbn, -quantity);
  bs.write_main(tmp, (*it).second);

  FinanceHistory fh{curUser().user_id, isbn, quantity, bs.bookinfo(isbn).price, SALE};
  fs.add_his(fh);
}

void modify_book(const ISBN &isbn) {
  if(curUser().privilege<3) error("low privilege\n");
  Book sel = bs.remove_book(curBook().isbn);
  // can throw "no book selected"
  sel.isbn = isbn;
  bs.insert_book(sel);
  auto t = login_list.top();
  login_list.pop();
  t.second = isbn;
  login_list.push(t);
}

void modify_book(const Author &author) {
  if(curUser().privilege<3) error("low privilege\n");
  Book sel = bs.remove_book(curBook().isbn);
  sel.author = author;
  bs.insert_book(sel);
}

void modify_book(const BookName &book_name) {
  if(curUser().privilege<3) error("low privilege\n");
  Book sel = bs.remove_book(curBook().isbn);
  sel.name = book_name;
  bs.insert_book(sel);
}

void modify_book(double price_) {
  if(curUser().privilege<3) error("low privilege\n");
  Book sel = bs.remove_book(curBook().isbn);
  sel.price=price_;
  bs.insert_book(sel);
}

void modify_book(const KeyWord &key_word_) {
  if(curUser().privilege<3) error("low privilege\n");
  Book sel = bs.remove_book(curBook().isbn);
  sel.key_word=key_word_;
  bs.insert_book(sel);
}

#endif
