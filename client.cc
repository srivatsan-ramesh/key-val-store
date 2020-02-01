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
    while(1) {
        char c;
        std::cin>>c;
        std::string key, value;
        switch(c) {
            case 's': std::cin>>key>>value;
                response = client.set(key, value);
                std::cout << response << std::endl;
                break;
            case 'g': std::cin>>key;
                value = client.get(key);
                std::cout << value << std::endl;
                break;
            case 'e':
                exit(0);
        }
    }
}

int main(int argc, char* argv[]){
    std::cout<<"Commands to set, get and exit"<<std::endl;
    std::cout<<"s <key> <value>"<<std::endl;
    std::cout<<"g <key>"<<std::endl;
    std::cout<<"e"<<std::endl;
    Run();

    return 0;
}