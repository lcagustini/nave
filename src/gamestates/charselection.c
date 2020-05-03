void runCharSelection() {
    char buffer1[40];
    char buffer2[40];

    startLoading();

    mountRomdisk("/cd/charselect_romdisk.img", "/charselect");

    assert(loaded_models_n == 0);

    loadModel("/charselect/background.obj", "/charselect/background.vq", VERTEX_ALL, 512);

    umountRomdisk("/charselect");

    mountRomdisk("/cd/players_romdisk.img", "/game");

    struct dirent *ep;
    DIR *dp = opendir ("/game");
    int player_count = 0;
    while ((ep = readdir (dp))) player_count++;
    player_count = (player_count-2)/3;
    closedir (dp);

    for (int i = 1; i <= player_count; i++) {
        sprintf(buffer1, "/game/player%d.obj", i);
        sprintf(buffer2, "/game/player%d.vq", i);
        loadModel(buffer1, buffer2, VERTEX_ALL, 256);
    }

    umountRomdisk("/game");

    endLoading();

    float rotation = 0;
    selected_player = 1;

    global_timer = 0;
    while (cur_gs == GS_CHARSELECT) {
        global_timer++;

        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (global_timer > INPUT_DELAY) {
            if (state->buttons & CONT_DPAD_LEFT) {
                selected_player--;
                global_timer = 0;
            }
            if (state->buttons & CONT_DPAD_RIGHT) {
                selected_player++;
                global_timer = 0;
            }
            if (state->buttons & CONT_START) cur_gs = GS_GAME;
        }
        if (selected_player < 1) {
            selected_player = player_count;
        }
        else if (selected_player > player_count) {
            selected_player = 1;
        }

        rotation += 0.4;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -1.5f);

        glPushMatrix();
        drawModel(0);
        glPopMatrix();

        for (int i = 1; i <= player_count; i++) {
            glPushMatrix();
            glTranslatef((i-selected_player)/2.0f, 0, 0);
            glRotatef(rotation, 0, 1, 0);
            glScalef(0.13f, 0.13f, 0.13f);
            drawModel(i);
            glPopMatrix();
        }

        glutSwapBuffers();
    }

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;

    entities_size = 0;
    cur_map.level = 1;
}
