#ifndef SERVER_H
#define SERVER_H

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../lib/con2db/pgsql.h"
#include "../../lib/con2redis/src/con2redis.h"
#include <unistd.h>

class Server {
    public:
        Server(char* RedisIP, int RedisPort, int serverPort, char* streamIN, char* streamOUT);
	private:
		redisContext *c2r;
    	redisReply *reply;
		char* WRITE_STREAM;
		char* READ_STREAM;
		void ConnectToRedis(char* RedisIP, int RedisPort, char* streamIN, char* streamOUT);
		void Autenticazione(int serverPort);
};



#endif //SERVER_H
