#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono> 
#include <iostream>
#include <fstream>

#include <grpcpp/grpcpp.h>
#include <pthread.h>
#include "key_value.grpc.pb.h"
#include "random.h"

#define SERVER_ADDRESS "0.0.0.0:5000"
#define keySize 128

using grpc::Channel;
using grpc::ClientReader;
using grpc::ClientContext;
using grpc::Status;

using keyvalue::KeyValue;
using keyvalue::Entity;
using keyvalue::Result;
using keyvalue::Key;
using keyvalue::Value;

pthread_barrier_t mybarrier;

class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

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
            return false;
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
    std::string address(SERVER_ADDRESS);
    KeyValueClient client(
        grpc::CreateChannel(
            address, 
            grpc::InsecureChannelCredentials()
        )
    );

    bool response;
    while(1) {
        std::cout<<"Commands to set = s, get = g, getPrefix = p and exit = e"<<std::endl;
        std::cout<<"s <key> <value>"<<std::endl;
        std::cout<<"g <key>"<<std::endl;
        std::cout<<"p <key>"<<std::endl;
        std::cout<<"e"<<std::endl;
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

struct ClientArgs {
    int cid;
    int vsize;
    int iniWrites;
    double updateRatio;
    int measInterval;
    int noOfIntervals;
    bool printStats;

    ClientArgs() {}

    ClientArgs(int cid, int vsize, int iniWrites, double updateRatio, int measInterval, 
    int noOfIntervals, bool printStats) : cid(cid), vsize(vsize), iniWrites(iniWrites), 
    updateRatio(updateRatio), measInterval(measInterval), noOfIntervals(noOfIntervals), printStats(printStats) {}

    std::string toString() {
        return std::to_string(cid) + "_" + std::to_string(vsize) + "_" + 
        std::to_string(iniWrites) + "_" + std::to_string(updateRatio) + "_" + 
        std::to_string(measInterval) + "_" + std::to_string(noOfIntervals);
    }
};

void *startClient(void *args) {
    ClientArgs *ca = (ClientArgs *)args;
    std::ofstream measureFile;
    if(ca->printStats) {
        measureFile.open(ca->toString() + ".log");

        if(!measureFile.is_open()) {
            std::cout<<"Problem with client "<<ca->cid<<'\n';
            return NULL;
        }
    }

    std::vector<std::pair<std::string, std::string>>& keyVals = Random::getKeyVals();

    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);

    std::string address(SERVER_ADDRESS);
    KeyValueClient client(
        grpc::CreateCustomChannel(
            address, 
            grpc::InsecureChannelCredentials(),
            ch_args
        )
    );

    int tops = 0;

    for(int x = 0; x < ca->noOfIntervals; x++) {
        int indices[ca->measInterval], rw[ca->measInterval];
        for(int i = 0; i < ca->measInterval; i++) {
            if(ca->printStats) 
                indices[i] = rand()%(keyVals.size());
            else  {
                indices[i] = (ca->cid)*(ca->noOfIntervals)*(ca->measInterval) + x*(ca->measInterval) + i;
                keyVals[indices[i]] = make_pair(Random::generateRandomString(keySize), Random::generateRandomString(ca->vsize));
            }
            rw[i] = (rand() <= (RAND_MAX*(ca->updateRatio)));
        }

        int ops = 0;

        auto t1 = std::chrono::high_resolution_clock::now();

        for(int i = 0; i < ca->measInterval; i++) {
            if(rw[i]) {
                ops += client.set(keyVals[indices[i]].first, keyVals[indices[i]].second);
            }
            else {
                ops += (client.get(keyVals[indices[i]].first).size() != 0);
            }
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        if(ca->printStats) {
            measureFile<<ops<<' '<<duration<<'\n';
        }

        tops += ops;

        pthread_barrier_wait(&mybarrier);
    }

    if(ca->printStats)
        measureFile.close();

    int * ret = new int;
    *ret = tops;
    return (void *)ret;
}

void initServerClient(int iniWrites, int valueSize, int noOfClients) {

    std::vector<std::pair<std::string, std::string>>& keyVals = Random::generateRandomKeyVals(keySize, valueSize, iniWrites);

    pthread_t clients[noOfClients];

    ClientArgs cargs[noOfClients];

    for(int i = 0; i < noOfClients; i++) {
        cargs[i] = ClientArgs(i, valueSize, 0, 1, iniWrites/noOfClients, 1, false);
        if (pthread_create(&clients[i], NULL, &startClient, (void *)&cargs[i]) != 0) {
            std::cout<<"Problem starting client "<<i<<" for initial writes\n";
        }
    }

    int ops = 0;

    void *ret;
    for(int i = 0; i < noOfClients; i++) {
        pthread_join(clients[i], &ret); 
        ops += *(int *)ret;
    }
    std::cout<<"Successfully stored "<<ops<<" key value pairs in the server\n";
}

int main(int argc, char* argv[]){
    InputParser input(argc, argv);

    if(input.cmdOptionExists("-h")){
        std::cout<<"Usage ./client -v <size_of_value(10b,10k,10m)> -c <no_of_clients> -u <fraction_of_updates> -w <initial_no_rand_writes> -i <measurement_interval> -n <no_of_measurements>\n";
        return 0;
    }

    int valueSize = 4*1024;

    if(input.cmdOptionExists("-v")){
        std::string value = input.getCmdOption("-v");
        char multiplier = value.back();
        value.pop_back();
        valueSize = stoi(value);
        switch(multiplier) {
            case 'g': valueSize *= 1024;
            case 'm': valueSize *= 1024;
            case 'k': valueSize *= 1024;
        }
    }

    int noOfClients = 1;

    if(input.cmdOptionExists("-c")){
        noOfClients = stoi(input.getCmdOption("-c"));
    }

    double updateRatio = 0;

    if(input.cmdOptionExists("-u")){
        updateRatio = stod(input.getCmdOption("-u"));
    }

    int iniWrites = 100;

    if(input.cmdOptionExists("-w")){
        iniWrites = stoi(input.getCmdOption("-w"));
    }

    int measInterval = 100;

    if(input.cmdOptionExists("-i")){
        measInterval = stoi(input.getCmdOption("-i"));
    }

    int noOfIntervals = 10;

    if(input.cmdOptionExists("-n")){
        noOfIntervals = stoi(input.getCmdOption("-n"));
    }

    pthread_barrier_init(&mybarrier, NULL, noOfClients);

    initServerClient(iniWrites, valueSize, noOfClients);

    pthread_t clients[noOfClients];

    ClientArgs cargs[noOfClients];

    for(int i = 0; i < noOfClients; i++) {
        cargs[i] = ClientArgs(i, valueSize, iniWrites, updateRatio, measInterval, noOfIntervals, true);
        if(pthread_create(&clients[i], NULL, &startClient, (void *)&cargs[i]) != 0) {
            std::cout<<"Problem starting client "<<i<<'\n';
        }
    }

    for(int i = 0; i < noOfClients; i++) {
        pthread_join(clients[i], NULL); 
    }

    pthread_barrier_destroy(&mybarrier);

    Run();

    return 0;
}