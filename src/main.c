#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAX_OBJ_VERTICES 10000
#define MAX_OBJ_FACES 10000

#define MAP_SCALE 1

#include "common.h"

struct map cur_map;
struct player player;

#include "vector.c"

#include "input.c"
#include "model.c"
#include "gfx.c"
#include "physics.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    gfxInit();

    player.pos.z = 15;
    player.model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 1024);
    player.speed = 0.02;
    player.hit_radius = 0.25;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 1024);

    while (1) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        getInput(state);

        struct vector player_z = {0, 0, -200};
        collidesWithMap(&player_z);
        player.dir.z = 0.5*(player_z.z - player.pos.z);
        player.pos = vectorAdd(player.pos, player.dir);

        drawFrame();

        glutSwapBuffers();
    }

    glDeleteTextures(1, &player.model.texture_id);

    return 0;
}
