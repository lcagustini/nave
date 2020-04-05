void getInput(cont_state_t *state) {
    if (!state) return;

    memset(&entities[PLAYER_ID].dir, 0, sizeof(entities[PLAYER_ID].dir));
    if (state->buttons & CONT_A) {
        entities[PLAYER_ID].dir.y -= 1;
    }
    if (state->buttons & CONT_Y) {
        entities[PLAYER_ID].dir.y += 1;
    }
    if (state->buttons & CONT_X) {
        entities[PLAYER_ID].dir.x -= 1;
    }
    if (state->buttons & CONT_B) {
        entities[PLAYER_ID].dir.x += 1;
    }
    vectorNormalize(&entities[PLAYER_ID].dir);

    memset(&entities[PLAYER_ID].vel, 0, sizeof(entities[PLAYER_ID].vel));
    if (state->joyx > DEADZONE || state->joyx < -DEADZONE) {
        entities[PLAYER_ID].vel.x += state->joyx/128.0f;
    }
    if (state->joyy > DEADZONE || state->joyy < -DEADZONE) {
        entities[PLAYER_ID].vel.y -= state->joyy/128.0f;
    }
    entities[PLAYER_ID].vel = vectorScale(entities[PLAYER_ID].speed, entities[PLAYER_ID].vel);

    if (vectorLenSquared(entities[PLAYER_ID].dir)) {
        if (entities[PLAYER_ID].cooldown <= 0) {
            struct projectile proj = {
                .pos = entities[PLAYER_ID].pos,
                .vel = entities[PLAYER_ID].dir,
                .model = entities[PLAYER_ID].model,
                .speed = 0.09,
                .scale = 0.1,
                .hit_radius = 0.9375,
                .damage = 1,
                .next = NULL,
                .prev = NULL
            };
            newProjectile(projectiles, proj);
            entities[PLAYER_ID].cooldown = 60;
        }
    }
}
