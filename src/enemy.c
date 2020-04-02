void doBasicFrame(int i) {
    enemies[i].dir = vectorSubtract(player.pos, enemies[i].pos);
    enemies[i].dir.z = 0;
    vectorNormalize(&enemies[i].dir);
    vectorScale(enemies[i].speed, enemies[i].dir);
}

void doEnemyFrame(int i) {
    switch (enemies[i].type) {
        case ET_ENEMY_BASIC:
            doBasicFrame(i);
            break;
        default:
            return;
    }

    struct vector enemy_z = {0, 0, -200};
    collidesWithPlayer(&enemies[i]);
    collidesWithMap(&enemies[i], &enemy_z);
    enemies[i].dir.z = 0.5*(enemy_z.z - enemies[i].pos.z);
    enemies[i].pos = vectorAdd(enemies[i].pos, enemies[i].dir);
    //enemies[i].dir = vectorScale(0.9, enemies[i].dir);
}
