void entityCollidesWithMap(int id, struct vector *max_z) {
    struct vector next_pos = vectorAdd(entities[id].pos, entities[id].vel);
    struct vector rotation_points[4] = {{-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}};
    struct vector rotation_normals[4] = {{-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}};

    for (int i = 0; i < loaded_models[cur_map.model].num_faces; i++) {
        struct face *cur = &loaded_models[cur_map.model].faces[i];

        struct vector vertex0 = vectorScale(MAP_SCALE, loaded_models[cur_map.model].vertices[cur->vertices[0]]);
        struct vector vertex1 = vectorScale(MAP_SCALE, loaded_models[cur_map.model].vertices[cur->vertices[1]]);
        struct vector vertex2 = vectorScale(MAP_SCALE, loaded_models[cur_map.model].vertices[cur->vertices[2]]);

        struct vector v1 = vectorSubtract(vertex2, vertex0);
        struct vector v2 = vectorSubtract(vertex1, vertex0);
        struct vector normal = vectorCross(v1, v2);
        vectorNormalize(&normal);
        if (normal.z < 0) {
            normal = vectorScale(-1, normal);
        }

        struct vector up = {0, 0, 1};

        float cosine = vectorDot(normal, up);

        float angle = acos(cosine) * 180 / M_PI;
        float scaled_radius = entities[id].hit_radius * entities[id].scale;

        // walls
        if (angle > 60 &&
                (vertex0.z > next_pos.z + 0.03 ||
                 vertex1.z > next_pos.z + 0.03 ||
                 vertex2.z > next_pos.z + 0.03)) {
            bool collides = sphereCollidesTriangle(next_pos, scaled_radius, vertex0, vertex1, vertex2);

            if (collides) {
                struct vector v = vectorSubtract(entities[id].pos, vertex0);
                float d = vectorDot(v, normal);
                struct vector collision = vectorScale(d, normal);

                struct vector reaction_v = vectorAdd(entities[id].vel, collision);
                entities[id].vel.x = reaction_v.x * entities[id].vel.x > 0 ? reaction_v.x : 0;
                entities[id].vel.y = reaction_v.y * entities[id].vel.y > 0 ? reaction_v.y : 0;
                entities[id].vel.z = reaction_v.z * entities[id].vel.z > 0 ? reaction_v.z : 0;
            }
        }

        // floors
        else {
            struct vector sky[4] = {
                {entities[id].pos.x + scaled_radius, entities[id].pos.y, 200},
                {entities[id].pos.x - scaled_radius, entities[id].pos.y, 200},
                {entities[id].pos.x, entities[id].pos.y + scaled_radius, 200},
                {entities[id].pos.x, entities[id].pos.y - scaled_radius, 200}
            };

            struct vector ground = {0, 0, -1};

            struct vector intersect_v;
            for (int j = 0; j < 4; j++) {
                bool intersect = rayIntersectsTriangle(sky[j], ground, cur_map.model, cur, &intersect_v);
                if (intersect) {
                    // only false if out of bounds
                    if (rotation_points[j].z < intersect_v.z) {
                        rotation_points[j] = intersect_v;
                        rotation_normals[j] = normal;
                    }
                }
            }

            struct vector sky_center = {entities[id].pos.x, entities[id].pos.y, 200};
            bool intersect = rayIntersectsTriangle(sky_center, ground, cur_map.model, cur, &intersect_v);
            if (intersect) {
                if (max_z->z < intersect_v.z) {
                    *max_z = intersect_v;
                }
            }
        }
    }

    struct vector normal_sum = {0};
    struct vector up = {0,0,1};

    for (int j = 0; j < 4; j++) {
        normal_sum = vectorAdd(normal_sum, rotation_normals[j]);
    }
    vectorNormalize(&normal_sum);
    entities[id].rotation = getRotationQuat(up, normal_sum);

    if (vectorLen(entities[id].vel) > entities[id].speed) {
        vectorNormalize(&entities[id].vel);
        entities[id].vel = vectorScale(entities[id].speed, entities[id].vel);
    }
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
    entities[id].vel = vectorSubtract(entities[PLAYER_ID].pos, entities[id].pos);
    entities[id].vel.z = 0;
    vectorNormalize(&entities[id].vel);
    entities[id].dir = entities[id].vel;
    entities[id].vel = vectorScale(entities[id].speed, entities[id].vel);

    entityCollidesWithPlayer(id);
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

    struct vector entity_z = {0, 0, -200};
    entityCollidesWithMap(id, &entity_z);
    entities[id].vel.z = 0.5*(entity_z.z - (entities[id].pos.z - entities[id].scale*entities[id].hit_radius));
    entities[id].pos = vectorAdd(entities[id].pos, entities[id].vel);
    if (entities[id].cooldown) entities[id].cooldown--;
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
