#include <string>
#include <unordered_map>
#include <chrono> 

#include <grpcpp/grpcpp.h>

#include "key_value.grpc.pb.h"
#include "concurrent_trie.h"
#include "storage.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerWriter;
using grpc::ServerContext;
using grpc::Status;

using keyvalue::KeyValue;
using keyvalue::Entity;
using keyvalue::Result;
using keyvalue::Key;
using keyvalue::Value;

ConcurrentTrie<std::string> map;

class KeyValueServiceImpl final : public KeyValue::Service {
    Status set(
        ServerContext* context, 
        const Entity* request, 
        Result* reply
    ) override {
        std::string key = request->key();
        std::string value = request->value();

        map.insert(key, value);

        reply->set_result(true);

        return Status::OK;
    } 

    Status get(
        ServerContext* context, 
        const Key* request, 
        Value* reply
    ) override {
        std::string key = request->key();

        std::string value = map.find(key);

        reply->set_value(value);

        return Status::OK;
    } 

    Status getPrefix(
        ServerContext* context, 
        const Key* request, 
        ServerWriter<Entity>* writer
    ) override {
        std::string key = request->key();

        std::vector<std::pair<std::string, std::string> > result = map.findAll(key);

        for(std::pair<std::string, std::string> kvp : result) {
            Entity entity;
            entity.set_key(kvp.first);
            entity.set_value(kvp.second);
            writer->Write(entity);
        }

        return Status::OK;
    } 
};

void Run() {

    auto t1 = std::chrono::high_resolution_clock::now();

    buildTrieFromFile(map);

    std::string address("0.0.0.0:5000");
    KeyValueServiceImpl service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    builder.SetMaxReceiveMessageSize(16*1024*1024);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout<<"Server listening on port: "<<address<<std::endl;

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout<<"Server took "<<duration/1000.0<<"ms to start\n";

    server->Wait();
}

int main(int argc, char** argv) {
    Run();

    return 0;
}