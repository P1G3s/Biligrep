#ifndef UTILS_H
#define UTILS_H
#include "config.h"

// Grep property to the holder with key from JSON format object
// *Holder shall be freed by the caller
void grepJSONProp(int fd, int klen, const char** key, char** holder);

// Grep 'Content-Length' property in http response header
long grepContentLength(int ClientSocketfd);

// Grep playlist from .dcahce according to page
// *Playlist shall be freed by the caller
playlist* getPlaylist(int page);

// 'Push' Dynamics info to .dcache
void pushDynamics();
#endif
