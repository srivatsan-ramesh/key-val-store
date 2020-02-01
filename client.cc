#include <string>

#include <grpcpp/grpcpp.h>
#include "key_value.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using keyvalue::KeyValue;
using keyvalue::Entity;
using keyvalue::Result;
using keyvalue::Key;
using keyvalue::Value;

class KeyValueClient {
    public:
        KeyValueClient(std::shared_ptr<Channel> channel) : stub_(KeyValue::NewStub(channel)) {}

    bool set(std::string key, std::string value) {
        Entity request;

        request.set_key(key);
        request.set_value(value);

        Result reply;

        ClientContext context;

        Status status = stub_->set(&context, request, &reply);

        if(status.ok()){
            return reply.result();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    std::string get(std::string key) {
        Key request;

        request.set_key(key);

        Value reply;

        ClientContext context;

        Status status = stub_->get(&context, request, &reply);

        if(status.ok()){
            return reply.value();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "";
        }
    }

    private:
        std::unique_ptr<KeyValue::Stub> stub_;
};

void Run() {
    std::string address("0.0.0.0:5000");
    KeyValueClient client(
        grpc::CreateChannel(
            address, 
            grpc::InsecureChannelCredentials()
        )
    );

    bool response;

    response = client.set("Hello", "World");
    std::cout << "Response for Set " << response << std::endl;

    std::string value = client.get("Hello");
    std::cout << "Response for Get " << value << std::endl;
}

int main(int argc, char* argv[]){
    Run();

    return 0;
}