#include <kos.h>
#include <stdlib.h>
#include <sys/dirent.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "config.h"
#include "types.h"
#include "globals.h"

#include "vector.c"
#include "memory.c"
#include "model.c"
#include "gfx.c"

#include "game/map.c"
#include "game/physics.c"
#include "game/projectiles.c"
#include "game/input.c"
#include "game/entity.c"

#include "gamestates/game.c"
#include "gamestates/title.c"

int main(int argc, char **argv) {
    glKosInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    srand(time(0));

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
                return ARCH_EXIT_MENU;
        }
    }

    return 0;
}
