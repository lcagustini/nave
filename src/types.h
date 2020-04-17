struct normal {
    float x;
    float y;
    float z;
};

struct textureCoord {
    float x;
    float y;
};

struct face {
    int vertices[3];
    int normals[3];
    int texture_coords[3];
};

enum vectorDimention {
    VD_X,
    VD_Y,
    VD_Z
};

struct vector {
    float x, y, z;
};

enum faceType {
    VERTEX_ONLY,
    VERTEX_NORMAL,
    VERTEX_TEXTURE,
    VERTEX_ALL,
    VERTEX_ALL_ALPHA
};

struct model {
    struct vector *vertices;
    int num_vertices;
    struct face *faces;
    int num_faces;

    GLuint texture_id;
    enum faceType face_type;
    float *draw_vertices;
    int num_draw_vertices;
    float *draw_normals;
    int num_draw_normals;
    float *draw_texture_coords;
    int num_draw_texture_coords;
};

struct quaternion {
    float w;
    float x;
    float y;
    float z;
};

enum gamestate {
    GS_TITLE,
    GS_GAME,
};

struct projectile {
    struct vector pos;
    struct vector vel;

    int model;
    float speed;
    float scale;

    float hit_radius;

    int damage;

    struct projectile *next;
    struct projectile *prev;
};

enum entityType {
    ET_PLAYER,
    ET_ENEMY_BASIC,
};

struct entity {
    struct vector pos;
    struct vector vel;
    struct vector dir;

    int model;
    float scale;

    struct quaternion rotation;

    float speed;
    float hit_radius;

    int health;
    int damage;

    int cooldown;

    enum entityType type;
};

enum mapCell {
    MC_WALL,

    MC_P_FLOOR,

    MC_FLOOR,

    MC_MAX
};

struct map {
    int models[MC_MAX];
    int grid[MAP_SIZE][MAP_SIZE];
};
