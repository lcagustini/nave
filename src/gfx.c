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

void drawModel(int model) {
    glBindTexture(GL_TEXTURE_2D, loaded_models[model].texture_id);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexCoordPointer(2, GL_FLOAT, 0, loaded_models[model].draw_texture_coords);
    glNormalPointer(GL_FLOAT, 0, loaded_models[model].draw_normals);
    glVertexPointer(3, GL_FLOAT, 0, loaded_models[model].draw_vertices);

    glDrawArrays(GL_TRIANGLES, 0, loaded_models[model].num_draw_vertices);

    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint loadTexture(const char *texture_filename, enum faceType face_type, int texture_size) {
    GLuint texture_id;
    FILE *f = fopen(texture_filename, "rb");

    struct stat st;
    stat(texture_filename, &st);

    char *sur = malloc(st.st_size);
    fread(sur, 1, st.st_size, f);
    fclose(f);

    assert(sur);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCompressedTexImage2D(GL_TEXTURE_2D,  /* This must be GL_TEXTURE_2D */
            0,             /* 0 = Texture does not contain Mip-Maps */
            face_type == VERTEX_ALL ? GL_UNSIGNED_SHORT_5_6_5_VQ_TWID : GL_UNSIGNED_SHORT_1_5_5_5_VQ_TWID,        /* GL Compressed Color Format */
            texture_size,           /* Texture Width */
            texture_size,           /* Texture Height */
            0,             /* This bit must be set to 0 */
            st.st_size, /* Compressed Texture Size*/
            sur);       /* Address of texture data in RAM: OpenGL will load the texture into VRAM for you.
                           Because of this, make sure to call glDeleteTextures() as needed, as that will
                           free the VRAM allocated for the texture. */
    glBindTexture(GL_TEXTURE_2D, 0);

    free(sur);

    return texture_id;
}
