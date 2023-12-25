#ifndef CODE_BLOCKLIST_H
#define CODE_BLOCKLIST_H
#pragma once

#include "error.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <climits>


using std::map;
using std::fstream;
using std::string;
using std::vector;
using std::pair;

// encapsulated blocklist template
// in the template, T is the type of the key, and block is the size of the block
// key can repeat, but the pair of key and pos (i.e., K) has to be unique
// T should include K
template<class T, int block> // K-ordered
class BlockList {
public:

  class TV;

  class Node;

  fstream file_main, file_aux;
  string main_name;
  int block_num = 0;
  map<TV, int> list;// T to pos of the node in file_main


  class TV {
  public:
    T key{};
    int pos = 0;

    TV() = default;

    TV(T _key, int c) {
      key = _key;
      pos = c;
    }

    bool operator==(const TV &rhs) const {
      return (key == rhs.key) && (pos == rhs.pos);
    }

    bool operator<(const TV &rhs) const {
      if (key == rhs.key) return pos < rhs.pos;
      return key < rhs.key;
    }

    TV &operator=(TV rhs) {
      key = rhs.key;
      pos = rhs.pos;
      return *this;
    }


  };


  class Node {
  public:

    TV first = TV();
    int size = 0;
    int pos = 0;
    TV data[block];// data

    Node() = default;

    Node(TV _first, int _pos) {
      first = _first;
      pos = _pos;
    }

    void insert(TV &tv) {
      bool inserted = false;
      for (int i = size; i > 0; i--) {
        if (tv < data[i - 1]) data[i] = data[i - 1];
        else if (data[i - 1] < tv) {
          data[i] = tv;
          inserted = true;
          break;
        } else {
          error("TV pair exists\n");
          return;
        }
      }
      if (!inserted) data[0] = tv;
      size++;
      first = data[0];
    }

    void remove(TV &tv) {
      bool found = false;
      for (int i = 0; i < size; i++) {
        if (!found && data[i] == tv) found = true;
        if (found) data[i] = data[i + 1];
      }
      if (found) size--;
      else error("TV pair does not exist\n");
      first = data[0];
    }

    vector<int> get_data(T _key, bool &exist) const {
      vector<int> v;
      exist = false;
      for (int i = 0; i < size; i++) {
        if (data[i].first == _key) {
          exist = true;
          v.push_back(data[i].second);
        }
      }
      return v;
    }

    void print() {
      std::cout << first.key << " " << first.pos << " " << size << " " << pos << ":\n";
      for (int i = 0; i < size; ++i) {
        std::cout << "   " << data[i].key << "   " << data[i].pos << '\n';
      }
    }

  };

  BlockList() = default;


  void init(const string &FN = ""){
    if (!FN.empty()) main_name = FN;
    file_main.open(main_name, std::ios::in);
    if (!file_main.is_open()) {
      file_main.close();
      init_list();
    } else {
      file_main.close();
      file_aux.close();
      read_list();
    }
  }

  ~BlockList() {
    write_list();
  }

  void init_list() {
    file_main.open(main_name, fstream::out | fstream::binary);
    file_main.close();
    list.clear();
    file_aux.open(main_name + "_aux", fstream::out | fstream::binary);
    file_aux.close();
  }

  void read_list() {
    list.clear();
    file_aux.open(main_name + "_aux", fstream::in | fstream::binary);
    file_aux.read(reinterpret_cast<char *>(&block_num), sizeof(int));
    pair<TV, int> tmp;
    file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<TV, int>));
    while (!file_aux.eof()) {
      list.insert(tmp);
      file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(pair<TV, int>));
    }
  }

  void write_list() {
    file_aux.open(main_name + "_aux", fstream::out | fstream::binary);
    file_aux.write(reinterpret_cast<char *>(&block_num), sizeof(int));
    for (auto i: list) {
      file_aux.write(reinterpret_cast<char *>(&i), sizeof(pair<TV, int>));
    }
    file_aux.close();
  }

  void append_main(Node &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(Node));
    file_main.close();
    ++block_num;
  }

  void write_main(Node &t, const int pos) {
    if (pos > block_num) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp((pos - 1) * sizeof(Node));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }

  void read_main(Node &t, const int pos) {
    if (pos > block_num) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg((pos - 1) * sizeof(Node));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }

  vector<int> get_data(const T &_key) {
    vector<int> v;
    auto ed = list.upper_bound(TV{_key, INT_MAX});
    if (ed == list.begin()) return v;
    auto st=list.lower_bound(TV{_key, INT_MIN});
    if(st!=list.begin()) st--;
    Node tmp;
    for(auto it=st;it!=ed;it++){
      read_main(tmp,it->second);
      for(int i=0;i<tmp.size;i++){
        if(tmp.data[i].key==_key) v.push_back(tmp.data[i].pos);
      }
    }
    return v;
  }

  int find(T& key,int p){
    TV tv{key,p};
    auto it = list.upper_bound(tv);
    if (it == list.begin()) return 0;
    it--;
    Node tmp;
    read_main(tmp, it->second);
    int l=0,r=tmp.size,mid=0;
    while(l+1<r){
      mid=(l+r)>>1;
      if(tv>tmp.data[mid]) r=mid;
      else l=mid;
    }
    if(tmp.data[l]==tv) return tmp.data[l].pos;
    else return 0;
  }

  void insert_pair(T &t, int p) {
    TV tv{t, p};
    //std::cout<<tv.key<<tv.pos;
    if (list.empty()) {
      //std::cout << "empty\n";
      Node tmp(tv, block_num+1);
      tmp.data[0] = tv;
      tmp.size++;
      append_main(tmp);
      list.insert(pair<TV, int>(tv, block_num));
      return;
    }
    auto it = list.upper_bound(tv);
    Node tmp{};
    if (it != list.begin()) { it--; }
    read_main(tmp, it->second);
    list.erase(it);
    tmp.insert(tv);
    list.insert({tmp.first, tmp.pos});
    if (tmp.size > block - 3) {
      Node new_node(tmp.data[tmp.size / 2], block_num + 1);
      new_node.size = tmp.size - tmp.size / 2;
      for (int i = tmp.size / 2; i < tmp.size; i++) {
        new_node.data[i - tmp.size / 2] = tmp.data[i];
      }
      tmp.size /= 2;
      list.insert(pair<TV, int>(new_node.first, block_num + 1));
      //std::cout << new_node.first.key << new_node.first.pos << block_num + 1 << '\n';
      append_main(new_node);
    }
    write_main(tmp, it->second);
  }

  void remove_pair(T &t, int p) {
    TV tv{t, p};
    auto it = list.upper_bound(tv);
    if (it == list.begin()) return;
    Node tmp;
    it--;
    int pos = it->second;
    read_main(tmp, pos);
    TV key = tmp.first;
    tmp.remove(tv);
    list.erase(key);
    write_main(tmp, pos);
    if (tmp.size != 0) list.insert(pair<TV, int>(tmp.first, pos));
  }

  void print() {
    Node mtp;
    for (auto i: list) {
      //std::cout << i.first.key <<" "<< i.first.pos << " " << i.second << '\n';
      read_main(mtp, i.second);
      mtp.print();
    }
  }

  vector<int> all_books(){
    vector<int> v;
    Node tmp;
    for(auto i:list){
      read_main(tmp,i.second);
      for(int j=0;j<tmp.size;j++){
        v.push_back(tmp.data[j].pos);
      }
    }
    return v;
  }

};

#endif //CODE_BLOCKLIST_H
