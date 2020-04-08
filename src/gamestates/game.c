void runGame() {
    entities[PLAYER_ID].pos.z = 15;
    entities[PLAYER_ID].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[PLAYER_ID].health = 10;
    entities[PLAYER_ID].scale = 0.24;
    entities[PLAYER_ID].speed = 0.05;
    entities[PLAYER_ID].hit_radius = 0.9375;
    entities[PLAYER_ID].type = ET_PLAYER;

    entities[1].pos.x = 1;
    entities[1].pos.y = -3;
    entities[1].pos.z = 15;
    entities[1].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[1].health = 3;
    entities[1].scale = 0.24;
    entities[1].speed = 0.02;
    entities[1].hit_radius = 0.9375;
    entities[1].damage = 1;
    entities[1].type = ET_ENEMY_BASIC;

    entities[2].pos.x = 1;
    entities[2].pos.y = 3;
    entities[2].pos.z = 15;
    entities[2].model = loadWavefrontModel("/rd/player.obj", "/rd/player.vq", VERTEX_ALL, 256);
    entities[2].health = 3;
    entities[2].scale = 0.24;
    entities[2].speed = 0.02;
    entities[2].hit_radius = 0.9375;
    entities[2].damage = 1;
    entities[2].type = ET_ENEMY_BASIC;

    entities_size = 3;

    cur_map.model = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 512);

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
    for (int i = 0; i < entities_size; i++) {
        destroyModel(entities[i].model);
        memset(&entities[i], 0, sizeof(struct entity));
    }
    entities_size = 0;
    destroyModel(cur_map.model);
}
