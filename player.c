#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpv/client.h>
#include <locale.h>
#include <string.h>

#include "config.h"

#define HEADER "User-Agent: MPlayer, Referer: https://www.bilibili.com/"


void check_error(int status)
{
    if (status < 0) {
        printf("mpv API error: %s\n", mpv_error_string(status));
        exit(1);
    }
}

int mpvPlay(BVideo bvideo)
{
	setlocale(LC_NUMERIC, "C");
    mpv_handle *ctx = mpv_create();
    if (!ctx) {
        printf("failed creating context\n");
        return 1;
    }

    check_error(mpv_set_option_string(ctx, "input-default-bindings", "yes"));
    mpv_set_option_string(ctx, "input-vo-keyboard", "yes");
    int val = 1;
    check_error(mpv_set_option(ctx, "osc", MPV_FORMAT_FLAG, &val));
    check_error(mpv_initialize(ctx));

	char* format = "https://www.bilibili.com/video/%s";
	char* url = calloc(strlen(format)+15,sizeof(char));
	sprintf(url,format,bvideo.bid);
    const char *cmd[] = {"loadfile", url, NULL};
	printf("You are now watching '%s'\n",bvideo.title);
	check_error(mpv_set_option_string(ctx, "ytdl", "yes"));
    check_error(mpv_command(ctx, cmd));

	// Wait for the first idle
    mpv_event *event = mpv_wait_event(ctx, 10000);
    while (1) {
        event = mpv_wait_event(ctx, 10000);
        if ((event->event_id == MPV_EVENT_SHUTDOWN)||(event->event_id == MPV_EVENT_IDLE))
            break;
    }
    mpv_terminate_destroy(ctx);
    return 0;
}
