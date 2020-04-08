#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "config.h"
#include "common.h"
#include "globals.h"

#include "vector.c"
#include "physics.c"
#include "projectiles.c"
#include "input.c"
#include "model.c"
#include "gfx.c"
#include "entity.c"

#include "gamestates/game.c"
#include "gamestates/title.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    gfxInit();

    cur_gs = GS_TITLE;
    while (true) {
        switch (cur_gs) {
            case GS_TITLE:
                runTitle();
                break;
            case GS_GAME:
                runGame();
                break;
            default:
                return 1;
        }
    }

    return 0;
}
