void runItemSelection() {
    startLoading();

    mountRomdisk("/cd/itemselect_romdisk.img", "/itemselect");

    assert(loaded_models_n == 0);
    loadModel("/itemselect/background.obj", "/itemselect/background.vq", VERTEX_ALL, 512);

    umountRomdisk("/itemselect");

    global_timer = 0;
    while (cur_gs == GS_ITEMSELECT) {
        global_timer++;

        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (global_timer > INPUT_DELAY) {
            if (state->buttons & CONT_START) cur_gs = GS_GAME;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glutSwapBuffers();
    }
    entities[PLAYER_ID].pos.z += GRAVITY;

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;
}
