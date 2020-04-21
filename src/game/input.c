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

    entities[PLAYER_ID].vel.x = entities[PLAYER_ID].vel.y = 0;
#if 0
    if (state->joyx > DEADZONE || state->joyx < -DEADZONE) {
        entities[PLAYER_ID].vel.x += state->joyx/128.0f;
    }
    if (state->joyy > DEADZONE || state->joyy < -DEADZONE) {
        entities[PLAYER_ID].vel.y -= state->joyy/128.0f;
    }
#else
    if (state->buttons & CONT_DPAD_DOWN) {
        entities[PLAYER_ID].vel.y -= 1;
    }
    if (state->buttons & CONT_DPAD_UP) {
        entities[PLAYER_ID].vel.y += 1;
    }
    if (state->buttons & CONT_DPAD_LEFT) {
        entities[PLAYER_ID].vel.x -= 1;
    }
    if (state->buttons & CONT_DPAD_RIGHT) {
        entities[PLAYER_ID].vel.x += 1;
    }
#endif

    if (vectorLenSquared(entities[PLAYER_ID].dir)) {
        if (entities[PLAYER_ID].cooldown <= 0) {
            struct projectile proj = {
                .pos = entities[PLAYER_ID].pos,
                .vel = entities[PLAYER_ID].dir,
                .model = entities[PLAYER_ID].model,
                .speed = entities[PLAYER_ID].shot_speed,
                .scale = entities[PLAYER_ID].shot_scale,
                .range = entities[PLAYER_ID].range,
                .knockback = entities[PLAYER_ID].knockback,
                .damage = entities[PLAYER_ID].damage,
                .next = NULL,
                .prev = NULL
            };
            newProjectile(projectiles, proj);
            entities[PLAYER_ID].cooldown = entities[PLAYER_ID].shot_rate;
        }
    }
}
