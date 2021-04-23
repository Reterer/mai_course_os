// Вычислительный узел
#include <iostream>
#include <string>
#include <zmq.hpp>
#include <unistd.h>
#include <csignal>

#include "message.hpp"
#include "zmq_tools.hpp"

static zmq::context_t context;
static zmq::socket_t pub_sock_to_children(context, ZMQ_PUB);
static zmq::socket_t pub_sock_to_parent(context, ZMQ_PUB);
static zmq::socket_t sub_sock(context, ZMQ_SUB);

static std::string pub_socket_to_children_name;
static std::string pub_socket_to_server_name;

static std::string parent_socket_name;
static std::vector<std::string> children_sockets_name;

static int node_id;
static bool run;

// node_id, parent_socket_name
void client_init(int argc, char* argv[]){
    std::stringstream sstream;
    sstream << argv[1];
    sstream >> node_id; 
    
    parent_socket_name = argv[2];

    pub_socket_to_children_name = create_name_of_socket_to_children(node_id);
    pub_socket_to_server_name = create_name_of_socket_to_parent(node_id);
    
    pub_sock_to_children.bind(pub_socket_to_children_name);
    pub_sock_to_parent.bind(pub_socket_to_server_name);

    sub_sock.connect(parent_socket_name);
    sub_sock.setsockopt(ZMQ_SUBSCRIBE, 0, 0);

    run = true;
}

void handle_create(zmq::message_t& data) {
    create_body body = get_message_create(data);
    int child_id = body.child_id;
    int fork_pid = fork();
    // Ошибка
    if(fork_pid == -1) {
        zmq::message_t ans = fill_message_create_answer(-1, strerror(errno));
        pub_sock_to_parent.send(ans);
        return;
    }
    // Процесс ребенка
    if(fork_pid == 0) {
        std::stringstream sstream;
        sstream << child_id;
        execl("client", "client", sstream.str().c_str(), pub_socket_to_children_name.c_str(), NULL);
    }
    
    std::string parent_pub_socket_name = create_name_of_socket_to_parent(child_id);
    children_sockets_name.push_back(parent_pub_socket_name);
    sub_sock.connect(parent_pub_socket_name);

    // Отсылаем ответ, что все вроде ОК
    zmq::message_t ans = fill_message_create_answer(fork_pid, "");
    pub_sock_to_parent.send(ans);
}

void handle_remove(zmq::message_t& data) {
    header_t* header = get_message_header(data);
    // Отсылаем детям
    header->to_id_node = BROADCAST_ID;
    pub_sock_to_children.send(data);
    // Удаляем текущий узел
    run = false;
}

void handle_exec(zmq::message_t& data) {
    exec_body body = get_message_exec(data);
    std::vector<int> entries;
    
    std::string::size_type pos = 0;
    while(std::string::npos != (pos = body.text.find(body.pattern, pos))){
        entries.push_back(pos);
        ++pos;
    }

    zmq::message_t ans = fill_message_exec_answer(entries);
    pub_sock_to_parent.send(ans);
}

void handle_ping(zmq::message_t& data) {
    zmq::message_t ans = fill_message_ping_answer(node_id);
    pub_sock_to_parent.send(ans);
}

void handle_task(zmq::message_t& data){
    header_t* header = get_message_header(data);
    if(header->type == MSG_CREATE)
        handle_create(data);
    else if(header->type == MSG_REMOVE)
        handle_remove(data);
    else if(header->type == MSG_EXEC)
        handle_exec(data);
    else if(header->type == MSG_PING)
        handle_ping(data);
}

void client_run(){
    while(run){
        try {
            zmq::message_t data;
            sub_sock.recv(data);
            header_t* header = get_message_header(data);
            // Если сообщение адресовано нам
            if(header->to_id_node == node_id || header->to_id_node == BROADCAST_ID)
                handle_task(data);
            // Иначе пропускаем его дальше
            if(header->to_id_node != node_id){
                if(header->dir == DIR_TO_SERVER)
                    pub_sock_to_parent.send(data);
                else
                    pub_sock_to_children.send(data);
            }    
        }
        catch(zmq::error_t) {
            std::cout << zmq_strerror(errno) << std::endl;
        }
        
    }
}

void client_deinit(){
    // Нужно удалить детей
    zmq::message_t msg = fill_message_remove(BROADCAST_ID);
    pub_sock_to_children.send(msg);

    // Закрываемся сами
    pub_sock_to_children.unbind(pub_socket_to_children_name);
    pub_sock_to_children.close();

    pub_sock_to_parent.unbind(pub_socket_to_server_name);
    pub_sock_to_parent.close();

    for(auto& name : children_sockets_name)
        sub_sock.disconnect(name);
    sub_sock.close();
}

void client_term(int){
    client_deinit();
    exit(0);
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, client_term);
    std::signal(SIGTERM, client_term);

    client_init(argc, argv);
    client_run();
    client_deinit();
    return 0;
}