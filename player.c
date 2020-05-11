// Build with: gcc -o simple simple.c `pkg-config --libs --cflags mpv`
// This is the simple version of a mpv API example

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpv/client.h>


void check_error(int status)
{
    if (status < 0) {
        printf("mpv API error: %s\n", mpv_error_string(status));
        exit(1);
    }
}

int mpvPlay(char* url)
{
    mpv_handle *ctx = mpv_create();
    if (!ctx) {
        printf("failed creating context\n");
        return 1;
    }

    // Enable default key bindings, so the user can actually interact with
    // the player (and e.g. close the window).
    check_error(mpv_set_option_string(ctx, "input-default-bindings", "yes"));
    mpv_set_option_string(ctx, "input-vo-keyboard", "yes");
    int val = 1;
    check_error(mpv_set_option(ctx, "osc", MPV_FORMAT_FLAG, &val));

    // Done setting up options.
    check_error(mpv_initialize(ctx));

    // Play this file.
    const char *cmd[] = {"loadfile", url, NULL};
	//check_error(mpv_set_option_string(ctx, "http-header-fields", HEADER));
	check_error(mpv_set_option_string(ctx, "ytdl", "yes"));
    check_error(mpv_command(ctx, cmd));

    // Let it play, and wait until the user quits.
	//char* time = malloc(100);
    while (1) {
        mpv_event *event = mpv_wait_event(ctx, 10000);
        printf("event: %s\n", mpv_event_name(event->event_id));
        if (event->event_id == MPV_EVENT_SHUTDOWN)
            break;
    }

    mpv_terminate_destroy(ctx);
    return 0;
}
