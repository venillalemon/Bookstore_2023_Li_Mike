#ifndef CODE_LOG_H
#define CODE_LOG_H

#include<cstring>
#include<iostream>
#include<fstream>
#include"books.h"

typedef m_string<500> Log;

using std::string;
using std::fstream;

class LogSys {
public:
  string main_name;
  fstream file_main, file_aux;

  explicit LogSys(const string &FN = "") {
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      file_main.open(main_name, fstream::out);
      file_main.close();
      file_aux.open(main_name + "_employee", fstream::out);
      file_aux.close();
    } else {
      file_main.close();
    }
  }

  ~LogSys() {}

  void write_log(Log &l) {
    file_main.open(main_name, fstream::app);
    file_main.write(reinterpret_cast<char *> (&l), sizeof(Log));
    file_main.close();
  }

  // write an employee log to the end of the file
  void write_employee(Log &l) {
    file_aux.open(main_name + "_employee", fstream::app);
    file_aux.write(reinterpret_cast<char *> (&l), sizeof(Log));
    file_aux.close();
  }

  void show() {
    file_main.open(main_name, fstream::in);
    Log l;
    file_main.read(reinterpret_cast<char *> (&l), sizeof(Log));
    while (!file_main.eof()) {
      std::cout << l << '\n';
      file_main.read(reinterpret_cast<char *> (&l), sizeof(Log));
    }
    file_main.close();
  }

  void show_employee() {
    file_aux.open(main_name + "_employee", fstream::in);
    Log l;
    file_aux.read(reinterpret_cast<char *> (&l), sizeof(Log));
    while (!file_aux.eof()) {
      std::cout << l << '\n';
      file_aux.read(reinterpret_cast<char *> (&l), sizeof(Log));
    }
    file_aux.close();
  }
};

#endif //CODE_LOG_H
