void drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-entities[PLAYER_ID].pos.x, -entities[PLAYER_ID].pos.y, -13.5f);

    int gx = 0.5f + (entities[PLAYER_ID].pos.x/(2*MAP_SCALE));
    int gy = 0.5f + (entities[PLAYER_ID].pos.y/(2*MAP_SCALE));

    for (int x = max(gx-10, 0); x < min(gx+11, MAP_SIZE); x++) {
        for (int y = max(gy-7, 0); y < min(gy+8, MAP_SIZE); y++) {
            glPushMatrix();
            glScalef(MAP_SCALE, MAP_SCALE, MAP_SCALE);
            glTranslatef(2*x, 2*y, 0);
            drawModel(cur_map.models[cur_map.grid[x][y]]);
            glPopMatrix();
        }
    }

    for (struct projectile *cur = projectiles; cur; cur = cur->next) {
        glPushMatrix();
        glTranslatef(cur->pos.x, cur->pos.y, cur->pos.z);
        glRotatef(90 + (atan2(cur->vel.y, cur->vel.x) * 180 / M_PI), 0, 0, 1);
        glScalef(cur->scale, cur->scale, cur->scale);
        drawModel(cur->model);
        glPopMatrix();
    }

    for (int i = 0; i < entities_size; i++) {
        glPushMatrix();
        glTranslatef(entities[i].pos.x, entities[i].pos.y, entities[i].pos.z);
        // From AllegroGL`s math.c
        glRotatef((2*acos(entities[i].rotation.w)) * 180 / M_PI,
                entities[i].rotation.x,
                entities[i].rotation.y,
                entities[i].rotation.z);
        glRotatef(90 + (atan2(entities[i].dir.y, entities[i].dir.x) * 180 / M_PI), 0, 0, 1);
        glScalef(entities[i].scale, entities[i].scale, entities[i].scale);
        drawModel(entities[i].model);
        glPopMatrix();
    }
}

void runGame() {
    startLoading();

    char romdisk[40];
    sprintf(romdisk, "/cd/level%d_romdisk.img", cur_map.level);

    mountRomdisk(romdisk, "/game");
    assert(loaded_models_n == 0);

    entities_models[ET_PLAYER] = loaded_models_n;
    entities_models[ET_ENEMY_BASIC] = loaded_models_n;
    loadModel("/game/player.obj", "/game/player.vq", VERTEX_ALL, 256);

    cur_map.models[MC_FLOOR] = loaded_models_n;
    loadModel("/game/floor.obj", "/game/floor.vq", VERTEX_ALL, 128);

    cur_map.models[MC_WALL] = loaded_models_n;
    loadModel("/game/wall.obj", "/game/wall.vq", VERTEX_ALL, 128);

    generateMap();

    assert(entities_size == 1);

    getAvailableMapPosition(&entities[PLAYER_ID].pos);
    entities[PLAYER_ID].model = entities_models[entities[PLAYER_ID].type];

    loadEntitiesFromFile("/game/enemies.ent");

    umountRomdisk("/game");

    endLoading();

    while (cur_gs == GS_GAME) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        getInput(state);

        for (struct projectile *cur = projectiles; cur; cur = cur->next) {
            doProjectileFrame(cur);
        }
        for (int i = 0; i < entities_size; i++) {
            doEntityFrame(i);
            i = checkDeadEntity(i);
        }

        if (entities_size == 1) {
            cur_map.level++;
            if (cur_map.level > 2) cur_gs = GS_TITLE; //TODO: goto ending
            break;
        }

        drawFrame();

        glutSwapBuffers();
    }

    while (projectiles) {
        deleteProjectile(projectiles);
    }
    entities_size = 1;

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;
}
