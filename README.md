# mdb
mdb is a simple embedded key-value database written in C.

## Table of Contents
- [Installation](#Installation)
- [Reference](#Reference)
- [Removal](#Removal)
- [License](#License)

## Installation
In order to install mdb clone this repository then run:\
```sudo make install```

## Reference
There are four basic commands in mdb:
- exit: exits the program
- GET: gets a given key-value pair from a given object
- SET: sets a given key-value pair in a given object
- DEL: deletes a given key-value pair from a given object
#### Syntax
The basic layout of a command looks like this:\
```cmd s/params/```
- `cmd` represents the command you're trying to execute
- `s` represents the selector.\
If `s` is a number, it will execute the command on that specific object.\
If `s` is an `*` it will execute the command on every object.
- `/params/` represents any paramaters\
that the command may need to execute properly.\
All parameters are separated by a `/`.

Examples:
- `GET *` - gets every key-value pair from every object.
- `SET 0/name:John` - sets the key `name` to the value `John` in object 0.
- `DEL */name` - deletes every key `name` from every object.
### API Reference
There are six functions in mdb.h
- readdb: reads a database from a given file
- writedb: writes a database to a given file
- getkeys: gets every key-value pair from a given object
- getkey: gets a single key-value pair from a given object
- setkey: sets a given key-value pair in a given object
- delkey: deletes a given key-value pair from a given object

## Removal
In order to remove mdb run:\
```sudo make remove```\
or run:\
```sudo make uninstall```

## License
This software is licensed under the BSD-2-Clause [License]("./LICENSE")
