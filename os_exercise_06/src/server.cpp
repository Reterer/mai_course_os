// Пользовательский узел
#include <iostream>
#include <string>
#include <zmq.hpp>
#include <unistd.h>
#include <csignal>

#include "message.hpp"
#include "zmq_tools.hpp"

static zmq::context_t context;

static zmq::socket_t pub_sock(context, ZMQ_PUB);
static zmq::socket_t sub_sock(context, ZMQ_SUB);

static std::string socket_name;
static std::vector<std::string> sockets_of_children;

void server_init(){
    try {
        socket_name = create_name_of_socket(0);
        pub_sock.bind(socket_name);

        sub_sock.setsockopt(ZMQ_SUBSCRIBE, 0, 0);
        int timeout = 1000;
        sub_sock.setsockopt(ZMQ_RCVTIMEO, timeout);
    }
    catch (zmq::error_t) {
        std::cout << "Error:ZMQ: " << zmq_strerror(errno) << std::endl;
        exit(-1);
    }

}

void server_deinit(){
    try {
        zmq::message_t msg = fill_message_remove(BROADCAST_ID);
        pub_sock.send(msg);

        pub_sock.unbind(socket_name);
        pub_sock.close();

        for(auto& name : sockets_of_children)
            sub_sock.disconnect(name);
        sub_sock.close();

        context.~context_t();
    }
    catch (zmq::error_t){
        std::cout << "Error:ZMQ: " << zmq_strerror(errno) << std::endl;
        exit(-2);
    }
}

bool receive_msg(zmq::message_t& msg){
    zmq::recv_result_t res;
    res = sub_sock.recv(msg);

    return res.has_value();
}

bool is_exists(int id) {
    zmq::message_t data = fill_message_ping(id);
    pub_sock.send(data);

    zmq::message_t receive_data;
    header_t* header;
    
    if(!receive_msg(receive_data))
        return false;
    
    ping_body_answer ans = get_message_ping_answer(receive_data);
    if(ans.src_id != id)
        return false;
    
    return true;
}

void handle_create(){
    int new_node_pid = -1;
    int parent_id, child_id;
    std::cin >> child_id >> parent_id;

    if(parent_id != -1 && !is_exists(parent_id)){
        std::cout << "Error: Parent not found" << std::endl;
        return;
    }
    if(is_exists(child_id)){
        std::cout << "Error: Already exists" << std::endl;
        return;
    }

    if(parent_id == -1){
        // Родитель - пользовательский узел
        int fork_pid = fork();
        // Ошибка fork
        if(fork_pid == -1) {
            std::cout << "Error:fork: " << strerror(errno) << std::endl;
            return;
        }
        // Процесс ребенка
        if(fork_pid == 0) {
            std::stringstream sstream;
            sstream << child_id;
            execl("client", "client", sstream.str().c_str(), socket_name.c_str(), NULL);
        }
        // Подписываемся на дочерний узел
        std::string parent_pub_socket_name = create_name_of_socket_to_parent(child_id);
        sockets_of_children.push_back(parent_pub_socket_name);
        sub_sock.connect(parent_pub_socket_name);
        
        new_node_pid = fork_pid;
    }
    else {
        // Отослать комманду
        zmq::message_t data = fill_message_create(parent_id, child_id);
        pub_sock.send(data);

        zmq::message_t receive_data;
        receive_msg(receive_data);
        create_body_answer ans = get_message_create_answer(receive_data);
        
        if(ans.pid == -1){
            std::cout << "Error:remote_create: " << ans.error << std::endl;
            return;
        }

        new_node_pid = ans.pid;
    }

    std::cout << "OK: " << new_node_pid << std::endl;
}

void handle_remove() {
    int remove_id;
    std::cin >> remove_id;

    if(!is_exists(remove_id)) {
        std::cout << "Error: Not found" << std::endl;
        return;
    }

    zmq::message_t data = fill_message_remove(remove_id);
    pub_sock.send(data);

    std::cout << "OK" << std::endl;
}

void handle_exec() {
    std::string text, pattern;
    int dest_id;

    std::cin.get();
    std::getline(std::cin, text);
    std::getline(std::cin, pattern);
    std::cin >> dest_id;

    if(!is_exists(dest_id)) {
        std::cout << "Error:" << dest_id << ": Not found" << std::endl;
        return;
    }

    zmq::message_t data = fill_message_exec(dest_id, text, pattern);
    pub_sock.send(data);

    zmq::message_t receive_data;
    header_t* header;
    receive_msg(receive_data);
    exec_body_answer ans = get_message_exec_answer(receive_data);
    
    std::cout << "OK:" << dest_id << ": [";
    if(ans.entries.size() > 0)
        std::cout << ans.entries[0];
    for(int i = 1; i < ans.entries.size(); ++i) {
        std::cout << ", " << ans.entries[i];
    }
    std::cout << "]" << std::endl;
}

void handle_ping() {
    int dest_id;
    std::cin >> dest_id;
    if(is_exists(dest_id))
        std::cout << "OK: 1" << std::endl;
    else
        std::cout << "Error: Not found" << std::endl;
}

void server_run(){
    std::string input_cmd;
    while(std::cin >> input_cmd) {
        try{
            if(input_cmd == "create")
            handle_create();
            else if(input_cmd == "remove")
                handle_remove();
            else if(input_cmd == "exec")
                handle_exec();
            else if(input_cmd == "ping")
                handle_ping();
            else {
                std::cout << "Error: данной команды нет!" << std::endl;
            }
        }
        catch(error_t) {
            std::cout << "Error:ZMQ: " << zmq_strerror(errno) << std::endl;
        }
    }
}

void server_term(int) {
    server_deinit();
    exit(0);
}

int main() {
    std::signal(SIGINT, server_term);
    std::signal(SIGTERM, server_term);

    server_init();
    server_run();
    server_deinit();
}