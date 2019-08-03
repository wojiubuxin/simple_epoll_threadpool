#ifndef _EPOLL_H
#define _EPOLL_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "thread.h"

class Epoll
{
public:
  Epoll();
  ~Epoll();
  void setnoblock(int sockfd);
private:
  int epfd;
  struct epoll_event events[EPOLL_SIZE];
  int listenfd;
};

Epoll::Epoll()
{
  epfd = epoll_create(1024);
  ThreadPool pool;
  struct sockaddr_in serveraddr;
  struct sockaddr_in clientaddr;
  struct epoll_event ev;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  setnoblock(listenfd);
  char data[256];
  ev.data.fd = listenfd;
  ev.events = EPOLLIN | EPOLLET;
  
  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_famliy = AF_INET;
  serveraddr.sin_port = htons(800);
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  bind(listenfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
  listen(listenfd, 5);
  int nfds = 0;
  int connfd = 0;
  while(1)
  {
    nfds = epoll_wait(epfd, events, EPOLL_SIZE, 0);
    for(int i = 0; i < nfds; i++)
    {
      if(events[i].data.fd == listenfd)
      {
        connfd = accept(listenfd, (sockaddr*)&clientaddr, sizeof(clientaddr));
        if(connfd < 0)
        {
          //exit(1);
          continue;
        }
        setnoblock(connfd);
        ev.data.fd = connfd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
      }
      else//略&EPOLLIN OR EPOLLOUT
      {
        int sockfd = 0;
        if((sockfd = events[i].data.fd) < 0)
          continue;
        int n = 0;
        if(n = read(sockfd, data, 256) < 0)
          continue;
         task ctask;
         //...
         pool.AddTask(ctask);
         ev.data.fd = sockfd;
         ev.events = EPOLLOUT | EPOLLET | EPOLLIN;
         epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
      }
    }
  }
}

Epoll::~Epoll()
{
  poll.StopAll_Thread();
  close(epfd);
}

void Epoll::setnoblock(int sockfd)
{
  int opts = 0;
  opts = fcntl(sock, F_GETFL);
  if(opts < 0)
    return;
  opts |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, opts);
   
}

#endif
