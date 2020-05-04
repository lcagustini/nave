void getAvailableMapPosition(struct vector *pos) {
    while (true) {
        int gx = 1 + (rand() % (cur_map.size-2));
        int gy = 1 + (rand() % (cur_map.size-2));

        if (cur_map.grid[gx][gy] == MC_FLOOR) {
            pos->x = 2*MAP_SCALE*gx;
            pos->y = 2*MAP_SCALE*gy;
            pos->z = 0.8f;

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

    if (x < cur_map.size-1) searchGrid(x + 1, y);
    if (y < cur_map.size-1) searchGrid(x, y + 1);
    if (x > 0) searchGrid(x - 1, y);
    if (y > 0) searchGrid(x, y - 1);
}

void generateMap(int size) {
    bool done = false;

    cur_map.size = size;
    int map_workers = (int) (fsqrt(cur_map.size/2.0f) + 0.5f);
    int map_workers_time = (8*cur_map.size)/3;
    printf("size %d map_workers %d map_workers_time %d\n", cur_map.size, map_workers*map_workers, map_workers_time);
    while (!done) {
        loading_progress = 0;
        memset(cur_map.grid, MC_WALL, sizeof(cur_map.grid));

        for (int i = 1; i <= map_workers*map_workers; i++) {
            int og_x = 0.8*(((1 + (i-1) % map_workers)*cur_map.size/map_workers)-1);
            int og_y = 0.8*(((1 + (i-1) / map_workers)*cur_map.size/map_workers)-1);

            int x = og_x;
            int y = og_y;
            for (int j = 0; j < 40 + cur_map.size/12; j++) {
                if (x >= cur_map.size-1) x = cur_map.size-2;
                if (y >= cur_map.size-1) y = cur_map.size-2;
                if (x <= 0) x = 1;
                if (y <= 0) y = 1;
                cur_map.grid[x][y] = MC_P_FLOOR;
                x = (x + (rand() % 3) -1);
                y = (y + (rand() % 3) -1);
            }

            x = og_x;
            y = og_y;
            for (int dir = 0; dir < 4; dir++) {
                for (int j = 0; j < 5; j++) {
                    if (x >= cur_map.size-1) x = cur_map.size-2;
                    if (y >= cur_map.size-1) y = cur_map.size-2;
                    if (x <= 0) x = 1;
                    if (y <= 0) y = 1;
                    cur_map.grid[x][y] = MC_P_FLOOR;
                    switch(dir) {
                        case 0:
                            x++;
                            break;
                        case 1:
                            y++;
                            break;
                        case 2:
                            x--;
                            break;
                        case 3:
                            y--;
                            break;
                    }
                }
            }
        }
        loading_progress = 50;

        for (int i = 0; i < cur_map.size; i++) {
            for (int j = 0; j < cur_map.size; j++) {
                if (cur_map.grid[i][j]) {
                    searchGrid(i, j);
                    goto break_search;
                }
            }
        }
break_search:
        loading_progress = 75;

        done = true;
        for (int i = 0; i < cur_map.size; i++) {
            for (int j = 0; j < cur_map.size; j++) {
                if (cur_map.grid[i][j] > MC_WALL && cur_map.grid[i][j] < MC_FLOOR) {
                    done = false;
                    goto break_test;
                }
            }
        }
break_test:;
    }
    loading_progress = 100;

#if 0
    for (int i = 0; i < cur_map.size; i++) {
        for (int j = 0; j < cur_map.size; j++) {
            printf("%d ", cur_map.grid[i][j]);
        }
        printf("\n");
    }
#endif
}
