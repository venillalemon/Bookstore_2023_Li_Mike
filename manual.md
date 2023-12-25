# 开发文档

Bookstore-2023 李方可

## 1. 代码文件

### 1.1. 代码文件结构

```bash
bookstore
    ├── README.md
    ├── main.cpp
    ├── src
    |   ├── parser.h
    |   ├── loginstate.h
    │   ├── blocklist.h
    │   ├── books.h
    │   ├── finance.h
    │   ├── accounts.h
    │   ├── log.h
    │   ├── error.h
```

### 1.2. 代码文件说明

- `main.cpp`：主函数，程序入口
- `blocklist.h`：块状链表模板类
- `parser.h`：解析命令行输入的指令
- `loginstate.h`：系统状态
- `books.h`：图书管理
- `accounts.h`：账户管理
- `finance.h`：日志财务管理
- `log.h`：全部日志管理

## 2. 文件关系

`main->src`表示`main.cpp`用到了`src`中的类和函数，即`main.cpp`中有指令`#include "src.h"`。

```bash
main->parser->loginstate
                    ├──> accounts.h
                    ├       ⬇
                    ├──> books.h -> blocklist.h -> error.h
                    ├       ⬆
                    ├──> finance.h 
                    ├──> error.h
                    ├──> log.h
   
```

## 3. 文件说明

### 3.0. `src/error.h`

派生类`ErrorException`，成员函数`error(std::string)`用于抛出错误。

### 3.1. `src/parser.h`

`parser.h`中定义了`parse_Command`函数，用于解析命令行输入的指令，并调用`loginstate.h`中的函数。用到`<regex>`库。如果指令合法，直接将指令写入文件`log`。如果指令不合法，直接调用`error`函数抛出错误。

### 3.2. `src/loginstate.h`

汇总了所有需要做的信息，可以查询、修改`BookSys`对象`bs`，`AccountSys`对象`as`，`FinanceSys`对象`fs`、`LogSys`对象`ls`，登录栈`login_list`。涉及到登陆状态的函数必须写在该文件中，仅仅涉及单个系统的函数又这个文件中的函数负责调用。

### 3.3. `src/blocklist.h`

`blocklist`模板类，有模板参数T。所有书籍信息按照输入顺序存储在文件`books`中，并在附加文件中用块状链表存储类型T(ISBN/书名/作者/关键词)到int(书籍位置)的映射。每次添加书籍，都在文件`books`末尾添加书籍信息，并在附加文件中添加映射。每次修改书籍信息，都直接在文件`books`中修改书籍信息，并且在块状链表中作相应的修改。

### 3.4. `src/books.h`

图书类`books`，包含图书的基本信息，包括图书ISBN号`[ISBN]`、图书名称`[name]`、图书作者`[author]`、图书关键词`[keyword]`、图书价格`[price]`、图书数量`[quantity]`。

BlockList类`ib_list, bn_list, au_list, kw_list`，分别用于存储图书ISBN号和int/书名和int/作者名字和int/关键字和int的对应关系。

文件`book`中按输入顺序储存图书信息。类`BookSys`有成员函数`Book bookinfo(const ISBN &isbn)`，按ISBN/书名/作者/关键词搜索的重载函数`void find_book(...)`，更改图书信息`void modify_book(...)`，可供更改信息时调用的私有函数`void insert_key_word(),void remove_from_kw()`。更改`map kw_list`时，需要调用`void insert_key_word(),void remove_from_kw()`。更改`map bn_list, au_list, ib_list`，只需要调用相应模板类的`insert_pair, remove_pair`函数。

### 3.5. `src/accounts.h`

账户类`accounts`，包含账户的基本信息，包括账户ID`[ID]`、账户密码`[passwd]`、账户名称`[name]`、账户权限`[privilege]`。使用块状链表存储。

### 3.6. `src/finance.h`

记录开支和收入，顺序存储即可。示例：

```cpp
worker	9787544287722	15	10.00	IMPORT
user0	9787559410443	5	38.00	SALE
user0	9787544287722	3	45.00	SALE
worker	9787559410443	20	38.00	SALE
worker	9787559410443	20	10.00	IMPORT
worker	9787544287722	3	10.00	IMPORT
worker	9787544287722	5	45.00	SALE
root	9787544287722	1	45.00	SALE
root	9787544287722	20	10.00	IMPORT
```

### 3.7. `src/log.h`

按顺序存储操作信息字符串。字符串示例：

```cpp
root adds worker with privilege 3
root selects 9787544287722
root modifies the keyword of 9787544287722 to KlinePLC|GoldenInc 
worker selects 9787544287722
worker imports 15 9787544287722 for total cost 150.000000
user0 registered
user0 buys 5 9787559410443 for 38.000000 each
```

## 4. `main`函数设计

首先以全局变量创建`BookSys`对象`bs`，`AccountSys`对象`as`，`FinanceSys`对象`fs`，`LogSys`对象`ls`，登录栈`login_list`，初始化这些数据。

然后进入主循环，每次循环开始时，读入指令，如果指令非法直接报错。根据指令调用相应的函数，这些函数包含在`operation.h`中，参数为`bs`,`as`,`fs`。注意这些函数可以判断用户是否有进行操作的权限，如果有权限，那么调用`bs`,`as`,`fs`的成员函数，也即对它们进行修改；如果没有权限或者任何未定义行为，直接认为操作失败。直到读入`exit`或`quit`指令或读入文件`eof`，正常退出程序。

## 5. 文件存储说明

### 4.1. `template<class T, int block> class blocklist`

存储T->int的映射，例如`pair<T,int>(t,i)`表示主存书籍的book文件中，类型T的值为t的一本书籍的位置在i处，以便直接在book文件读取。

每个块大小为`block`，每个块中存储`block`个`pair<T, int>`。存储在main文件中。链表结构用`std::map`实现，每个`map`的`key`为`T`，`value`为`int`，`pair<T,int>(t,i)`表示在块状链表中，位于位置i的块的起始书籍的key=t。`map`的`key`按照字典序排列。

这样，每个块状链表都需要两个文件存储。例如，`bn_list`需要`book_bn`和`book_bn_aux`两个文件。`book_bn`文件存储块状链表的内容，`book_bn_aux`存储`map`的内容。

### 4.2. `finance`和`log`开头的文件

由于无需查找，只需顺序输出，这些文件按照顺序存储，每次添加新的信息都在文件末尾添加。例如，`finance`文件存储开支和收入信息，`log`文件存储操作信息。

其中，`finance`文件需要一个辅助文件，以快速查询一段时间内的收支信息。辅助文件`finance_aux`存储`std::vector<std::pair<double, double>> v`，表示到第i条记录为止，收入为`v[i].first`，支出为`v[i].second`。
