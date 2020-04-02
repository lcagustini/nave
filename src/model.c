struct model loadWavefrontModel(const char *obj_filename, const char *texture_filename, enum faceType face_type, int texture_size) {
    struct model model = {};

    // NOTE: Objects bigger than the constants are undefined behavior
    model.vertices = (struct vector *) malloc(MAX_OBJ_VERTICES * sizeof(struct vector));
    model.faces = (struct face *) malloc(MAX_OBJ_FACES * sizeof(struct face));
    model.normals = (struct normal *) malloc(MAX_OBJ_VERTICES * sizeof(struct normal));
    model.texture_coords = (struct textureCoord *) malloc(MAX_OBJ_VERTICES * sizeof(struct textureCoord));

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
