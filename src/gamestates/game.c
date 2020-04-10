void drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-entities[PLAYER_ID].pos.x, -entities[PLAYER_ID].pos.y, -14.0f);

    for (int x = 0; x < MAP_SIZE; x++) {
        for (int y = 0; y < MAP_SIZE; y++) {
            glPushMatrix();
            glTranslatef(MAP_TRANSLATE_FACTOR*x, MAP_TRANSLATE_FACTOR*y, 0);
            glScalef(MAP_SCALE, MAP_SCALE, MAP_SCALE);
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
    mountRomdisk("/cd/level1_romdisk.img", "/game");
    assert(loaded_models_n == 0);

    loaded_models[loaded_models_n++] = loadWavefrontModel("/game/player.obj", "/game/player.vq", VERTEX_ALL, 256);
    loaded_models[loaded_models_n++] = loadWavefrontModel("/game/floor.obj", "/game/map.vq", VERTEX_ALL, 512);
    loaded_models[loaded_models_n++] = loadWavefrontModel("/game/wall.obj", "/game/map.vq", VERTEX_ALL, 512);

    umountRomdisk("/game");

    GLfloat light_position[] = { 3.4, 1.0, 3.6, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    entities[PLAYER_ID].pos.x = 5;
    entities[PLAYER_ID].pos.y = 5;
    entities[PLAYER_ID].pos.z = 15;
    entities[PLAYER_ID].model = 0;
    entities[PLAYER_ID].health = 10;
    entities[PLAYER_ID].scale = 0.24;
    entities[PLAYER_ID].speed = 0.05;
    entities[PLAYER_ID].hit_radius = 0.9375;
    entities[PLAYER_ID].type = ET_PLAYER;

#if 0
    entities[1].pos.x = 1;
    entities[1].pos.y = -3;
    entities[1].pos.z = 15;
    entities[1].model = 0;
    entities[1].health = 3;
    entities[1].scale = 0.24;
    entities[1].speed = 0.02;
    entities[1].hit_radius = 0.9375;
    entities[1].damage = 1;
    entities[1].type = ET_ENEMY_BASIC;

    entities[2].pos.x = 1;
    entities[2].pos.y = 3;
    entities[2].pos.z = 15;
    entities[2].model = 0;
    entities[2].health = 3;
    entities[2].scale = 0.24;
    entities[2].speed = 0.02;
    entities[2].hit_radius = 0.9375;
    entities[2].damage = 1;
    entities[2].type = ET_ENEMY_BASIC;
#endif
    entities_size = 1;

    cur_map.models[MGC_FLOOR] = 1;
    cur_map.models[MGC_WALL] = 2;

    generateMap();

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

        drawFrame();

        glutSwapBuffers();
    }

    while (projectiles) {
        deleteProjectile(projectiles);
    }
    entities_size = 0;

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;
}
