void getInput(cont_state_t *state) {
    if (!state) return;

    memset(&entities[PLAYER_ID].vel, 0, sizeof(entities[PLAYER_ID].vel));
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
    vectorNormalize(&entities[PLAYER_ID].vel);
    if (vectorLenSquared(entities[PLAYER_ID].vel) > 0) {
        entities[PLAYER_ID].dir = entities[PLAYER_ID].vel;
    }
    entities[PLAYER_ID].vel = vectorScale(entities[PLAYER_ID].speed, entities[PLAYER_ID].vel);
}
