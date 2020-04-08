void runTitle() {
    struct model map = loadWavefrontModel("/rd/map.obj", "/rd/map.vq", VERTEX_ALL, 512);
    float rotation = 0;

    while (cur_gs == GS_TITLE) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (state->buttons & CONT_START) {
            cur_gs = GS_GAME;
        }

        rotation += 0.001;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -14.0f);

        glPushMatrix();
        glRotatef(rotation, 0, 0, 1);
        glScalef(MAP_SCALE, MAP_SCALE, MAP_SCALE);
        drawModel(map);
        glPopMatrix();

        glutSwapBuffers();
    }

    destroyModel(map);
}
