void entityCollidesWithMap(int id, int dir) {
    struct vector masked_vel = {
        dir == VD_X ? entities[id].vel.x : 0,
        dir == VD_Y ? entities[id].vel.y : 0,
        dir == VD_Z ? entities[id].vel.z : 0
    };
    struct vector next_pos = vectorAdd(entities[id].pos, masked_vel);
    struct vector total_normal = {0};

    struct vector up = {0, 0, 1};

    int gx = 0.5f + (entities[id].pos.x/(2*MAP_SCALE));
    int gy = 0.5f + (entities[id].pos.y/(2*MAP_SCALE));

    for (int x = max(gx-1, 0); x < min(gx+2, MAP_SIZE); x++) {
        for (int y = max(gy-1, 0); y < min(gy+2, MAP_SIZE); y++) {
            struct model *model = &loaded_models[cur_map.models[cur_map.grid[x][y]]];
            struct vector cell_pos = {2*x, 2*y, 0};
            for (int i = 0; i < model->num_faces; i++) {
                struct face *cur = &model->faces[i];

                struct vector vertex0 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[cur->vertices[0]], cell_pos));
                struct vector vertex1 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[cur->vertices[1]], cell_pos));
                struct vector vertex2 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[cur->vertices[2]], cell_pos));

                struct vector v1 = vectorSubtract(vertex2, vertex0);
                struct vector v2 = vectorSubtract(vertex1, vertex0);
                struct vector normal = vectorCross(v1, v2);
                vectorNormalize(&normal);
                if (normal.z < 0) {
                    normal = vectorScale(-1, normal);
                }

                float scaled_radius = entities[id].hit_radius * entities[id].scale;

                if (sphereCollidesTriangle(next_pos, scaled_radius, vertex0, vertex1, vertex2)) {
                    float cosine = vectorDot(normal, up);
                    switch (dir) {
                        case VD_X:
                            if (cosine < 0.9) {
                                masked_vel.x = 0;
                                entities[id].vel.x = 0;
                            }
                            break;
                        case VD_Y:
                            if (cosine < 0.9) {
                                masked_vel.y = 0;
                                entities[id].vel.y = 0;
                            }
                            break;
                        case VD_Z:
                            {
                                struct vector ground = {0, 0, -1};
                                struct vector intersect_v;
                                if (rayIntersectsTriangle(next_pos, ground, vertex0, vertex1, vertex2, &intersect_v)) {
                                    total_normal = vectorAdd(total_normal, normal);
                                }

                                masked_vel.z = 0;
                                entities[id].vel.z = 0;
                            }
                            break;
                    }
                }
            }
        }
    }

    if (dir == VD_Z) {
        vectorNormalize(&total_normal);
        entities[id].rotation = getRotationQuat(up, total_normal);
    }

    entities[id].pos = vectorAdd(entities[id].pos, masked_vel);
}

void entityCollidesWithPlayer(int id) {
    struct vector next_pos = vectorAdd(entities[id].pos, entities[id].vel);

    bool collides = sphereCollidesSphere(entities[PLAYER_ID].pos, entities[PLAYER_ID].hit_radius * entities[PLAYER_ID].scale, next_pos, entities[id].hit_radius * entities[id].scale);

    if (collides) {
        struct vector v = vectorSubtract(entities[id].pos, entities[PLAYER_ID].pos);
        vectorNormalize(&v);
        v = vectorScale(entities[id].speed, v);

        entities[id].vel = vectorAdd(entities[id].vel, v);

        entities[PLAYER_ID].health -= entities[id].damage;
    }
}

void doBasicFrame(int id) {
    float z = entities[id].vel.z;
    entities[id].vel = vectorSubtract(entities[PLAYER_ID].pos, entities[id].pos);
    entities[id].vel.z = z;
    entities[id].dir = entities[id].vel;
    entities[id].dir.z = 0;
    vectorNormalize(&entities[id].dir);
}

void doPlayerFrame() {
}

void doEntityFrame(int id) {
    switch (entities[id].type) {
        case ET_PLAYER:
            doPlayerFrame();
            break;
        case ET_ENEMY_BASIC:
            doBasicFrame(id);
            break;
        default:
            return;
    }

    vectorNormalize2D(&entities[id].vel);
    entities[id].vel.x *= entities[id].speed;
    entities[id].vel.y *= entities[id].speed;
    entities[id].vel.z -= GRAVITY;
    entityCollidesWithMap(id, VD_X);
    entityCollidesWithMap(id, VD_Y);
    entityCollidesWithMap(id, VD_Z);
    if (entities[id].cooldown) entities[id].cooldown--;
    if (id != PLAYER_ID) entityCollidesWithPlayer(id);
}

int checkDeadEntity(int id) {
    if (entities[id].health <= 0) {
        if (id != PLAYER_ID) {
            entities_size--;
            for (int j = id; j < entities_size; j++) {
                entities[j] = entities[j+1];
            }
            id--;
        }
        else {
            cur_gs = GS_TITLE;
        }
    }
    return id;
}
