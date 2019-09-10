#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <assert.h>
#include <math.h>

#define bool char
#define true 1
#define false 0

#define MAX_OBJ_VERTICES 100000
#define MAX_OBJ_FACES 100000

typedef struct {
    float x;
    float y;
    float z;
} Normal;

typedef struct {
    float x;
    float y;
} TextureCoord;

typedef struct {
    int vertices[3];
    int normals[3];
    int texture_coords[3];
} Face;

typedef struct {
    float x, y, z;
} Vector;

typedef struct {
    Vector *vertices;
    int num_vertices;
    Face *faces;
    int num_faces;
    Normal *normals;
    int num_normals;
    TextureCoord *texture_coords;
    int num_texture_coords;
    GLuint texture_id;
} Model;

typedef enum {
    VERTEX_ONLY,
    VERTEX_NORMAL,
    VERTEX_TEXTURE,
    VERTEX_ALL
} FaceType;

typedef struct {
    float w;
    float x;
    float y;
    float z;
} Quat;

typedef struct {
    Vector pos;
    Vector dir;

    float pitch;
    float roll;
} Player;

inline static float vectorLen(Vector v) {
    float len = v.x * v.x + v.y * v.y + v.z * v.z;
    return (float) fsqrt(len);
}

inline static void vectorNormalize(Vector *v) {
    float len = v->x * v->x + v->y * v->y + v->z * v->z;
    if (!len)
        return;
    float div = frsqrt(len);
    v->x *= div;
    v->y *= div;
    v->z *= div;
}

inline static float vectorDot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector vectorCross(Vector a, Vector b) {
    Vector n = {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };
    return n;
}

inline Vector vectorScale(float a, Vector v) {
    v.x *= a;
    v.y *= a;
    v.z *= a;
    return v;
}

inline Vector vectorAdd(Vector a, Vector b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
Vector vectorRotate(Vector v, Quat q) {
    Vector u = {.x = q.x, .y = q.y, .z = q.z};
    float s = q.w;

    return vectorAdd(vectorScale(2.0f * vectorDot(u, v), u),
            vectorAdd(vectorScale(s*s - vectorDot(u, u), v),
                vectorScale(2.0f * s, vectorCross(u, v))));
}

Quat getRotationQuat(Vector from, Vector to) {
    Quat result;
    Vector H;

    H.x = from.x + to.x;
    H.y = from.y + to.y;
    H.z = from.z + to.z;
    vectorNormalize(&H);

    result.w = vectorDot(from, H);
    result.x = from.y*H.z - from.z*H.y;
    result.y = from.z*H.x - from.x*H.z;
    result.z = from.x*H.y - from.y*H.x;

    return result;
}

Quat quatMult(Quat a, Quat b) {
    Quat ret;

    ret.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
    ret.x = a.w*b.x + a.x*b.w - a.y*b.z + a.z*b.y;
    ret.y = a.w*b.y + a.x*b.z + a.y*b.w - a.z*b.x;
    ret.z = a.w*b.z - a.x*b.y + a.y*b.x + a.z*b.w;

    return ret;
}

void drawModel(Model model) {
    glBindTexture(GL_TEXTURE_2D, model.texture_id);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; i < model.num_faces; i++) {
        Face f = model.faces[i];

        glBegin(GL_TRIANGLES);

        for (int j = 0; j <= 2; j++) {

            if (model.num_normals) {
                Normal n = model.normals[f.normals[j]];
                glNormal3f(n.x, n.y, n.z);
            }

            if (model.num_texture_coords) {
                TextureCoord t = model.texture_coords[f.texture_coords[j]];
                glTexCoord2f(t.x, t.y);
            }

            Vector v = model.vertices[f.vertices[j]];

            glVertex3f(v.x, v.y, v.z);
        }

        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

Model loadWavefrontModel(const char *obj_filename, const char *texture_filename, FaceType face_type, int texture_size) {
    Model model = {};

    // NOTE: Objects bigger than the constants are undefined behavior
    model.vertices = (Vector *) malloc(MAX_OBJ_VERTICES * sizeof(Vector));
    model.faces = (Face *) malloc(MAX_OBJ_FACES * sizeof(Face));
    model.normals = (Normal *) malloc(MAX_OBJ_VERTICES * sizeof(Normal));
    model.texture_coords = (TextureCoord *) malloc(MAX_OBJ_VERTICES * sizeof(TextureCoord));

    if (face_type == VERTEX_ALL || face_type == VERTEX_TEXTURE) {
        // NOTE: this can be freed if it`s not a map model

        // normal texture
        {
            FILE *f = fopen(texture_filename, "rb");
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

            char *sur = malloc(fsize);
            fread(sur, 1, fsize, f);
            fclose(f);

            assert(sur);

            glGenTextures(1, &model.texture_id);
            glBindTexture(GL_TEXTURE_2D, model.texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glCompressedTexImage2D(GL_TEXTURE_2D,  /* This must be GL_TEXTURE_2D */
                    0,             /* 0 = Texture does not contain Mip-Maps */
                    GL_UNSIGNED_SHORT_5_6_5_VQ_TWID,        /* GL Compressed Color Format */
                    texture_size,           /* Texture Width */
                    texture_size,           /* Texture Height */
                    0,             /* This bit must be set to 0 */
                    fsize, /* Compressed Texture Size*/
                    sur);       /* Address of texture data in RAM: OpenGL will load the texture into VRAM for you.
                                   Because of this, make sure to call glDeleteTextures() as needed, as that will
                                   free the VRAM allocated for the texture. */
            glBindTexture(GL_TEXTURE_2D, 0);

            free(sur);
        }
    }

    FILE *f = fopen(obj_filename, "r");
    assert(true);
    char type[40] = {};
    while ((fscanf(f, " %s", type)) != EOF) {
        if (!strcmp(type, "v")) {
            Vector v = {};

            fscanf(f, " %f %f %f", &v.x, &v.y, &v.z);
            model.vertices[++model.num_vertices] = v;
        } else if (!strcmp(type, "vn")) {
            Normal n = {};

            fscanf(f, " %f %f %f", &n.x, &n.y, &n.z);
            model.normals[++model.num_normals] = n;
        } else if (!strcmp(type, "vt")) {
            TextureCoord t = {};

            fscanf(f, " %f %f", &t.x, &t.y);
            t.y = 1 - t.y;
            model.texture_coords[++model.num_texture_coords] = t;
        } else if (!strcmp(type, "f")) {
            Face face = {};

            if (face_type == VERTEX_ONLY) {
                fscanf(f, " %d %d %d", &face.vertices[0], &face.vertices[1], &face.vertices[2]);
            } else if (face_type == VERTEX_NORMAL) {
                fscanf(f, " %d//%d %d//%d %d//%d", &face.vertices[0], &face.normals[0], &face.vertices[1],
                        &face.normals[1], &face.vertices[2], &face.normals[2]);
            } else if (face_type == VERTEX_TEXTURE) {
                // NOTE: not supported
                assert(false);
            } else if (face_type == VERTEX_ALL) {
                fscanf(f, " %d/%d/%d %d/%d/%d %d/%d/%d", &face.vertices[0], &face.texture_coords[0],
                        &face.normals[0], &face.vertices[1],
                        &face.texture_coords[1], &face.normals[1],
                        &face.vertices[2], &face.texture_coords[2],
                        &face.normals[2]);
            } else {
                assert(false);
            }

            model.faces[model.num_faces++] = face;
        }
    }

    return model;
}

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

void mm(float matA[1][3], float yaw, float pitch, float roll, float out[1][3]) {
    float matB[3][3] = {
        {fcos(yaw)*fcos(pitch), fcos(yaw)*fsin(pitch)*fsin(roll)-fsin(yaw)*fcos(roll), fcos(yaw)*fsin(pitch)*fcos(roll)+fsin(yaw)*fsin(roll)},
        {fsin(yaw)*fcos(pitch), fsin(yaw)*fsin(pitch)*fsin(roll)+fcos(yaw)*fcos(roll), fsin(yaw)*fsin(pitch)*fcos(roll)-fcos(yaw)*fsin(roll)},
        {-fsin(pitch), fcos(pitch)*fsin(roll), fcos(pitch)*fcos(roll)}
    };
    for (int i = 0; i < 1; i++) {
        for(int j = 0; j < 3; j++) {
            for(int k = 0; k < 3; k++) {
                out[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
}

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

    Player player;
    memset(&player, 0, sizeof(player));
    Model ship = loadWavefrontModel("/rd/spaceship.obj", "/rd/spaceship.vq", VERTEX_ALL, 1024);

    while(1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        if(!cont) continue;

        state = (cont_state_t *)maple_dev_status(cont);
        if(!state) continue;

        if (state->joyy > 20) {
            player.pitch += 0.05;
        }
        else if (state->joyy < -20) {
            player.pitch -= 0.05;
        }
        if (state->joyx > 20) {
            player.roll += 0.05;
        }
        else if (state->joyx < -20) {
            player.roll -= 0.05;
        }

        player.dir.x = -fsin(player.pitch)*fsin(player.roll);
        player.dir.y = fcos(player.roll)*fsin(player.pitch);
        player.dir.z = -fcos(player.pitch);

        vectorNormalize(&player.dir);

        printf("%f %f %f\n", player.dir.x, player.dir.y, player.dir.z);

        //player.pos.x += (state->rtrig/1000.0f) * player.dir.x;
        //player.pos.y += (state->rtrig/1000.0f) * player.dir.y;
        //player.pos.z += (state->rtrig/1000.0f) * player.dir.z;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glTranslatef(-player.pos.x, -player.pos.y, -player.pos.z - 35.0f);

        glPushMatrix();
        glTranslatef(player.pos.x, player.pos.y, player.pos.z);

        Vector up = {0,0,-1};
        Vector a = vectorCross(player.dir, up);
        float omega = acos(vectorDot(up, player.dir));
        glRotatef((180.0f/F_PI) * omega, a.x, a.y, a.z);
        drawModel(ship);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, -60);
        drawModel(ship);
        glPopMatrix();

        glutSwapBuffers();
    }

    glDeleteTextures(1, &ship.texture_id);

    return 0;
}
