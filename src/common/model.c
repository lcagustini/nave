void destroyModel(int model) {
    free(loaded_models[model].vertices);
    free(loaded_models[model].normals);
    free(loaded_models[model].texture_coords);

    if (loaded_models[model].face_type == VERTEX_ALL || loaded_models[model].face_type == VERTEX_ALL_ALPHA) {
        glDeleteTextures(1, &loaded_models[model].texture_id);
    }
}

void loadModel(const char *obj_filename, const char *texture_filename, enum faceType face_type, int texture_size) {
    struct {
        struct vector *vertices;
        int num_vertices;
        struct face *faces;
        int num_faces;
        struct normal *normals;
        int num_normals;
        struct textureCoord *texture_coords;
        int num_texture_coords;
    } file = {0};

    file.vertices = malloc(MAX_OBJ_SIZE * sizeof(struct vector));
    file.faces = malloc(MAX_OBJ_SIZE * sizeof(struct face));
    file.normals = malloc(MAX_OBJ_SIZE * sizeof(struct normal));
    file.texture_coords = malloc(MAX_OBJ_SIZE * sizeof(struct textureCoord));

    FILE *f = fopen(obj_filename, "r");
    char buffer[40] = {0};
    while ((fscanf(f, " %s", buffer)) != EOF) {
        if (!strcmp(buffer, "v")) {
            struct vector v = {};
            fscanf(f, " %f %f %f", &v.x, &v.y, &v.z);
            file.vertices[++file.num_vertices] = v;
        }
        else if (!strcmp(buffer, "vn")) {
            struct normal n = {};
            fscanf(f, " %f %f %f", &n.x, &n.y, &n.z);
            file.normals[++file.num_normals] = n;
        }
        else if (!strcmp(buffer, "vt")) {
            struct textureCoord t = {};
            fscanf(f, " %f %f", &t.x, &t.y);
            t.y = 1 - t.y;
            file.texture_coords[++file.num_texture_coords] = t;
        }
        else if (!strcmp(buffer, "f")) {
            struct face face = {};
            switch (face_type) {
                case VERTEX_ONLY:
                    fscanf(f, " %d %d %d",
                            &face.vertices[0], &face.vertices[1], &face.vertices[2]
                          );
                    break;
                case VERTEX_NORMAL:
                    fscanf(f, " %d//%d %d//%d %d//%d",
                            &face.vertices[0], &face.normals[0], &face.vertices[1],
                            &face.normals[1], &face.vertices[2], &face.normals[2]
                          );
                    break;
                case VERTEX_ALL:
                case VERTEX_ALL_ALPHA:
                case VERTEX_TEXTURE:
                    fscanf(f, " %d/%d/%d %d/%d/%d %d/%d/%d",
                            &face.vertices[0], &face.texture_coords[0], &face.normals[0],
                            &face.vertices[1], &face.texture_coords[1], &face.normals[1],
                            &face.vertices[2], &face.texture_coords[2], &face.normals[2]
                          );
                    break;
                default:
                    assert(false);
            }

            file.faces[file.num_faces++] = face;
        }
    }
    fclose(f);

    struct model *model = &loaded_models[loaded_models_n];

    if (face_type == VERTEX_ALL || face_type == VERTEX_ALL_ALPHA) {
        model->texture_id = loadTexture(texture_filename, face_type, texture_size);
    }
    model->face_type = face_type;
    model->num_faces = file.num_faces;
    model->vertices = malloc(model->num_faces*3*sizeof(struct vector));
    model->normals = malloc(model->num_faces*3*sizeof(struct normal));
    model->texture_coords = malloc(model->num_faces*3*sizeof(struct textureCoord));

    int k = 0;
    for (int i = 0; i < file.num_faces; i++) {
        model->vertices[k] = file.vertices[file.faces[i].vertices[0]];
        model->vertices[k+1] = file.vertices[file.faces[i].vertices[1]];
        model->vertices[k+2] = file.vertices[file.faces[i].vertices[2]];

        model->normals[k] = file.normals[file.faces[i].normals[0]];
        model->normals[k+1] = file.normals[file.faces[i].normals[1]];
        model->normals[k+2] = file.normals[file.faces[i].normals[2]];

        model->texture_coords[k] = file.texture_coords[file.faces[i].texture_coords[0]];
        model->texture_coords[k+1] = file.texture_coords[file.faces[i].texture_coords[1]];
        model->texture_coords[k+2] = file.texture_coords[file.faces[i].texture_coords[2]];

        k += 3;
    }
    loaded_models_n++;

    free(file.vertices);
    free(file.faces);
    free(file.normals);
    free(file.texture_coords);
}
