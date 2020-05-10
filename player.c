// Build with: gcc -o simple simple.c `pkg-config --libs --cflags mpv`
// This is the simple version of a mpv API example

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <mpv/client.h>
#define HEADER "User-Agent: MPlayer, Referer: https://www.bilibili.com/, Cookie: _uuid=30569C8D-558F-AAE4-0E66-712DF7DB4FD148666infoc; buvid3=6FFF2E88-6570-45D1-A1BF-149532284470155825infoc; LIVE_BUVID=AUTO9115853601587540; sid=5un8vz46; CURRENT_FNVAL=16; rpdid=|(J|~u~Jlm)|0J'ul)lJYluJk; flash_player_gray=false; html5_player_gray=false; CURRENT_QUALITY=64; PVID=1; DedeUserID=14066157; DedeUserID__ckMd5=6dc1b1f4b6b08f71; SESSDATA=173d81eb%2C1603863194%2C493da*51; bili_jct=6248c199d9f67dc646af20d8cc443972; bp_t_offset_14066157=384427185340436625"

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
	//url = "https://upos-sz-mirrorcos.bilivideo.com/upgcxcode/48/28/188382848/188382848-1-80.flv?e=ig8euxZM2rNcNbhjhbUVhoMz7bNBhwdEto8g5X10ugNcXBlqNxHxNEVE5XREto8KqJZHUa6m5J0SqE85tZvEuENvNo8g2ENvNo8i8o859r1qXg8xNEVE5XREto8GuFGv2U7SuxI72X6fTr859r1qXg8gNEVE5XREto8z5JZC2X2gkX5L5F1eTX1jkXlsTXHeux_f2o859IB_&uipk=5&nbs=1&deadline=1588930348&gen=playurl&os=cosbv&oi=3071736214&trid=3d50f9adf18645209227eee1777113b0u&platform=pc&upsig=86b048f44b96a809e8756f31956ab5fd&uparams=e,uipk,nbs,deadline,gen,os,oi,trid,platform&mid=14066157&logo=80000000";
    const char *cmd[] = {"loadfile", url, NULL};
	check_error(mpv_set_option_string(ctx, "http-header-fields", HEADER));
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
