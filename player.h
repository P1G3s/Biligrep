#ifndef PLAYER_H
#define PLAYER_H
#include "config.h"

#define HEADER "User-Agent: MPlayer, Referer: https://www.bilibili.com/"

void check_error(int status);

int mpvPlay(BVideo bvideo);
#endif
