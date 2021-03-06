void addVelocityToEntity(struct vector vel, float scale, int id) {
    vectorNormalize(&vel);
    vel = vectorScale(scale, vel);
    vel = vectorSubtract(vel, entities[id].vel);

    /*
    if (vectorLen(vel) > entities[id].speed) {
        vectorNormalize(&vel);
        vel = vectorScale(entities[id].speed, vel);
    }
    */

    if (scale == entities[id].speed) vel = vectorScale(0.35f, vel);
    vel = vectorAdd(entities[id].vel, vel);
    /*
    if (vectorLen(vel) > entities[id].speed) {
        vectorNormalize(&vel);
        vel = vectorScale(entities[id].speed, vel);
    }
    */

    entities[id].vel = vel;
}

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

    for (int x = max(gx-1, 0); x < min(gx+2, cur_map.size); x++) {
        for (int y = max(gy-1, 0); y < min(gy+2, cur_map.size); y++) {
            struct model *model = &loaded_models[cur_map.models[cur_map.grid[x][y]]];
            struct vector cell_pos = {2*x, 2*y, 0};
            for (int i = 0; i < model->num_faces; i++) {
                struct vector vertex0 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i], cell_pos));
                struct vector vertex1 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i+1], cell_pos));
                struct vector vertex2 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i+2], cell_pos));

                struct vector v1 = vectorSubtract(vertex2, vertex0);
                struct vector v2 = vectorSubtract(vertex1, vertex0);
                struct vector normal = vectorCross(v1, v2);
                vectorNormalize(&normal);
                if (normal.z < 0) {
                    normal = vectorScale(-1, normal);
                }

                float scaled_radius = HIT_RADIUS * entities[id].scale;

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

    bool collides = sphereCollidesSphere(entities[PLAYER_ID].pos, HIT_RADIUS * entities[PLAYER_ID].scale, next_pos, HIT_RADIUS * entities[id].scale);

    if (collides) {
        struct vector v = vectorSubtract(entities[id].pos, entities[PLAYER_ID].pos);

        addVelocityToEntity(v, 0.1f, id);

        entities[PLAYER_ID].health -= (int) entities[id].damage;
    }
}

void doEntityFrame(int id) {
    switch (entities[id].type) {
        case ET_PLAYER:
            break;
        case ET_ENEMY_BASIC:
            {
                struct vector target = vectorSubtract(entities[PLAYER_ID].pos, entities[id].pos);

                addVelocityToEntity(target, entities[id].speed, id);

                entities[id].dir = entities[id].vel;
                entities[id].dir.z = 0;
                vectorNormalize(&entities[id].dir);
            }
            break;
        default:
            return;
    }

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

int loadEntitiesFromFile(const char *filename) {
    FILE *f = fopen(filename, "r");

    int count;
    fscanf(f, " %d", &count);

    int loaded = 0;
    for (int i = 0; i < count; i++) {
        struct entity *ent = &entities[entities_size];
        memset(ent, 0, sizeof(struct entity));

        char buffer[40];
        while ((fscanf(f, " %s", buffer)) != EOF) {
            if (!strcmp(buffer, "type")) {
                fscanf(f, " %u", &ent->type);
            }
            else if (!strcmp(buffer, "health")) {
                fscanf(f, " %d", &ent->max_health);
                ent->health = ent->max_health;
            }
            else if (!strcmp(buffer, "damage")) {
                fscanf(f, " %f", &ent->damage);
            }
            else if (!strcmp(buffer, "shot_rate")) {
                fscanf(f, " %f", &ent->shot_rate);
            }
            else if (!strcmp(buffer, "range")) {
                fscanf(f, " %f", &ent->range);
            }
            else if (!strcmp(buffer, "shot_speed")) {
                fscanf(f, " %f", &ent->shot_speed);
            }
            else if (!strcmp(buffer, "speed")) {
                fscanf(f, " %f", &ent->speed);
            }
            else if (!strcmp(buffer, "knockback")) {
                fscanf(f, " %f", &ent->knockback);
            }
            else if (!strcmp(buffer, "scale")) {
                fscanf(f, " %f", &ent->scale);
            }
            else if (!strcmp(buffer, "shot_scale")) {
                fscanf(f, " %f", &ent->shot_scale);
            }
            else if (!strcmp(buffer, "shot_type")) {
                fscanf(f, " %u", &ent->shot_type);
            }
            else if (!strcmp(buffer, "next")) {
                break;
            }
            else {
                assert(false);
            }
        }
        getAvailableMapPosition(&ent->pos);
        ent->model = entities_models[ent->type];

        loaded++;
        entities_size++;
    }

    fclose(f);
    return loaded;
}
