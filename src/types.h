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
    GLuint texture_id;
    enum faceType face_type;

    struct vector *vertices;
    struct normal *normals;
    struct textureCoord *texture_coords;
    int num_faces;
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

    int damage;

    struct projectile *next;
    struct projectile *prev;
};

enum entityType {
    ET_PLAYER,
    ET_ENEMY_BASIC,

    ET_MAX
};

struct entity {
    enum entityType type;

    int model;

    int health;
    int cooldown;

    int max_health; //TODO
    int damage; //TODO
    int shot_rate; //TODO
    float range; //TODO
    float shot_speed; //TODO
    float speed;
    float knockback; //TODO
    float scale;
    float shot_scale; //TODO

    struct vector pos;
    struct vector vel;
    struct vector dir;

    struct quaternion rotation;
};

enum mapCell {
    MC_WALL,

    MC_P_FLOOR,

    MC_FLOOR,

    MC_MAX
};

struct map {
    int level;

    int models[MC_MAX];
    int grid[MAP_SIZE][MAP_SIZE];
};
