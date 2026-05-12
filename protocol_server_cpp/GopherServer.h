#ifndef GOPHERSERVER_H
#define GOPHERSERVER_H

#include "ProtocolServer.h"
#include <pthread.h>
#include <list>

class GopherConnection;

class GopherServer : public ProtocolServer {
public:
    GopherServer(int port = 0);
    ~GopherServer();

    void run() override;

private:
    std::list<GopherConnection*> connection_list;
};

#endif
