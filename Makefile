LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -lgrpc++_reflection -lpthread \
           -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: client server

client: key_value.pb.o key_value.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

server: concurrent_trie.o key_value.pb.o key_value.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h client server