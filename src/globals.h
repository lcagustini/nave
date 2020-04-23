struct map cur_map;

struct entity entities[10];
int entities_size;
int entities_models[ET_MAX];

struct projectile *projectiles;
int projectiles_models[PT_MAX];

enum gamestate cur_gs;

struct model loaded_models[20];
int loaded_models_n;

thrd_t loading_thread;
int loading_progress;

unsigned long long global_timer;

int selected_player;
