void runCharSelection() {
    mountRomdisk("/cd/charselect_romdisk.img", "/charselect");

    assert(loaded_models_n == 0);

    loadModel("/charselect/background.obj", "/charselect/background.vq", VERTEX_ALL, 512);

    umountRomdisk("/charselect");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    global_timer = 0;
    while (cur_gs == GS_CHARSELECT) {
        global_timer++;

        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (global_timer > INPUT_DELAY && state->buttons & CONT_START) {
            cur_gs = GS_GAME;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -1.5f);

        glPushMatrix();
        drawModel(0);
        glPopMatrix();

        glutSwapBuffers();
    }
    glDisable(GL_BLEND);

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;

    entities_size = 0;
    loadPlayer();
    cur_map.level = 1;
}
