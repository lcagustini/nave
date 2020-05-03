void runItemSelection() {
    startLoading();

    mountRomdisk("/cd/itemselect_romdisk.img", "/itemselect");

    assert(loaded_models_n == 0);
    loadModel("/itemselect/background.obj", "/itemselect/background.vq", VERTEX_ALL, 512);

    umountRomdisk("/itemselect");
    mountRomdisk("/cd/items_romdisk.img", "/items");

    for (int i = 0; i < IT_MAX; i++) {
        char buffer1[40], buffer2[40];
        sprintf(buffer1, "/items/%d.obj", i);
        sprintf(buffer2, "/items/%d.vq", i);
        items_models[i] = loaded_models_n;
        loadModel(buffer1, buffer2, VERTEX_ALL, 512);
    }

    umountRomdisk("/items");

    endLoading();

    float rotation = 0;
    int selected_item = 0;

    global_timer = 0;
    while (cur_gs == GS_ITEMSELECT) {
        global_timer++;

        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (global_timer > INPUT_DELAY) {
            if (state->buttons & CONT_DPAD_LEFT) {
                selected_item--;
                global_timer = 0;
            }
            if (state->buttons & CONT_DPAD_RIGHT) {
                selected_item++;
                global_timer = 0;
            }
            if (state->buttons & CONT_START) {
                removeItem(selected_item);
                cur_gs = GS_GAME;
            }
        }
        if (selected_item < 0) {
            selected_item = player_items_n-1;
        }
        else if (selected_item >= player_items_n) {
            selected_item = 0;
        }

        rotation += 0.4;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -1.5f);

        glPushMatrix();
        drawModel(0);
        glPopMatrix();

        for (int i = 0; i < player_items_n; i++) {
            glPushMatrix();
            glTranslatef((i-selected_item)/8.0f, 0, 0);
            glRotatef(rotation, 0, 1, 0);
            glScalef(0.03f, 0.03f, 0.03f);
            drawModel(items_models[player_items[i].item_type]);
            glPopMatrix();
        }

        glutSwapBuffers();
    }
    entities[PLAYER_ID].pos.z = INITIAL_POS_Z;

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;
}
