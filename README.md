# blocking-socket

[![Build Status](https://travis-ci.org/kotarondo/blocking-socket.svg?branch=master)](https://travis-ci.org/kotarondo/blocking-socket)
[![npm version](https://badge.fury.io/js/blocking-socket.svg)](https://badge.fury.io/js/blocking-socket)

Blocking open/close/send/recv calls on unix domain sockets. These calls intentionally *block* the executing context. This module provides only simplest functionality implemented by a Node.js addon.

## Installation

```sh
npm install blocking-socket
```

## Using

```javascript
var bs = require('blocking-socket');
var fd = bs.open('/tmp/socket123');
var sbuf = new Buffer('abcdef');
var transferred = bs.send(fd, sbuf);
var rbuf = new Buffer(4096);
var transferred = bs.recv(fd, rbuf);
var err = bs.close(fd);
```
