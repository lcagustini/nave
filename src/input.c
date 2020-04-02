void getInput(cont_state_t *state) {
    if (!state) return;

    memset(&player.dir, 0, sizeof(player.dir));
    if (state->buttons & CONT_DPAD_DOWN) {
        player.dir.y -= 1;
    }
    if (state->buttons & CONT_DPAD_UP) {
        player.dir.y += 1;
    }
    if (state->buttons & CONT_DPAD_LEFT) {
        player.dir.x -= 1;
    }
    if (state->buttons & CONT_DPAD_RIGHT) {
        player.dir.x += 1;
    }
    vectorNormalize(&player.dir);
    player.dir = vectorScale(player.speed, player.dir);
}
