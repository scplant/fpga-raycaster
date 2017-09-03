#include <xscutimer.h>
#include <limits.h>
#include "xtime_l.h"

float averagefps = 0.0;
XTime lasttime = 0;

//Call frame_timer after the call to render_frame().
//After at least 3 frames, get_fps() will contain a valid FPS value.
void frame_timer() {
    if(lasttime == 0) {
        XTime_GetTime(&lasttime);
    } else {
        XTime thistime;

        XTime_GetTime(&thistime);

        XTime frametime = thistime - lasttime;
        lasttime = thistime;
        float timeinsecs = 1.0 * frametime / COUNTS_PER_SECOND;
        float fps = 1.0 / timeinsecs;

        if(averagefps == 0.0) {
            averagefps = fps;
        } else {
            averagefps = (averagefps + fps) / 2.0;
        }
    }
}

float get_fps() {
    return averagefps;
}
