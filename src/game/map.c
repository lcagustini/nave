struct model mergeMeshes(struct model in1, struct model in2, struct vector offset) {
    memcpy(&in1.vertices[in1.num_vertices+1], in2.vertices, (in2.num_vertices+1)*sizeof(struct vector));
    for (int i = 0; i <= in2.num_vertices; i++) {
        in1.vertices[in1.num_vertices+1+i] = vectorAdd(in1.vertices[in1.num_vertices+1+i], offset);
    }

    memcpy(&in1.faces[in1.num_faces+1], in2.faces, (in2.num_faces+1)*sizeof(struct face));
    for (int i = 0; i <= in2.num_faces; i++) {
        struct face *cur = &in1.faces[in1.num_faces+1+i];
        for (int j = 0; j < 3; j++) {
            cur->vertices[j] += in1.num_vertices+1;
            cur->normals[j] += in1.num_normals+1;
            cur->texture_coords[j] += in1.num_texture_coords+1;
        }
    }

    memcpy(&in1.normals[in1.num_normals+1], in2.normals, (in2.num_normals+1)*sizeof(struct normal));
    memcpy(&in1.texture_coords[in1.num_texture_coords+1], in2.texture_coords, (in2.num_texture_coords+1)*sizeof(struct textureCoord));

    in1.num_vertices += in2.num_vertices+1;
    in1.num_faces += in2.num_faces+1;
    in1.num_normals += in2.num_normals+1;
    in1.num_texture_coords += in2.num_texture_coords+1;

    return in1;
}

void searchGrid(int grid[MAP_SIZE][MAP_SIZE], int x, int y) {
    if (grid[x][y] == 0 || grid[x][y] == 2) return;

    grid[x][y] = 2;
    if (x < MAP_SIZE-1) searchGrid(grid, x + 1, y);
    if (y < MAP_SIZE-1) searchGrid(grid, x, y + 1);
    if (x > 0) searchGrid(grid, x - 1, y);
    if (y > 0) searchGrid(grid, x, y - 1);
}

void generateMap(const char *texture_filename) {
    bool done = false;

    int grid[MAP_SIZE][MAP_SIZE];

    while (!done) {
        memset(grid, 0, sizeof(grid));

        for (int i = 1; i <= MAP_WORKERS; i++) {
            int x = 1 + (rand() % (MAP_SIZE-2));
            int y = 1 + (rand() % (MAP_SIZE-2));
            for (int j = 0; j < MAP_WORKTIME; j++) {
                grid[x][y] = 1;
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
                if (grid[i][j] == 1) {
                    done = false;
                }
            }
        }
    }

    struct model final_model;
    final_model.vertices = malloc(6000 * sizeof(struct vector));
    final_model.num_vertices = 0;
    final_model.faces = malloc(6000 * sizeof(struct face));
    final_model.num_faces = 0;
    final_model.normals = malloc(6000 * sizeof(struct normal));
    final_model.num_normals = 0;
    final_model.texture_coords = malloc(6000 * sizeof(struct textureCoord));
    final_model.num_texture_coords = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            struct vector pos = {2*i, 2*j, 0};
            final_model = mergeMeshes(final_model, loaded_models[grid[i][j] == 2 ? 1 : 2], pos); //Model number from order loaded before calling generateMap
        }
    }
    final_model.vertices = realloc(final_model.vertices, (final_model.num_vertices+1) * sizeof(struct vector));
    final_model.faces = realloc(final_model.faces, (final_model.num_faces+1) * sizeof(struct face));
    final_model.normals = realloc(final_model.normals, (final_model.num_normals+1) * sizeof(struct normal));
    final_model.texture_coords = realloc(final_model.texture_coords, (final_model.num_texture_coords+1) * sizeof(struct textureCoord));

    final_model.texture_id = loadTexture(texture_filename, VERTEX_ALL, 512);

    destroyModel(--loaded_models_n);
    destroyModel(--loaded_models_n);
    loaded_models[loaded_models_n++] = final_model;
    cur_map.model = loaded_models_n-1;
}
