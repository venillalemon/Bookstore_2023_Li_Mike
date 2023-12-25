#ifndef CODE_LOGINSTATE_H
#define CODE_LOGINSTATE_H
#pragma once

#include "accounts.h"
#include "books.h"
#include "finance.h"
#include "error.h"
#include "log.h"


extern AccountSys as;
extern BookSys bs;
extern FinanceSys fs;
extern LogSys ls;
extern vector<pair<ID, ISBN>> login_list;

//get the current user
Account curUser() {
  if (login_list.empty()) error("visitor mode\n");
  ID i = login_list.back().first;
  return as.user(i);
}

Book curBook() {
  if (login_list.empty()) error("import/buy: no user logged in\n");
  ISBN i = login_list.back().second;
  if (i == ISBN()) error("import/buy: no book selected\n");
  return bs.bookinfo(i);
}

int curPrivilege() {
  if (login_list.empty()) return 0;
  return curUser().privilege;
}

void print() {
  cout << "==============as\n";
  as.print();
  cout << "==============bs\n";
  bs.print();
  cout << "==============fs\n";
  fs.print();
  cout << "==============login\n";
  for (auto i: login_list) {
    cout << i.first.id << i.second << '\n';
  }
  cout << "=================end\n";
}

void reg(const ID &id, const char password[35], const char username[35]) {
  as.insert_account(Account(id, password, username, 1));
  char log[500]="";
  strcat(log, id.id);
  strcat(log, " registered");
  ls.write_log((Log &) (log));
}

void useradd(const ID &id, const char password[35], const char username[35], int privilege) {
  if (curPrivilege() <= privilege || curPrivilege() < 3) {
    error("useradd: low privilege\n");
  }
  as.insert_account(Account(id, password, username, privilege));

  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " adds ");
  strcat(log, id.id);
  strcat(log, " with privilege ");
  strcat(log, std::to_string(privilege).c_str());
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void login(const ID &id, const char password[35] = nullptr) {
  Account i = as.user(id);
  if (curPrivilege() <= i.privilege) {
    if (password == nullptr) {
      error("login: require password\n");
    }

    if (!(i.user_id == id)) {
      error("login: account not found to log in\n");
    }
    if (strcmp(i.password, password) == 0) {
      pair<ID, ISBN> m{};
      m.first = id;
      login_list.push_back(m);
    } else error("wrong password\n");
  } else {
    if(password!= nullptr){
      if(strcmp(i.password, password) != 0) error("wrong password\n");
    }
    if (!(i.user_id == id)) {
      error("login: account not found to log in\n");
    }
    login_list.emplace_back(id, ISBN());
  }
}

pair<ID, ISBN> logout() {
  if (login_list.empty()) {
    error("logout: no user logged in\n");
  }
  auto tmp = login_list.back();
  login_list.pop_back();
  //cout << tmp.first.id << " " << tmp.second.id << '\n';
  return tmp;
}

void resetpasswd(const ID &id, const char _new_p[35], const char _password[35] = nullptr) {
  if(curPrivilege() < 1) error("resetpasswd: low privilege\n");
  Account tmp = as.user(id);
  if (!(tmp.user_id == id)) {
    error("passwd: account does not exist\n");
  }
  char log[500]="";
  if (curUser().privilege == 7) {
    if(_password!= nullptr) {
      if(strcmp(_password, tmp.password) != 0) error("passwd: wrong password\n");
    }
    as.resetpasswd(id, _new_p);
    strcat(log, curUser().user_id.id);
    strcat(log, " resets the password of ");
    strcat(log, id.id);
    ls.write_log((Log &) (log));
  } else {
    if (strcmp(tmp.password, _password) == 0) {
      as.resetpasswd(id, _new_p);
      strcat(log, curUser().user_id.id);
      strcat(log, " resets the password of ");
      strcat(log, id.id);
      ls.write_log((Log &) (log));
    } else {
      error("passwd: wrong password\n");
    }
  }

}

void select(const ISBN &isbn) {
  if (curPrivilege() < 3) {
    error("select: low privilege\n");
  }
  login_list.back().second = isbn;
  //cout << tmp.first.id << " selects " << tmp.second.id << '\n';
  auto it = bs.bookinfo(isbn);
  if (it.isbn != isbn) {
    bs.insert_book(Book(isbn));
  }
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " selects ");
  strcat(log, isbn.id);
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void import(int quantity, double tot_cost) {
  if (curPrivilege() < 3) error("import: low privilege\n");
  if (quantity <= 0) error("import: invalid quantity\n");
  if (tot_cost <= 0) error("import: invalid cost\n");


  bs.import_book(curBook().isbn, quantity);
  // throw an error when no book is selected

  FinanceHistory fh{curUser().user_id, curBook().isbn, quantity, tot_cost / quantity, IMPORT};
  fs.add_his(fh);

  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " imports ");
  strcat(log, std::to_string(quantity).c_str());
  strcat(log, " ");
  strcat(log, curBook().isbn.id);
  strcat(log, " for total cost ");
  strcat(log, std::to_string(tot_cost).c_str());
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void buy(const ISBN &isbn, int quantity) {
  if (curPrivilege() < 1) error("buy: low privilege\n");
  if (quantity <= 0) error("buy: invalid quantity\n");

  double tot_p=bs.buy_book(isbn,quantity);

  ID id = curUser().user_id;
  FinanceHistory fh{id, isbn, quantity, tot_p/quantity, SALE};
  fs.add_his(fh);
  cout << std::fixed << std::setprecision(2) << tot_p << '\n';
  cout << std::defaultfloat;
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " buys ");
  strcat(log, std::to_string(quantity).c_str());
  strcat(log, " ");
  strcat(log, isbn.id);
  strcat(log, " for ");
  strcat(log, std::to_string(tot_p/quantity).c_str());
  strcat(log, " each");
  ls.write_log((Log &) log);
}

void modify_book(ISBN &isbn) {
  if (curPrivilege() < 3) error("modify book: low privilege\n");
  ISBN del_isbn = curBook().isbn;// can throw "no book selected"
  if (del_isbn == isbn) { error("modify book: ISBN remains the same\n"); }
  bs.modify_book(del_isbn, isbn);
  for (auto &i: login_list) {
    if (i.second == del_isbn) i.second = isbn;
  }
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " modifies the ISBN of ");
  strcat(log, del_isbn.id);
  strcat(log, " to ");
  strcat(log, isbn.id);

  strcat(log, " ");
  ls.write_log((Log &) (log));
}

void modify_book(Author &author) {
  if (curPrivilege() < 3) error("modify book: low privilege\n");
  bs.modify_book(curBook().isbn, author);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " modifies the author of ");
  strcat(log, curBook().isbn.id);
  strcat(log, " to ");
  strcat(log, author.id);
  strcat(log, " ");
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void modify_book(BookName &book_name) {
  if (curPrivilege() < 3) error("modify book: low privilege\n");
  bs.modify_book(curBook().isbn, book_name);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " modifies the name of ");
  strcat(log, curBook().isbn.id);
  strcat(log, " to ");
  strcat(log, book_name.id);
  strcat(log, " ");
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void modify_book(double price_) {
  if (curPrivilege() < 3) error("modify book: low privilege\n");
  bs.modify_book(curBook().isbn, price_);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " modifies the price of ");
  strcat(log, curBook().isbn.id);
  strcat(log, " to ");
  strcat(log, std::to_string(price_).c_str());
  strcat(log," ");
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void modify_book(const KeyWord &key_word_) {
  if (curPrivilege() < 3) error("modify book: low privilege\n");
  bs.modify_book(curBook().isbn, key_word_);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " modifies the keyword of ");
  strcat(log, curBook().isbn.id);
  strcat(log, " to ");
  strcat(log, key_word_.id);
  strcat(log, " ");
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void delete_account(const ID &id) {
  if (curPrivilege() < 7) error("delete: low privilege\n");
  for (auto i: login_list) {
    if (i.first == id) {
      error("delete: cannot delete current user\n");
    }
  }
  as.remove_account(id);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " deletes the user ");
  strcat(log, id.id);
  ls.write_log((Log &) (log));
  ls.write_employee((Log &) (log));
}

void find_book(const ISBN &isbn) {
  if (curPrivilege() < 1) error("show: low privilege\n");
  bs.find_book(isbn);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " shows -ISBN=");
  strcat(log, isbn.id);
  ls.write_log((Log &) (log));
}

void find_book(const BookName &isbn) {
  if (curPrivilege() < 1) error("show: low privilege\n");
  bs.find_book(isbn);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " shows -name=");
  strcat(log, isbn.id);
  ls.write_log((Log &) (log));
}

void find_book(const Author &isbn) {
  if (curPrivilege() < 1) error("show: low privilege\n");
  bs.find_book(isbn);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " shows -author=");
  strcat(log, isbn.id);
  ls.write_log((Log &) (log));
}

void find_book(const KeyWord &isbn) {
  if (curPrivilege() < 1) error("show: low privilege\n");
  bs.find_book(isbn);
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " shows -keyword=");
  strcat(log, isbn.id);
  ls.write_log((Log &) (log));
}

void show_finance(int count = -1) {
  if (curPrivilege() < 7) error("show finance: low privilege\n");
  pair<double, double> ans;
  if (count == -1) { ans = fs.get_finance(); }
  else if (count == 0) {
    cout << '\n';
    return;
  } else ans = fs.get_finance(count);
  cout << std::fixed << std::setprecision(2) << "+ " << ans.first << " - " << ans.second << '\n';
  cout << std::defaultfloat;
}

void show_all(){
  bs.show();
  char log[500]="";
  strcat(log, curUser().user_id.id);
  strcat(log, " shows all the books");
  ls.write_log((Log &) (log));
}

void show_log(){
  if(curPrivilege() < 7) error("show log: low privilege\n");
  ls.show();
}

void report_employee(){
  if(curPrivilege() < 7) error("report employee: low privilege\n");
  ls.show_employee();
}

#endif
