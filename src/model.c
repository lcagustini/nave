void destroyModel(int model) {
    free(loaded_models[model].vertices);
    free(loaded_models[model].faces);
    free(loaded_models[model].normals);
    free(loaded_models[model].texture_coords);
    if (loaded_models[model].face_type == VERTEX_ALL || loaded_models[model].face_type == VERTEX_ALL_ALPHA) {
        glDeleteTextures(1, &loaded_models[model].texture_id);
    }
}

struct model loadWavefrontModel(const char *obj_filename, const char *texture_filename, enum faceType face_type, int texture_size) {
    struct model model = {};

    // NOTE: Objects bigger than the constants are undefined behavior
    model.vertices = malloc(MAX_OBJ_VERTICES * sizeof(struct vector));
    model.faces = malloc(MAX_OBJ_FACES * sizeof(struct face));
    model.normals = malloc(MAX_OBJ_VERTICES * sizeof(struct normal));
    model.texture_coords = malloc(MAX_OBJ_VERTICES * sizeof(struct textureCoord));
    model.face_type = face_type;

    if (face_type == VERTEX_ALL || face_type == VERTEX_ALL_ALPHA) {
        model.texture_id = loadTexture(texture_filename, face_type, texture_size);
    }

    FILE *f = fopen(obj_filename, "r");
    char type[40] = {};
    while ((fscanf(f, " %s", type)) != EOF) {
        if (!strcmp(type, "v")) {
            struct vector v = {};

            fscanf(f, " %f %f %f", &v.x, &v.y, &v.z);
            model.vertices[++model.num_vertices] = v;
        } else if (!strcmp(type, "vn")) {
            struct normal n = {};

            fscanf(f, " %f %f %f", &n.x, &n.y, &n.z);
            model.normals[++model.num_normals] = n;
        } else if (!strcmp(type, "vt")) {
            struct textureCoord t = {};

            fscanf(f, " %f %f", &t.x, &t.y);
            t.y = 1 - t.y;
            model.texture_coords[++model.num_texture_coords] = t;
        } else if (!strcmp(type, "f")) {
            struct face face = {};

            if (face_type == VERTEX_ONLY) {
                fscanf(f, " %d %d %d", &face.vertices[0], &face.vertices[1], &face.vertices[2]);
            } else if (face_type == VERTEX_NORMAL) {
                fscanf(f, " %d//%d %d//%d %d//%d", &face.vertices[0], &face.normals[0], &face.vertices[1],
                        &face.normals[1], &face.vertices[2], &face.normals[2]);
            } else if (face_type == VERTEX_ALL || face_type == VERTEX_ALL_ALPHA || face_type == VERTEX_TEXTURE) {
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
    fclose(f);

    model.vertices = realloc(model.vertices, (model.num_vertices+1) * sizeof(struct vector));
    model.faces = realloc(model.faces, (model.num_faces+1) * sizeof(struct face));
    model.normals = realloc(model.normals, (model.num_normals+1) * sizeof(struct normal));
    model.texture_coords = realloc(model.texture_coords, (model.num_texture_coords+1) * sizeof(struct textureCoord));

    return model;
}
