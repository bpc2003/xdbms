# mdb
mdb is a simple embedded key-value database written in C.

## Table of Contents
- [Installation](#Installation)
- [Reference](#Reference)
- [Removal](#Removal)
- [License](./LICENSE)

## Installation
In order to install mdb clone this repository then run:
```sudo make install```

## Reference
There are four basic commands in mdb:
- exit: exits the program
- GET: gets a given key from a given object
- SET: sets a given key-value pair in a given object
- DEL: deletes a given key-value pair from a given object
### API Reference
There are six functions in mdb.h
- readdb: reads a database from a given file
- writedb: writes a database to a given file
- getkeys: gets every key-value pair from a given object
- getkey: gets a single key-value pair from a given object
- setkey: sets a given key-value pair in a given object
- delkey: deletes a given key-value pair from a given object

## Removal
In order to remove mdb run:
```sudo make remove```
or
```sudo make uninstall```
