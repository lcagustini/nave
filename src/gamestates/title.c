void runTitle() {
    mountRomdisk("/cd/title_romdisk.img", "/title");

    assert(loaded_models_n == 0);

    loaded_models[loaded_models_n++] = loadWavefrontModel("/title/map.obj", "/title/map.vq", VERTEX_ALL, 512);
    loaded_models[loaded_models_n++] = loadWavefrontModel("/title/title.obj", "/title/title.vq", VERTEX_ALL_ALPHA, 256);

    umountRomdisk("/title");

    GLfloat light_position[] = { 3.4, 1.0, 3.6, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float rotation = 0;

    while (cur_gs == GS_TITLE) {
        maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

        if (state->buttons & CONT_START) {
            cur_gs = GS_GAME;
        }

        rotation += 0.025;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -14.0f);

        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0, 0, 8.0f);
        drawModel(1);
        glPopMatrix();

        glEnable(GL_LIGHTING);
        glPushMatrix();
        glRotatef(rotation, 0, 0, 1);
        glScalef(0.25, 0.25, 0.25);
        drawModel(0);
        glPopMatrix();

        glutSwapBuffers();
    }
    glDisable(GL_BLEND);

    for (int i = 0; i < loaded_models_n; i++) {
        destroyModel(i);
    }
    loaded_models_n = 0;
}
