void getAvailableMapPosition(struct vector *pos) {
    while (true) {
        int gx = 1 + (rand() % (MAP_SIZE-2));
        int gy = 1 + (rand() % (MAP_SIZE-2));

        if (cur_map.grid[gx][gy] == MC_FLOOR) {
            pos->x = 2*MAP_SCALE*gx;
            pos->y = 2*MAP_SCALE*gy;
            pos->z = 1.0f;

            return;
        }
    }
}

void searchGrid(int x, int y) {
    switch (cur_map.grid[x][y]) {
        case MC_P_FLOOR:
            cur_map.grid[x][y] = MC_FLOOR;
            break;
        default:
            return;
    }

    if (x < MAP_SIZE-1) searchGrid(x + 1, y);
    if (y < MAP_SIZE-1) searchGrid(x, y + 1);
    if (x > 0) searchGrid(x - 1, y);
    if (y > 0) searchGrid(x, y - 1);
}

void generateMap() {
    bool done = false;

    while (!done) {
        memset(cur_map.grid, MC_WALL, sizeof(cur_map.grid));
        loading_progress = 0;

        for (int i = 1; i <= MAP_WORKERS; i++) {
            int root = (int) fsqrt((float) MAP_WORKERS);
            int x = 0.8*(((1 + (i-1) % root)*MAP_SIZE/root)-1);
            int y = 0.8*(((1 + (i-1) / root)*MAP_SIZE/root)-1);
            for (int j = 0; j < MAP_WORKTIME; j++) {
                cur_map.grid[x][y] = MC_P_FLOOR;
                x = (x + (rand() % 3) -1);
                y = (y + (rand() % 3) -1);
                if (x >= MAP_SIZE-1) x = MAP_SIZE-2;
                if (y >= MAP_SIZE-1) y = MAP_SIZE-2;
                if (x <= 0) x = 1;
                if (y <= 0) y = 1;
            }
        }
        loading_progress = 50;

        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (cur_map.grid[i][j]) {
                    searchGrid(i, j);
                    goto break_search;
                }
            }
        }
break_search:
        loading_progress = 75;

        done = true;
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (cur_map.grid[i][j] > MC_WALL && cur_map.grid[i][j] < MC_FLOOR) {
                    done = false;
                }
            }
        }
    }
    loading_progress = 100;

#if 0
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            printf("%d ", cur_map.grid[i][j]);
        }
        printf("\n");
    }
#endif
}
