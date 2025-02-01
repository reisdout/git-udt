#pragma once 
#include "./Socket.h" 
#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include <string>
#include <iostream>

#define PORT 5000 
#define MAXLINE 350*SEGMENT_SIZE 

class SocketTCPNewReno : public Socket{
public:

char buffer[MAXLINE]; 
std::string message; 
SocketTCPNewReno();
virtual ~SocketTCPNewReno(){};
virtual void Send();
virtual void SendNSegmens(unsigned parNSeg){};
virtual void Receive();
virtual void Bind();



};