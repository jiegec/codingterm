
#include "mytcpserver.h"

void MyTcpServer::incomingConnection(qintptr fd) {
    emit newConnection(fd);
}