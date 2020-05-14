#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpv/client.h>
#include <locale.h>
#include <string.h>

#include "config.h"

#define HEADER "User-Agent: MPlayer, Referer: https://www.bilibili.com/"

void check_error(int status);

int mpvPlay(BVideo bvideo);
