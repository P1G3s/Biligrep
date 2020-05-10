#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "spawner.h"
#include "net.h"
#include "utils.h"


void extractID(BVideo* bvideo);

char* extractPlayUrl(BVideo* bvideo);

void requestVideoStream(BVideo*, const char* url);

playlist* extractLatestDynamic();

playlist* extractHistory(int page);

