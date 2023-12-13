#ifndef CODE_STATEMENTS_H
#define CODE_STATEMENTS_H

#include <iostream>

using std::string;

class Base {
public:
  Base()=default;
  virtual void e()=0;
};

class Login:public Base {
  Login(string str) {

  }
};

#endif //CODE_STATEMENTS_H
