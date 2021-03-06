# key-val-store

Install gRPC
------------

```sh
brew install autoconf automake libtool shtool
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update —-init
LIBTOOL=glibtool LIBTOOLIZE=glibtoolize make install
cd third_party/protobuf
sudo make install
```

Build App
---------

```sh
make all
```

Run App
-------

```sh
./kvserver
./kvclient -v 4k -c 10 -u 0.5 -w 10000 -i 10000 -n 1
```

Output
------

* Server will output startup time
* Each client thread will create a .log file with details on number of operations performed and the time taken

Args
----

* -v value size (suffix b/k/m/g)
* -c number of clients
* -u Fraction of operations that has to updates [0, 1]
* -w Numbur of random values to be written to the server before clients are started
* -i Interval (number of ops) for which latency/throughput is recorded
* -n Number of latency/throughput measurements
