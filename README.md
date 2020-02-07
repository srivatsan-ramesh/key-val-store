# key-val-store

Install gRPC
------------

```sh
brew install autoconf automake libtool shtool
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update — init
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
./server
./client
```
