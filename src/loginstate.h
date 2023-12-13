#ifndef CODE_LOGINSTATE_H
#define CODE_LOGINSTATE_H

#include "accounts.h"
#include "books.h"
#include "error.h"

stack<pair<ID, ISBN>> login_list;
AccountSys as("account");
BookSys bs("book");

//get the current user
Account curUser() {
  if (login_list.empty()) return {};
  ID i = login_list.top().first;
  return as.user(i);
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
  return tmp;
}

void select (const ISBN &isbn) {
  auto tmp=login_list.top();
  login_list.pop();
  tmp.second=isbn;
  login_list.push(tmp);
  tmp=login_list.top();
  cout<<tmp.first.id<<" selects "<<tmp.second.id<<'\n';
  Book b=bs.bookinfo(isbn);
  if(!(b.isbn==isbn)) {
    bs.insert_book(Book(isbn));
  }
}

void import(int quantity, int tot_cost) {
  auto it = bs.list.upper_bound(login_list.top().second);
  it--;
  BookNode tmp;
  bs.read_main(tmp, (*it).second);
  tmp.modify(login_list.top().second, quantity);
  bs.write_main(tmp, (*it).second);
}

#endif