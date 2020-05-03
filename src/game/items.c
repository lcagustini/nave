void removeItem(int i) {
    entities[PLAYER_ID].max_health -= player_items[i].max_health;
    entities[PLAYER_ID].damage /= player_items[i].damage;
    entities[PLAYER_ID].shot_rate /= player_items[i].shot_rate;
    entities[PLAYER_ID].range /= player_items[i].range;
    entities[PLAYER_ID].shot_speed /= player_items[i].shot_speed;
    entities[PLAYER_ID].speed /= player_items[i].speed;
    entities[PLAYER_ID].knockback /= player_items[i].knockback;
    entities[PLAYER_ID].scale /= player_items[i].scale;
    entities[PLAYER_ID].shot_scale /= player_items[i].shot_scale;

    player_items_n--;
    for (int j = i; i < player_items_n; i++) {
        player_items[j] = player_items[j+1];
    }

    for (int j = 0; j < player_items_n; j++) {
        if (player_items[j].shot_type > entities[PLAYER_ID].shot_type) {
            entities[PLAYER_ID].shot_type = player_items[j].shot_type;
        }
    }
}

void applyAllItems() {
    for (int i = 0; i < player_items_n; i++) {
        if (player_items[i].shot_type > entities[PLAYER_ID].shot_type) {
            entities[PLAYER_ID].shot_type = player_items[i].shot_type;
        }
        entities[PLAYER_ID].max_health += player_items[i].max_health;
        entities[PLAYER_ID].damage *= player_items[i].damage;
        entities[PLAYER_ID].shot_rate *= player_items[i].shot_rate;
        entities[PLAYER_ID].range *= player_items[i].range;
        entities[PLAYER_ID].shot_speed *= player_items[i].shot_speed;
        entities[PLAYER_ID].speed *= player_items[i].speed;
        entities[PLAYER_ID].knockback *= player_items[i].knockback;
        entities[PLAYER_ID].scale *= player_items[i].scale;
        entities[PLAYER_ID].shot_scale *= player_items[i].shot_scale;
    }
}

void loadPlayerItems(const char *item_filename) {
    mountRomdisk("/cd/items_romdisk.img", "/items");

    FILE *item_file = fopen(item_filename, "r");
    player_items_n = 0;

    int item_id;
    char buffer[40];
    while ((fscanf(item_file, " %d", &item_id)) != EOF) {
        sprintf(buffer, "/items/%d.item", item_id);
        FILE *item = fopen(buffer, "r");

        struct item *cur = &player_items[player_items_n];

        cur->item_type = (enum itemType) item_id;
        while ((fscanf(item, " %s", buffer)) != EOF) {
            if (!strcmp(buffer, "shot_type")) {
                fscanf(item, " %u", &cur->shot_type);
            }
            else if (!strcmp(buffer, "max_health")) {
                fscanf(item, " %d", &cur->max_health);
            }
            else if (!strcmp(buffer, "damage")) {
                fscanf(item, " %f", &cur->damage);
            }
            else if (!strcmp(buffer, "shot_rate")) {
                fscanf(item, " %f", &cur->shot_rate);
            }
            else if (!strcmp(buffer, "range")) {
                fscanf(item, " %f", &cur->range);
            }
            else if (!strcmp(buffer, "shot_speed")) {
                fscanf(item, " %f", &cur->shot_speed);
            }
            else if (!strcmp(buffer, "speed")) {
                fscanf(item, " %f", &cur->speed);
            }
            else if (!strcmp(buffer, "knockback")) {
                fscanf(item, " %f", &cur->knockback);
            }
            else if (!strcmp(buffer, "scale")) {
                fscanf(item, " %f", &cur->scale);
            }
            else if (!strcmp(buffer, "shot_scale")) {
                fscanf(item, " %f", &cur->shot_scale);
            }
            else {
                assert(false);
            }
        }
        player_items_n++;

        fclose(item);
    }

    fclose(item_file);

    umountRomdisk("/items");
}
