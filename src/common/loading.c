int updateLoading() {
    while (true) {
        if (loading_progress == 100) return 0;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0, 0, -1);

        glBegin(GL_QUADS);

        if (loading_progress < 25) glColor3f(0.0f, 0.0f, 0.0f);
        else glColor3f(0.8f, 0.1f, 0.4f);
        glVertex3f(-1.0f, -1.0f, 0.0f);

        if (loading_progress < 50) glColor3f(0.0f, 0.0f, 0.0f);
        else glColor3f(0.8f, 0.1f, 0.4f);
        glVertex3f(-1.0f, 1.0f, 0.0f);

        if (loading_progress < 75) glColor3f(0.0f, 0.0f, 0.0f);
        else glColor3f(0.8f, 0.1f, 0.4f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        if (loading_progress < 90) glColor3f(0.0f, 0.0f, 0.0f);
        else glColor3f(0.8f, 0.1f, 0.4f);
        glVertex3f(1.0f, -1.0f, 0.0f);

        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glutSwapBuffers();
    }
}

void startLoading() {
    loading_progress = 0;
    thrd_create(&loading_thread, updateLoading, NULL);
}

void endLoading() {
    int res;
    loading_progress = 100;
    thrd_join(loading_thread, &res);
}
