void newProjectile(struct projectile *cur, struct projectile proj) {
    struct projectile *new = malloc(sizeof(struct projectile));
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

bool projectileCollidesWithEntity(struct projectile *cur) {
    for (int i = 0; i < entities_size; i++) {
        if (i == PLAYER_ID) continue;

        struct vector next_pos = vectorAdd(cur->pos, vectorScale(cur->speed, cur->vel));

        bool collides = sphereCollidesSphere(entities[i].pos, entities[i].hit_radius * entities[i].scale, next_pos, cur->hit_radius * cur->scale);

        if (collides) {
            entities[i].health -= cur->damage;
            deleteProjectile(cur);
            return true;
        }
    }
    return false;
}

bool projectileCollidesWithMap(struct projectile *proj) {
    struct vector next_pos = vectorAdd(proj->pos, vectorScale(proj->speed, proj->vel));

    for (int i = 0; i < cur_map.model.num_faces; i++) {
        struct face *cur = &cur_map.model.faces[i];

        struct vector vertex0 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[0]]);
        struct vector vertex1 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[1]]);
        struct vector vertex2 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[2]]);

        float scaled_radius = proj->hit_radius * proj->scale;

        bool collides = sphereCollidesTriangle(next_pos, scaled_radius, vertex0, vertex1, vertex2);

        if (collides) {
            deleteProjectile(proj);
            return true;
        }
    }
    return false;
}

void doProjectileFrame(struct projectile *cur) {
    if (projectileCollidesWithMap(cur)) return;
    if (projectileCollidesWithEntity(cur)) return;
    cur->pos = vectorAdd(cur->pos, vectorScale(cur->speed, cur->vel));
}
