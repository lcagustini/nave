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
#include "model.c"
#include "physics.c"

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

    GLfloat light_position[] = { 1.4, 1.0, 1.6, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    player.pos.z = 15;
    player.model = loadWavefrontModel("/rd/rolo.obj", "/rd/rolo.vq", VERTEX_ALL, 1024);
    player.speed = 0.02;
    player.hit_radius = 0.25;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 1024);

    while(1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        state = (cont_state_t *)maple_dev_status(cont);

        if (state) {
            memset(&player.dir, 0, sizeof(player.dir));
            if(state->buttons & CONT_DPAD_DOWN) {
                player.dir.y -= 1;
            }
            if(state->buttons & CONT_DPAD_UP) {
                player.dir.y += 1;
            }
            if(state->buttons & CONT_DPAD_LEFT) {
                player.dir.x -= 1;
            }
            if(state->buttons & CONT_DPAD_RIGHT) {
                player.dir.x += 1;
            }
            vectorNormalize(&player.dir);
            player.dir = vectorScale(player.speed, player.dir);
        }

        struct vector player_z = {0, 0, -200};
        struct vector normal_sum = {0, 0, 0};
        collidesWithMap(&normal_sum, &player_z);
        player.normal_sum = normal_sum;
        struct vector up = {0,0,1};
        player.rotation = getRotationQuat(up, normal_sum);
        if (vectorLen(player.dir) > player.speed) {
            vectorNormalize(&player.dir);
            player.dir = vectorScale(player.speed, player.dir);
        }
        player.pos = vectorAdd(player.pos, player.dir);
        if (player_z.z > 0) {
            player.pos.z += 0.5*(player_z.z - player.pos.z);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(-player.pos.x, -player.pos.y, -14.0f);

        glPushMatrix();
        glScalef(MAP_SCALE, MAP_SCALE, MAP_SCALE);
        drawModel(cur_map.model);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(player.pos.x, player.pos.y, player.pos.z);
        // From AllegroGL`s math.c
        glRotatef((2*acos(player.rotation.w)) * 180 / M_PI,
                player.rotation.x,
                player.rotation.y,
                player.rotation.z);
        glScalef(0.24, 0.24, 0.24);
        drawModel(player.model);
        glPopMatrix();

        glutSwapBuffers();
    }

    glDeleteTextures(1, &player.model.texture_id);

    return 0;
}
