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

struct vector {
    float x, y, z;
};

struct model {
    struct vector *vertices;
    int num_vertices;
    struct face *faces;
    int num_faces;
    struct normal *normals;
    int num_normals;
    struct textureCoord *texture_coords;
    int num_texture_coords;
    GLuint texture_id;
};

enum faceType {
    VERTEX_ONLY,
    VERTEX_NORMAL,
    VERTEX_ALL,
    VERTEX_ALL_ALPHA
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

enum map_grid_cell {
    MGC_WALL,
    MGC_FLOOR,
};

struct map {
    int models[2];

    int grid[MAP_SIZE][MAP_SIZE];
};
