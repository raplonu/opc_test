# OPC test

A simple test to check the OPC UA server and client.

This repository contains a simple server and 2 clients. The server is a simple OPC UA server that exposes a variable that can be read and written. The first client will read and write to the server, while the second client will subscribe to the server and print the values as they change.

Tested features:

- [x] Read
- [x] Write
- [x] Dynamic array support
- [x] Subscribe (aka monitoring)

## Setup

In order to run, you need to have the following installed:

- [g++](https://gcc.gnu.org/)
- [cmake](https://cmake.org/download/)
- [conan](https://conan.io/downloads.html)

> Note: `cmake` and `conan` can be installed using `pip`

## Build

```bash
conan build . -b missing
```

## Run

Run the server:

```bash
build/Release/opc_server
```

Run the clients:

```bash
build/Release/opc_client1
```

```bash
build/Release/opc_client2
```
