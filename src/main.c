#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAX_OBJ_VERTICES 1000
#define MAX_OBJ_FACES 1000

#define MAP_SCALE 0.25

#define PLAYER_ID 0

#define DEADZONE 25

#include "common.h"

struct map cur_map;

struct entity entities[10];
int entities_size;

struct projectile *projectiles;

#include "vector.c"

#include "physics.c"
#include "projectiles.c"
#include "input.c"
#include "model.c"
#include "gfx.c"
#include "entity.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    gfxInit();

    entities[PLAYER_ID].pos.z = 15;
    entities[PLAYER_ID].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[PLAYER_ID].health = 10;
    entities[PLAYER_ID].scale = 0.24;
    entities[PLAYER_ID].speed = 0.05;
    entities[PLAYER_ID].hit_radius = 0.9375;
    entities[PLAYER_ID].type = ET_PLAYER;

    entities[1].pos.x = 1;
    entities[1].pos.y = -3;
    entities[1].pos.z = 15;
    entities[1].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[1].health = 3;
    entities[1].scale = 0.24;
    entities[1].speed = 0.02;
    entities[1].hit_radius = 0.9375;
    entities[1].type = ET_ENEMY_BASIC;

    entities[2].pos.x = 1;
    entities[2].pos.y = 3;
    entities[2].pos.z = 15;
    entities[2].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[2].health = 3;
    entities[2].scale = 0.24;
    entities[2].speed = 0.02;
    entities[2].hit_radius = 0.9375;
    entities[2].type = ET_ENEMY_BASIC;

    entities_size = 3;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 512);

    while (1) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        getInput(state);

        for (struct projectile *cur = projectiles; cur; cur = cur->next) {
            doProjectileFrame(cur);
        }
        for (int i = 0; i < entities_size; i++) {
            doEntityFrame(i);
            i = checkDeadEntity(i);
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
