#ifndef CODE_FINANCE_H
#define CODE_FINANCE_H
#pragma once

#include <iostream>
#include <vector>
#include "books.h"
#include "accounts.h"

const int block_length_finance=250;

enum Type {
  SALE,IMPORT
};

class FinanceHistory {
  ID user_id{};
  ISBN book_id{};
  int amount=0;
  int price=0;
  Type type;
};

class FinanceNode {
  FinanceHistory FH[block_length_finance];
};

class FinanceSys {
  string main_name{};
  fstream file_main;
  int count=0;

  FinanceSys(const string& FN=""){
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      init_main();
    } else {
      file_main.close();
      read_count();
    }
  }

  void init_main() {
    file_main.open(main_name, fstream::out | fstream::binary);
    file_main.write(reinterpret_cast<char*> (&count),sizeof(int));
    file_main.close();
  }

  void read_count() {
    file_main.open(main_name, fstream::in);
    file_main.read(reinterpret_cast<char*> (&count),sizeof(int));
    file_main.close();
  }

};

#endif
