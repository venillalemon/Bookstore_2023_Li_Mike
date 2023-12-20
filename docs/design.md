# 总体设计文档

Bookstore-2023 李方可

## 功能设计

### 模块划分

#### 账户管理系统

- 账户数据库的I/O
- 账户登录、注册
- 登录状态记录

#### 书籍管理系统

- 保存书籍信息
- 按信息查询图书
- 图书信息修改（包括购买、进货、基本信息修改）
- 图书信息创建

#### 日志管理系统

- 更新图书采购、销售信息
- 更新图书信息修改（创建）记录
- 更新账户创建记录

## 用户交互设计

### `visitor`

#### 注册

- 输入：`Register <account_name>`
- 输出：`Signed up as <account_type=customer> <account_name>`
  
#### 登录

- 输入：`Login <account_name>`
- 输出：`Logged in Successfully: <account_type> <account_name>`

### `customer`

#### 查询

- 输入：`Find <key> <key>`, 其中可以有多个查找关键字，包括ISBN号、作者名、关键字、书名
- 输出：`<ISBN> <book_name>`。如果不存在，输出`null`

#### 购买

- 输入：`Buy <ISBN> <buy_amount>`

### `staff`

#### 进货

录入：

- 输入：`Input <ISBN> <book_name> <author_name> <key> <amount> <price> <cost>`
  
补货：

- 输入：`Replenish <ISBN> <amount> <cost>`

#### 图书信息修改

`Revise <ISBN>`进入修改模式

- 输入：`<var_name> <value>`，其中库存量不可更改
- 输出：该书籍所有信息 `<ISBN> <book_name> <author_name> <key> <storage> <price>`

`Revise End` 指令退出对`<ISBN>`为号码的书的修改

#### 用户创建

- 输入：`New Account <account_type> <account_name>`
- 输出：`Account Created: <account_type> <account_name>`

### `manager`

#### 采购信息

- 输入：`Purchase History <begin> <end>`
- 输出：按时间顺序输出采购信息（包括录入和进补货），每一行`<time> <staff_name> <ISBN> <amount> <cost>`

#### 销售记录

- 输入：`Sale History <begin> <end>`
- 输出：按时间顺序输出销售信息，每一行`<time> <customer_name> <ISBN> <amount> <price>`

#### 盈利信息

- 输入：`Profit <begin> <end>`
- 输出：`<revenue> <expense> <profit>`

#### 查看员工工作报告

- 输入：`View <staff_name>`
- 输出：按时间顺序输出操作内容，每一行为`<operation_name> <object_name> <object_info>`
- 若没有这个销售人员，输出`error`并让用户（`manager`）重新输入

#### 系统工作日志

- 输入：`Logbook`
- 输出：按时间顺序输出操作内容，每一行为`<operation_type> <operation_name> <object_name> <object_info>`

## 数据库设计

### 书籍信息库

2个二进制文件，内部储存方式为块状链表。

第一个文件index，存储每一块的头数据，并有序储存。index文件的元素为`map<string,int>`，存储块首元素的ISBN号和块在book文件的位置。

第二个文件book，存储每一块存储的具体书籍的数据。book中的元素都是`struct book`结构体，存储书籍所有的数据。

### 用户信息库

2个二进制文件。

文件person，存储用户类型（`staff`,`customer`）、账户名称、操作记录（包括`customer`的购买、`staff`的进货、录入、信息修改、创建用户记录）

文件person_index，存储有序的`account_name`,`account_type`和在person文件中的位置

### 日志文件

2个二进制文件。

文件logbook，按时间存储操作信息。

文件operation，按时间存储操作类别`operation_type`(这是一个`enum`类型的变量)，以便从文件中读取`LogList`的派生类。

## 类、结构体设计

### 对象设计

#### 书籍

类`book`，存储ISBN 号、书名、作者、关键字、库存量、单价。

对书籍的信息修改、销售、进货（即新书录入）、补货，写成`book`类的成员函数。

#### 日志

**基类**
`LogList`，存储内容：
- 时间`time`
- 账户名`account_name`
- 账户类别`account_type`
- 操作类型`operation_type`(这是一个`enum`类型的变量，枚举`{SALE, INPUT, REPLENISH, REVISE, CREATEACCOUNT}`)
- 虚构造函数

**派生类**
由`LogList`派生而来：

- `SaleLog`表示`account_name`进行了购买操作，增加存储ISBN号、数量、售价。
- `InputLog`表示`account_name`进行了录入，增加存储ISBN号、数量、成本价。
- `ReplenishLog`表示`account_name`进行补货，存储ISBN号、数量、成本价。
- `ReviseLog`表示`account_name`对图书信息进行了更改，存储ISBN号、更改的变量和更改后的值。
- `CreateAccountLog`表示`account_name`创建了账户`new_account_name`的信息，存储`new_account_name`和`account_type`。

#### 人员

类`visitor`单独一类，可以通过登录操作转变成其它任意类；剩下的账户类型有继承关系`customer->staff->manager`。设置友元类`book`,`loglist`。

数据成员包含用户名`account_name`、用户类型`account_type`，所有功能写成成员函数，成员函数有权限调用`book`类、`loglist`类的成员函数，能读写图书信息或写入日志信息。
