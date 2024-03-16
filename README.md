# IRC

Internet Relay Chat Server that can connect with IRC Clients like Irssi.

<br>

## Environment

### Develop Environment
  - <img src="https://img.shields.io/badge/Language-%23121011?style=for-the-badge"><img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"><img src="https://img.shields.io/badge/98-515151?style=for-the-badge">

### Supported OS

  - ![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)

### Recommend IRC Client
  - irssi<a href="https://github.com/irssi/irssi">

<br>

## Installation

    git clone https://github.com/minishell-yecseo/IRC && cd IRC && make -j && make clean

<br>

## Usage

### Run Server

    ./ircserv [port] [password]
<br>
If success, you can see the server information like below <br>

    IRC server:Happy started at 0.0.0.0 :: 9090
<br>
But when you got `no such file or directory: ./ircserv` error message, try to find `IRC` directory and then run `./ircserv [port] [password]` again.

    cd "$(find ~/ -name "IRC" 2>/dev/null | head -n 1)" && ./ircserv [port] [password]

<br>

## Test

We're testing this project with our own test set that contains `Request Test`, `Response Test`, `Command Unit Test`, etc.<br>
If you execute the test binary file made by command line below, The `ircserv_test` will test the source code automatically and let you know about the test results.

### Build & Run Test

    make test -j && ./ircserv_test

<br>

## Structure
### Repository structure

    ┌── Makefile
    ├── Makefile_Test
    ├── Makefile_Variable
    ├── README.md
    ├── Test
    │   ├── main.cpp
    │   └── (...)
    └── source
        ├── main.cpp
        ├── Client.cpp
        ├── Command.cpp
        ├── Mutex.cpp
        ├── NumericDefine.hpp
        ├── Request.cpp
        ├── Response.cpp
        ├── Server.cpp
        ├── ThreadPool.cpp
        ├── Channel.cpp
        ├── Utils.cpp
        ├── log.cpp
        ├── main.cpp
        ├── ircserv.h
        ├── *.hpp (headers)
        └── command
            ├── *Command.cpp
            └── *Command.hpp


## :busts_in_silhouette: Developers

* 김우석(Wooseok Kim) <a href="https://github.com/kimwoo123"><img src="https://img.shields.io/badge/github-181717?style=flat-square&logo=Github&logoColor=white"/></a>
* 서상민(Sangmin Seo) <a href="https://github.com/SeoSangmin"><img src="https://img.shields.io/badge/github-181717?style=flat-square&logo=Github&logoColor=white"/></a>
