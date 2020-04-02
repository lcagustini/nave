#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAX_OBJ_VERTICES 1000
#define MAX_OBJ_FACES 1000

#define MAP_SCALE 1

#define PLAYER_ID 0

#include "common.h"

struct map cur_map;

struct entity entities[10];
int entities_size;

#include "vector.c"

#include "input.c"
#include "model.c"
#include "gfx.c"
#include "physics.c"
#include "entity.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    gfxInit();

    entities[PLAYER_ID].pos.z = 15;
    entities[PLAYER_ID].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 1024);
    entities[PLAYER_ID].speed = 0.05;
    entities[PLAYER_ID].hit_radius = 0.25;
    entities[PLAYER_ID].type = ET_PLAYER;

    entities[1].pos.x = 0.1;
    entities[1].pos.y = 0.7;
    entities[1].pos.z = 15;
    entities[1].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 1024);
    entities[1].speed = 0.02;
    entities[1].hit_radius = 0.25;
    entities[1].type = ET_ENEMY_BASIC;

    entities_size = 2;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 1024);

    while (1) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        getInput(state);

        for (int i = 0; i < entities_size; i++) {
            doEntityFrame(i);
        }

        drawFrame();

        glutSwapBuffers();
    }

    for (int i = 0; i < entities_size; i++) {
        glDeleteTextures(1, &entities[i].model.texture_id);
    }
    glDeleteTextures(1, &cur_map.model.texture_id);

    return 0;
}
