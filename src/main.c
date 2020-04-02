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
struct entity player;

struct entity enemies[10];
int enemies_size;

#include "vector.c"

#include "input.c"
#include "model.c"
#include "gfx.c"
#include "physics.c"
#include "enemy.c"

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
    gfxInit();

    player.pos.z = 15;
    player.model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 1024);
    player.speed = 0.05;
    player.hit_radius = 0.25;

    enemies[0].pos.x = 0.1;
    enemies[0].pos.y = 0.7;
    enemies[0].pos.z = 15;
    enemies[0].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 1024);
    enemies[0].speed = 0.02;
    enemies[0].hit_radius = 0.25;
    enemies[0].type = ET_ENEMY_BASIC;
    enemies_size = 1;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 1024);

    while (1) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        getInput(state);

        struct vector player_z = {0, 0, -200};
        collidesWithMap(&player, &player_z);
        player.dir.z = 0.5*(player_z.z - player.pos.z);
        player.pos = vectorAdd(player.pos, player.dir);

        for (int i = 0; i < enemies_size; i++) {
            doEnemyFrame(i);
        }

        drawFrame();

        glutSwapBuffers();
    }

    glDeleteTextures(1, &player.model.texture_id);
    glDeleteTextures(1, &cur_map.model.texture_id);

    return 0;
}
