#ifndef SERVER_H
#define SERVER_H

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <unistd.h>
#include <mutex>

class Server {
    public:
		Server(const char* RedisIP, int RedisPort, int serverPort, const char* streamIN, const char* streamOUT);
		void Autenticazione(const char* PORT, const char* USERNAME, const char* PASSWORD);
	private:
		const char* WRITE_STREAM;
		const char* READ_STREAM;
		void ConnectToRedis(const char* RedisIP, int RedisPort, const char* streamIN, const char* streamOUT);
};



#endif //SERVER_H
