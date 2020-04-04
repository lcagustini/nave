void newProjectile(struct projectile *cur, struct projectile proj) {
    struct projectile *new = malloc(sizeof(struct projectile));
    *new = proj;

    if (cur) {
        new->next = cur->next;
        cur->next = new;
    }
    else {
        projectiles = new;
    }
}

void doProjectileFrame(struct projectile *cur) {
    cur->pos = vectorAdd(cur->pos, vectorScale(cur->speed, cur->vel));
}
