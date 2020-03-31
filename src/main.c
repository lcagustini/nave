#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAX_OBJ_VERTICES 10000
#define MAX_OBJ_FACES 10000

#include "common.h"
#include "vector.c"
#include "model.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    maple_device_t *cont;
    cont_state_t *state;

    /* Get basic stuff initialized */
    glKosInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    struct player player = {{0}};
    player.model = loadWavefrontModel("/rd/spaceship.obj", "/rd/spaceship.vq", VERTEX_ALL, 1024);

    while(1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        if(!cont) continue;

        state = (cont_state_t *)maple_dev_status(cont);
        if(!state) continue;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glTranslatef(-player.pos.x, -player.pos.y, -player.pos.z - 35.0f);

        glPushMatrix();
        glTranslatef(player.pos.x, player.pos.y, player.pos.z);
        drawModel(player.model);
        glPopMatrix();

        glutSwapBuffers();
    }

    glDeleteTextures(1, &player.model.texture_id);

    return 0;
}
