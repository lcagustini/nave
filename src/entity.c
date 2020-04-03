void doBasicFrame(int id) {
    entities[id].vel = vectorSubtract(entities[PLAYER_ID].pos, entities[id].pos);
    entities[id].vel.z = 0;
    vectorNormalize(&entities[id].vel);
    entities[id].dir = entities[id].vel;
    entities[id].vel = vectorScale(entities[id].speed, entities[id].vel);
}

void doBulletFrame(int id) {
}

void doPlayerFrame() {
}

void doEntityFrame(int id) {
    switch (entities[id].type) {
        case ET_PLAYER:
            doPlayerFrame();
            break;
        case ET_BULLET:
            doBulletFrame(id);
            break;
        case ET_ENEMY_BASIC:
            doBasicFrame(id);
            break;
        default:
            return;
    }

    struct vector entity_z = {0, 0, -200};
    if (entities[id].type != ET_PLAYER) collidesWithPlayer(id);
    collidesWithMap(id, &entity_z);
    entities[id].vel.z = 0.5*(entity_z.z - entities[id].pos.z);
    entities[id].pos = vectorAdd(entities[id].pos, entities[id].vel);
}
