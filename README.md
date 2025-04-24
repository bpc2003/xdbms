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
### Syntax
The basic layout of a command looks like this:\
```cmd s/params/```
- `cmd` represents the command you're trying to execute
- `s` represents the selector.\
If `s` is a number, it will execute the command on that specific object.\
If `s` is an `*` it will execute the command on every object.
- `/params/` represents any parameters\
that the command may need to execute properly.\
All parameters are separated by a `/`.

Examples:
- `GET *` - gets every key-value pair from every object.
- `SET 0/name:John` - sets the key `name` to the value `John` in object 0.
- `DEL */name` - deletes every key `name` from every object.
### API Reference
There are six functions and two types in mdb.h
#### Types:
- tabidx_t: represents a single key-value pair in an object.
- tablist_t: represents a single document.
#### Functions:
- readdb: takes a filename and returns a tablist array\
if the file doesn't exist it will return an empty tablist array\
if readdb fails in any other way it will return NULL.
- writedb: takes a filename and tablist array\
writedb writes the given tablist array to a file.
- getkeys: takes a tablist array, id, list of keys,\
and the length of the list of keys.\
If id is -1, it will get every provided key from every document.\
If keys is NULL, it will get every key-value pair from a document.
- setkeys: takes a tablist array, id, list of key-value pairs,\
and the length of the list of pairs.\
If id is -1, it will set the provided pairs in every document.
- delkeys: takes a tablist array, id, list of keys,\
and the length of the list of keys.\
If id is -1, it will delete every provided key from every document.\
If keys is NULL it will delete every key from a document.

## Removal
In order to remove mdb run:\
```sudo make remove```\
or run:\
```sudo make uninstall```

## License
This software is licensed under the BSD-2-Clause [License](./LICENSE)
