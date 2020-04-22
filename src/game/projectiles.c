void newProjectile(struct projectile *cur, struct projectile proj) {
    struct projectile *new = malloc(sizeof(struct projectile));

    if (proj.type == PT_EXPLOSIVE) proj.vel.z = 0.75f;
    vectorNormalize(&proj.vel);
    proj.model = projectiles_models[proj.type];

    *new = proj;

    if (cur) {
        new->next = cur->next;
        new->prev = cur;
        cur->next = new;
        if (new->next) new->next->prev = new;
    }
    else {
        projectiles = new;
    }
}

void deleteProjectile(struct projectile *cur) {
    if (cur == projectiles) {
        projectiles = cur->next;
        projectiles->prev = NULL;
    }
    else {
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
    }
    free(cur);
}

void projectileDamageArea(struct projectile *cur) { //TODO: take walls into consideration
    if (cur->type != PT_EXPLOSIVE) return;

    for (int i = 0; i < entities_size; i++) {
        if (i == cur->owner_entity) continue;

        if (vectorLenSquared(vectorSubtract(entities[i].pos, cur->pos)) < cur->range*cur->range) {
            entities[i].health -= cur->damage;
        }
    }
}

bool projectileCollidesWithEntity(struct projectile *cur) {
    for (int i = 0; i < entities_size; i++) {
        if (i == cur->owner_entity) continue;

        struct vector next_pos = vectorAdd(cur->pos, vectorScale(cur->speed, cur->vel));

        bool collides = sphereCollidesSphere(entities[i].pos, HIT_RADIUS * entities[i].scale, next_pos, HIT_RADIUS * cur->scale);

        if (collides) {
            entities[i].vel = vectorAdd(vectorScale(cur->knockback, cur->vel), entities[i].vel);

            entities[i].health -= cur->damage;
            projectileDamageArea(cur);

            deleteProjectile(cur);
            return true;
        }
    }
    return false;
}

bool projectileCollidesWithMap(struct projectile *proj) {
    struct vector next_pos = vectorAdd(proj->pos, vectorScale(proj->speed, proj->vel));

    int gx = 0.5f + (proj->pos.x/(2*MAP_SCALE));
    int gy = 0.5f + (proj->pos.y/(2*MAP_SCALE));

    for (int x = max(gx-1, 0); x < min(gx+2, MAP_SIZE); x++) {
        for (int y = max(gy-1, 0); y < min(gy+2, MAP_SIZE); y++) {
            struct model *model = &loaded_models[cur_map.models[cur_map.grid[x][y]]];
            struct vector cell_pos = {2*x, 2*y, 0};
            for (int i = 0; i < model->num_faces; i++) {
                struct vector vertex0 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i], cell_pos));
                struct vector vertex1 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i+1], cell_pos));
                struct vector vertex2 = vectorScale(MAP_SCALE, vectorAdd(model->vertices[3*i+2], cell_pos));

                float scaled_radius = HIT_RADIUS * proj->scale;

                bool collides = sphereCollidesTriangle(next_pos, scaled_radius, vertex0, vertex1, vertex2);

                if (collides) {
                    projectileDamageArea(proj);

                    deleteProjectile(proj);
                    return true;
                }
            }
        }
    }
    return false;
}

void doProjectileFrame(struct projectile *cur) {
    struct vector scaled_vel = vectorScale(cur->speed, cur->vel);

    switch (cur->type) {
        case PT_NORMAL:
            if (cur->range <= 0) cur->vel.z -= GRAVITY;
            cur->range -= vectorLen(scaled_vel);
            break;
        case PT_EXPLOSIVE:
            cur->vel.z -= 2.5f*GRAVITY;
            break;
        default:
            return;
    }

    if (projectileCollidesWithMap(cur)) return;
    if (projectileCollidesWithEntity(cur)) return;

    cur->pos = vectorAdd(cur->pos, scaled_vel);
}
