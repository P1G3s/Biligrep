#ifndef SPAWNER_H
#define SPAWNER_H
#define PROTOCOL "HTTP/1.1"
#define REQTYPE "GET"

char* spawnReq(const char* url, const char* header);

char* spawnHeader(const char* hostname);

char* spawnHostName(const char* url);
#endif
