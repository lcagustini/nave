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
    GS_CHARSELECT,
};

enum projectileType {
    PT_NORMAL,
    PT_EXPLOSIVE,

    PT_MAX
};

struct projectile {
    enum projectileType type;
    int owner_entity;

    int model;
    float speed;
    float scale;
    float range;
    float knockback;

    int damage;

    struct vector pos;
    struct vector vel;

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
    enum projectileType shot_type;

    int model;

    int health;
    int cooldown;

    int max_health;
    int damage;
    int shot_rate;
    float range;
    float shot_speed;
    float speed;
    float knockback;
    float scale;
    float shot_scale;

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
