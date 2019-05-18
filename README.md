## About

This is an simple game inspired by Crazy Arcade 

Using C++ and [ege graphics library](https://xege.org/)

## The Code

Main source code can be found in *src/* folder

The *client.exe* and *server.exe* will be in *bin/* folder after make

## Control

* `w` move up
* `a` move left
* `s` move down
* `d` move right
* `space` lay the bomb

## Usage

### Offline Mode

Just run the *server.exe* and type `L` in the login page

![](https://raw.githubusercontent.com/hujunhan/cloudimage/master/img/paopaotang-server-main.jpg)

![](https://raw.githubusercontent.com/hujunhan/cloudimage/master/img/ppt-server-main.jpg)

### Online Mode

1. First run *server.exe* in computer A in a local network and have an IPv4 address *\$aa.bb.cc.dd\$*, and type `z` to activate online mode, start listening

2. Run *client.exe* in computer B in the same local network with A.

   type the server’s address (in this case \$aa.bb.cc.dd\$) in the command line 

   ![](https://raw.githubusercontent.com/hujunhan/cloudimage/master/img/ppt-client.png)

3. Start fighting with your friend

