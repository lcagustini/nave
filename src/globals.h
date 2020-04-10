struct map cur_map;

struct entity entities[10];
int entities_size;

struct projectile *projectiles;

enum gamestate cur_gs;

struct model loaded_models[20];
int loaded_models_n;
