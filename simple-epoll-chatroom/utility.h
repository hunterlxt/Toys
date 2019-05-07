#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

// clients_list save all the clients's socket
list<int> clients_list;

/**********************   macro defintion **************************/
// server ip 可以更改为其它IP地址
#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

// epoll size 支持的最大句柄数量
#define EPOLL_SIZE 5000

// message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME                                                         \
    "Welcome you join to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

// exit
#define EXIT "EXIT"

#define CAUTION "There is only one int the char room!"

/**********************   some function **************************/
/**
 * @param sockfd: socket descriptor
 * @return 0
 * 将文件描述符设置为非阻塞的方式
 * fcntl用来操纵文件描述符的一些特性，第一个参数是fd，第二个参数是命令
 **/
int setnonblocking(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

/**
 * @param epollfd: epoll handle
 * @param fd: socket descriptor
 * @param enable_et : enable_et = true, epoll use ET; otherwise LT
 * 将fd加入epoll内核事件表中，并注册EPOLLIN和EPOLLET两个事件，最后将fd设为非阻塞
 **/
void addfd(int epollfd, int fd, bool enable_et) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et)
        ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
    printf("fd added to epoll!\n\n");
}

/**
 * @param clientfd: socket descriptor
 * @return : len
 * 服务器发送广播信息到所有的clients
 **/
int sendBroadcastmessage(int clientfd) {
    // buf[BUF_SIZE] receive new chat message
    // message[BUF_SIZE] save format message
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    // receive message
    printf("read from client(clientID = %d)\n", clientfd);
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    if (len == 0) // len = 0 means the client closed connection
    {
        close(clientfd);
        clients_list.remove(clientfd); // server remove the client
        printf("ClientID = %d closed.\n now there are %d client in the char "
               "room\n",
               clientfd, (int)clients_list.size());

    } else // broadcast message
    {
        if (clients_list.size() ==
            1) { // this means There is only one int the char room
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }
        // format message to broadcast
        sprintf(message, SERVER_MESSAGE, clientfd, buf);

        list<int>::iterator it;
        for (it = clients_list.begin(); it != clients_list.end(); ++it) {
            if (*it != clientfd) {
                if (send(*it, message, BUF_SIZE, 0) < 0) {
                    perror("error");
                    exit(-1);
                }
            }
        }
    }
    return len;
}
#endif // UTILITY_H_INCLUDED