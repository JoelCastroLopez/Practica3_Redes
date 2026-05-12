#ifndef GEMINISERVER_H
#define GEMINISERVER_H

#include "ProtocolServer.h"
#include <pthread.h>
#include <list>

class GeminiConnection;

class GeminiServer : public ProtocolServer {
public:
    GeminiServer(int port = 0, bool use_tls = false);
    ~GeminiServer();

    void run() override;
    
    bool is_using_tls() const { return use_tls; }

private:
    bool use_tls;
    std::list<GeminiConnection*> connection_list;
};

#endif
