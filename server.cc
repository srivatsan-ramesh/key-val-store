#include <string>
#include <unordered_map>

#include <grpcpp/grpcpp.h>
#include "key_value.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using keyvalue::KeyValue;
using keyvalue::Entity;
using keyvalue::Result;
using keyvalue::Key;
using keyvalue::Value;

std::unordered_map<std::string, std::string> map;

class MathServiceImplementation final : public KeyValue::Service {
    Status set(
        ServerContext* context, 
        const Entity* request, 
        Result* reply
    ) override {
        std::string key = request->key();
        std::string value = request->value();

        map[key] = value;

        reply->set_result(true);

        return Status::OK;
    } 

    Status get(
        ServerContext* context, 
        const Key* request, 
        Value* reply
    ) override {
        std::string key = request->key();

        std::string value = "";

        if(map.find(key) != map.end())
            value = map[key];

        reply->set_value(value);

        return Status::OK;
    } 
};

void Run() {
    std::string address("0.0.0.0:5000");
    MathServiceImplementation service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    Run();

    return 0;
}