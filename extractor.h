#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "config.h"

void extractID(BVideo* bvideo);

char* extractPlayUrl(BVideo* bvideo);

void requestVideoStream(BVideo*, const char* url);

playlist* extractLatestDynamic();

playlist* extractHistory(int page);

#endif
