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
    VERTEX_TEXTURE,
    VERTEX_ALL
};

struct quaternion {
    float w;
    float x;
    float y;
    float z;
};

struct player {
    struct vector pos;
    struct model model;
};

