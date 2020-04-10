void searchGrid(int x, int y) {
    if (cur_map.grid[x][y] == 0 || cur_map.grid[x][y] == 2) return;

    cur_map.grid[x][y] = 2;
    if (x < MAP_SIZE-1) searchGrid(x + 1, y);
    if (y < MAP_SIZE-1) searchGrid(x, y + 1);
    if (x > 0) searchGrid(x - 1, y);
    if (y > 0) searchGrid(x, y - 1);
}

void generateMap() {
    bool done = false;

    while (!done) {
        memset(cur_map.grid, 0, sizeof(cur_map.grid));

        for (int i = 1; i <= MAP_WORKERS; i++) {
            int x = 1 + (rand() % (MAP_SIZE-2));
            int y = 1 + (rand() % (MAP_SIZE-2));
            for (int j = 0; j < MAP_WORKTIME; j++) {
                cur_map.grid[x][y] = 1;
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
                if (cur_map.grid[i][j]) {
                    searchGrid(i, j);
                    goto break_search;
                }
            }
        }
break_search:

        done = true;
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (cur_map.grid[i][j] == 1) {
                    done = false;
                }
            }
        }
    }

    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (cur_map.grid[i][j] == 2) cur_map.grid[i][j] = MGC_FLOOR;
            else cur_map.grid[i][j] = MGC_WALL;
        }
    }
}
