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
  if (login_list.empty()) return {};
  ID i = login_list.top().first;
  return as.user(i);
}

Book curBook() {
  if (login_list.empty()) return {};
  ISBN i = login_list.top().second;
  if (i == ISBN()) error("import/buy: no book selected");
  return bs.bookinfo(i);
}

void login(const ID &id, const char password[35] = nullptr) {
  if (curUser().privilege != 7) {
    if (password == nullptr) {
      error("require password");
      return;
    }
    Account i = as.user(id);
    if (!(i.user_id == id)) {
      error("account not found to log in");
      return;
    }
    if (strcmp(i.password, password) == 0)
      login_list.emplace(id, ISBN());
    else error("wrong password\n");
  } else {
    Account i = as.user(id);
    if (!(i.user_id == id)) {
      error("account not found to log in\n");
      return;
    }
    login_list.emplace(id, ISBN());
  }
  if (!login_list.empty()) {
    auto tmp = login_list.top();
    cout << tmp.first.id << " " << tmp.second.id << '\n';
  }
}

pair<ID, ISBN> logout() {
  if (login_list.empty()) {
    error("no user logged in\n");
    return pair<ID, ISBN>{};
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
    return;
  }

  if (curUser().privilege == 7) {
    as.remove_account(id);
    strcpy(tmp.password, _password);
    as.insert_account(tmp);
  } else {
    if (strcmp(tmp.password, _password) == 0) {
      as.remove_account(id);
      strcpy(tmp.password, _password);
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
  auto it = bs.list.upper_bound(login_list.top().second);
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
  tmp.modify(login_list.top().second, -quantity);
  bs.write_main(tmp, (*it).second);

  FinanceHistory fh{curUser().user_id, curBook().isbn, quantity, curBook().price, SALE};
  fs.add_his(fh);
}

void modify_book(const ISBN &isbn) {
  Book sel = bs.remove_book(curBook().isbn);
  sel.isbn = isbn;
  bs.insert_book(sel);
  auto t = login_list.top();
  login_list.pop();
  t.second = isbn;
  login_list.push(t);
}

void modify_book(const Author &author) {
  Book sel = bs.remove_book(curBook().isbn);
  sel.author = author;
  bs.insert_book(sel);
}

void modify_book(const BookName &book_name) {
  Book sel = bs.remove_book(curBook().isbn);
  sel.name = book_name;
  bs.insert_book(sel);
}

void modify_book(double price_) {
  Book sel = bs.remove_book(curBook().isbn);
  sel.price=price_;
  bs.insert_book(sel);
}

void modify_book(const KeyWord &key_word_) {
  Book sel = bs.remove_book(curBook().isbn);
  sel.key_word=key_word_;
  bs.insert_book(sel);
}



#endif