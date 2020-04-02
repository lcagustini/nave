void gfxInit() {
    glKosInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    GLfloat light_position[] = { 1.4, 1.0, 1.6, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void drawSphere(float r, int UResolution, int VResolution) {
    float startU = 0;
    float startV = 0;
    float endU = M_PI*2;
    float endV = M_PI;
    float stepU = (endU-startU)/UResolution; // step size between U-points on the grid
    float stepV = (endV-startV)/VResolution; // step size between V-points on the grid
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < UResolution; i++) { // U-points
        for (int j = 0; j < VResolution; j++) { // V-points
            float u = i*stepU + startU;
            float v = j*stepV + startV;
            float un = (i+1 == UResolution) ? endU : (i+1)*stepU+startU;
            float vn = (j+1 == VResolution) ? endV : (j+1)*stepV+startV;
            // Find the four points of the grid
            // square by evaluating the parametric
            // surface function
            float p0_x, p0_y, p0_z;
            p0_x = cos(u)*sin(v)*r;
            p0_y = cos(v)*r;
            p0_z = sin(u)*sin(v)*r;
            float p1_x, p1_y, p1_z;
            p1_x = cos(u)*sin(vn)*r;
            p1_y = cos(vn)*r;
            p1_z = sin(u)*sin(vn)*r;
            float p2_x, p2_y, p2_z;
            p2_x = cos(un)*sin(v)*r;
            p2_y = cos(v)*r;
            p2_z = sin(un)*sin(v)*r;
            float p3_x, p3_y, p3_z;
            p3_x = cos(un)*sin(vn)*r;
            p3_y = cos(vn)*r;
            p3_z = sin(un)*sin(vn)*r;

            // NOTE: For spheres, the normal is just the normalized
            // version of each vertex point; this generally won't be the case for
            // other parametric surfaces.
            // Output the first triangle of this grid square
            glColor3f(1.0, 0, 0);
            glVertex3f(p0_x, p0_y, p0_z);
            glColor3f(1.0, 0, 0);
            glVertex3f(p2_x, p2_y, p2_z);
            glColor3f(1.0, 0, 0);
            glVertex3f(p1_x, p1_y, p1_z);
            // Output the other triangle of this grid square
            glColor3f(1.0, 0, 0);
            glVertex3f(p3_x, p3_y, p3_z);
            glColor3f(1.0, 0, 0);
            glVertex3f(p1_x, p1_y, p1_z);
            glColor3f(1.0, 0, 0);
            glVertex3f(p2_x, p2_y, p2_z);
        }
    }
    glColor3f(1.0, 1.0, 1.0);
    glEnd();
}

void drawModel(struct model model) {
    glBindTexture(GL_TEXTURE_2D, model.texture_id);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; i < model.num_faces; i++) {
        struct face f = model.faces[i];

        glBegin(GL_TRIANGLES);

        for (int j = 0; j <= 2; j++) {
            if (model.num_normals) {
                struct normal n = model.normals[f.normals[j]];
                glNormal3f(n.x, n.y, n.z);
            }

            if (model.num_texture_coords) {
                struct textureCoord t = model.texture_coords[f.texture_coords[j]];
                glTexCoord2f(t.x, t.y);
            }

            struct vector v = model.vertices[f.vertices[j]];

            glVertex3f(v.x, v.y, v.z);
        }

        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-entities[PLAYER_ID].pos.x, -entities[PLAYER_ID].pos.y, -14.0f);

    glPushMatrix();
    glScalef(MAP_SCALE, MAP_SCALE, MAP_SCALE);
    drawModel(cur_map.model);
    glPopMatrix();

    for (int i = 0; i < entities_size; i++) {
        glPushMatrix();
        glTranslatef(entities[i].pos.x, entities[i].pos.y, entities[i].pos.z);
        // From AllegroGL`s math.c
        glRotatef((2*acos(entities[i].rotation.w)) * 180 / M_PI,
                entities[i].rotation.x,
                entities[i].rotation.y,
                entities[i].rotation.z);
        glScalef(0.24, 0.24, 0.24);
        drawModel(entities[i].model);
        glPopMatrix();
    }
}
