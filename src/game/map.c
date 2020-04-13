void getAvailableMapPosition(float *x, float *y) {
    while (true) {
        int gx = 1 + (rand() % (MAP_SIZE-2));
        int gy = 1 + (rand() % (MAP_SIZE-2));

        if (cur_map.grid[gx][gy] == MC_FLOOR) {
            *x = 2*MAP_SCALE*gx;
            *y = 2*MAP_SCALE*gy;

            return;
        }
    }
}

void removeDuplicateVertices(int model) {
    struct model *cur = &loaded_models[model];

    for (int i = 0; i < cur->num_vertices; i++) {
        for (int j = i+1; j < cur->num_vertices; j++) {
            if (vectorLen(vectorSubtract(cur->vertices[i], cur->vertices[j])) <= 0.1f) {
                for (int k = 0; k < cur->num_faces; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (cur->faces[k].vertices[l] == j) {
                            cur->faces[k].vertices[l] = i;
                        }
                        if (cur->faces[k].vertices[l] == cur->num_vertices) {
                            cur->faces[k].vertices[l] = j;
                        }
                    }
                }

                cur->vertices[j] = cur->vertices[cur->num_vertices];
                cur->num_vertices--;
                j--;
            }
        }
    }
    cur->vertices = realloc(cur->vertices, (cur->num_vertices+1) * sizeof(struct vector));
}

void mergeMeshes(struct model *in1, struct model in2, struct vector offset) {
    memcpy(&in1->vertices[in1->num_vertices], in2.vertices, (in2.num_vertices+1)*sizeof(struct vector));
    for (int i = 0; i <= in2.num_vertices; i++) {
        in1->vertices[in1->num_vertices+i] = vectorAdd(in1->vertices[in1->num_vertices+i], offset);
    }

    memcpy(&in1->faces[in1->num_faces], in2.faces, in2.num_faces*sizeof(struct face));
    for (int i = 0; i < in2.num_faces; i++) {
        struct face *cur = &in1->faces[in1->num_faces+i];
        for (int j = 0; j < 3; j++) {
            cur->vertices[j] += in1->num_vertices;
            cur->normals[j] += in1->num_normals;
            cur->texture_coords[j] += in1->num_texture_coords;
        }
    }

    memcpy(&in1->normals[in1->num_normals], in2.normals, (in2.num_normals+1)*sizeof(struct normal));
    memcpy(&in1->texture_coords[in1->num_texture_coords], in2.texture_coords, (in2.num_texture_coords+1)*sizeof(struct textureCoord));

    in1->num_vertices += in2.num_vertices+1;
    in1->num_faces += in2.num_faces;
    in1->num_normals += in2.num_normals+1;
    in1->num_texture_coords += in2.num_texture_coords+1;
}

void searchGrid(int grid[MAP_SIZE][MAP_SIZE], int x, int y) {
    switch (grid[x][y]) {
        case MC_P_FLOOR:
            grid[x][y] = MC_FLOOR;
            break;
        default:
            return;
    }

    if (x < MAP_SIZE-1) searchGrid(grid, x + 1, y);
    if (y < MAP_SIZE-1) searchGrid(grid, x, y + 1);
    if (x > 0) searchGrid(grid, x - 1, y);
    if (y > 0) searchGrid(grid, x, y - 1);
}

void generateMap(const char *texture_filename, int texture_size) {
    bool done = false;

    int grid[MAP_SIZE][MAP_SIZE];

    while (!done) {
        memset(grid, MC_WALL, sizeof(grid));

        for (int i = 1; i <= MAP_WORKERS; i++) {
            int root = (int) fsqrt((float) MAP_WORKERS);
            int x = 0.8*(((1 + (i-1) % root)*MAP_SIZE/root)-1);
            int y = 0.8*(((1 + (i-1) / root)*MAP_SIZE/root)-1);
            for (int j = 0; j < MAP_WORKTIME; j++) {
                grid[x][y] = MC_P_FLOOR;
                x = (x + (rand() % 3) -1);
                y = (y + (rand() % 3) -1);
                if (x >= MAP_SIZE-1) x = MAP_SIZE-2;
                if (y >= MAP_SIZE-1) y = MAP_SIZE-2;
                if (x <= 0) x = 1;
                if (y <= 0) y = 1;
            }
        }

        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (grid[i][j]) {
                    searchGrid(grid, i, j);
                    goto break_search;
                }
            }
        }
break_search:

        done = true;
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (grid[i][j] > MC_WALL && grid[i][j] < MC_FLOOR) {
                    done = false;
                }
            }
        }
    }

#if 0
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
#endif

    struct model final_model;
    final_model.vertices = malloc(10*MAX_OBJ_VERTICES * sizeof(struct vector));
    final_model.num_vertices = 0;
    final_model.faces = malloc(10*MAX_OBJ_FACES * sizeof(struct face));
    final_model.num_faces = 0;
    final_model.normals = malloc(10*MAX_OBJ_VERTICES * sizeof(struct normal));
    final_model.num_normals = 0;
    final_model.texture_coords = malloc(10*MAX_OBJ_VERTICES * sizeof(struct textureCoord));
    final_model.num_texture_coords = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (grid[i][j] == MC_WALL) {
                struct vector pos = {2*i, 2*j, 0};
                mergeMeshes(&final_model, loaded_models[2], pos); //Model number from order loaded before calling generateMap
            }
        }
    }
    for (int i = 0; i <= loaded_models[1].num_vertices; i++) {
        loaded_models[1].vertices[i] = vectorScale(MAP_SIZE-1, loaded_models[1].vertices[i]); //Model number from order loaded before calling generateMap
    }
    struct vector floor_offset = {MAP_SIZE, MAP_SIZE, 0};
    mergeMeshes(&final_model, loaded_models[1], floor_offset);

    final_model.vertices = realloc(final_model.vertices, (final_model.num_vertices+1) * sizeof(struct vector));
    final_model.faces = realloc(final_model.faces, (final_model.num_faces+1) * sizeof(struct face));
    final_model.normals = realloc(final_model.normals, (final_model.num_normals+1) * sizeof(struct normal));
    final_model.texture_coords = realloc(final_model.texture_coords, (final_model.num_texture_coords+1) * sizeof(struct textureCoord));

    final_model.texture_id = loadTexture(texture_filename, VERTEX_ALL, texture_size);
    final_model.face_type = VERTEX_ALL;

    destroyModel(--loaded_models_n);
    destroyModel(--loaded_models_n);

    loaded_models[loaded_models_n++] = final_model;
    cur_map.model = loaded_models_n-1;
    memcpy(cur_map.grid, grid, sizeof(grid));

    removeDuplicateVertices(cur_map.model);
}
