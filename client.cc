#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include "key_value.grpc.pb.h"

using grpc::Channel;
using grpc::ClientReader;
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

    std::vector<std::pair<std::string, std::string> > getPrefix(std::string key) {
        Key request;

        request.set_key(key);

        Entity entity;

        ClientContext context;

        std::unique_ptr<ClientReader<Entity> > reader(
            stub_->getPrefix(&context, request));
        
        std::vector<std::pair<std::string, std::string> > result;
        while (reader->Read(&entity)) {
            result.push_back(make_pair(entity.key(), entity.value()));
        }
        Status status = reader->Finish();
        if (status.ok()) {
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        return result;
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
        std::vector<std::pair<std::string, std::string> > values;
        switch(c) {
            case 's': std::cin>>key>>value;
                response = client.set(key, value);
                std::cout << response << std::endl;
                break;
            case 'g': std::cin>>key;
                value = client.get(key);
                std::cout << value << std::endl;
                break;
            case 'p': std::cin>>key;
                values = client.getPrefix(key);
                for(std::pair<std::string, std::string> val : values) {
                    std::cout << val.first << " : " << val.second << std::endl;
                }
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
    std::cout<<"p <key>"<<std::endl;
    std::cout<<"e"<<std::endl;
    Run();

    return 0;
}