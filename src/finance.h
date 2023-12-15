#ifndef CODE_FINANCE_H
#define CODE_FINANCE_H
#pragma once

#include <iostream>
#include <vector>
#include "books.h"
#include "accounts.h"

using std::vector;

const int block_length_finance = 250;

enum Type {
  SALE, IMPORT
};

class FinanceHistory {
public:
  ID user_id{};
  ISBN book_id{};
  int amount = 0;
  double price = 0;// price per book
  Type type;
};

class FinanceSys {
public:
  string main_name{};
  fstream file_main, file_aux;
  int count = 0;// in main
  vector<pair<double, double>> v;// in aux, sale/cost

  explicit FinanceSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    file_aux.open(main_name + "_aux", std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_aux.close();
      init_main();
      init_v();
    } else {
      file_main.close();
      file_aux.close();
      read_count();
      read_v();
    }
  }

  ~FinanceSys() {
    write_count();
    write_v();
  }

  void init_main() {
    file_main.open(main_name, fstream::out | fstream::binary);
    file_main.write(reinterpret_cast<char *> (&count), sizeof(int));
    file_main.close();
  }

  void init_v() {
    file_aux.open(main_name + "_aux", fstream::out | fstream::binary);
    file_aux.close();
  }

  void read_count() {
    file_main.open(main_name, fstream::in);
    file_main.read(reinterpret_cast<char *> (&count), sizeof(int));
    file_main.close();
  }

  void read_v() {
    file_aux.open(main_name + "_aux", fstream::in);
    pair<double, double> tmp;
    while (!file_aux.eof()) {
      file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<double, double>));
      v.push_back(tmp);
    }
    file_aux.close();
  }

  void write_count() {
    file_main.open(main_name, fstream::in | fstream::out);
    file_main.write(reinterpret_cast<char *> (&count), sizeof(int));
    file_main.close();
  }

  void write_v() {
    file_aux.open(main_name + "_aux", fstream::in | fstream::out);
    for (auto i: v) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<double, double>));
    }
    file_aux.close();
  }

  void add_his(FinanceHistory &FH) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&FH), sizeof(FinanceHistory));
    file_main.close();
    ++count;
    pair<double, double> last = v.back();
    if (FH.type == SALE) last.first += FH.price * FH.amount;
    else if (FH.type == IMPORT) last.second += FH.price * FH.amount;
    v.push_back(last);
  }

  pair<double,double> get_finance(){
    return v.back();
  }

  pair<double,double> get_finance(int cnt){
    if(cnt>v.size()) {
      error("overflow\n");
      return {};
    } else if (cnt==count) {
      return get_finance();
    }
    pair<double,double> st=v[v.size()-cnt-1],ed=v.back();
    return {ed.first-st.first,ed.second-st.second};
  }

};

#endif
