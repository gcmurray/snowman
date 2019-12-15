#ifndef UPDATE_H
#define UPDATE_H

/*****************************************************/
// General Entity Functions
/*****************************************************/
//each entity has pointer to function for it's update
int update_entities(void* node, void* game);
void discard_entities(LIST* lst); //only if not alive
void delete_all_entities(GAME* game); //for all entities no matter what
/****************************************************/
// Update Functions
/****************************************************/
void update_PLAYER1(void* ent, void* game);
void update_BLOCK(void* ent, void* game);
void update_BOUNCER(void* ent, void* game);
//int update_ENEMY1(void* ent, void* game);
//int update_ENEMY2(void* ent, void* game);
//int update_LOOT(void* ent, void* game);
/***************************************************/
//Collision Functions
/***************************************************/
int update_player_collision(GAME* game, ENTITY* ent, LIST* lst, LEVEL* lvl, const uint8_t* key_state);
int update_bouncer_collision(ENTITY* ent, LIST* lst, LEVEL* lvl);
/***************************************************/
// Utility Functions
/**************************************************/
SDL_bool PointInRect(const SDL_Point * p, const SDL_Rect * r);
SDL_Point add_points(SDL_Point a, SDL_Point b);
bool collision_point(const SDL_Point* point, const SDL_Rect* rect);
bool collision_rect(const SDL_Rect* a, const SDL_Rect* b, 
                                      SDL_Rect* rerect, bool intersect);
SDL_Rect get_tile_collisions(SDL_Rect ent_rect, LEVEL* lvl);  
void drawEntity(ENTITY* ent, GAME* game);
int player_get_clid_priority(KIND kind);
/**************************************************/
// Physics Functions
/**************************************************/
void xy_delta(float* dx, float* dy, float speed, double angle, bool add);
void apply_jump(float * spX, float * spY, float jumpSpeed, 
     double gravityDirection, uint32_t updateTick, uint32_t lastUpdate);
void apply_gravity(float * spX, float * spY, float gravity, 
      double gravityDirection, uint32_t updateTick, uint32_t lastUpdate);
float apply_linear_motion(float pixelsPerSecond, uint32_t updateTick, uint32_t lastUpdate);
/***************************************************/
// Camera, misc. graphical
/***************************************************/
void center_camera(GAME* game, int x, int y);
void move_camera(GAME*, int x, int y);
/************************************************************/
//Alarm Callbacks
/************************************************************/
void update_alarms(void* ent, void* game);
uint32_t create_callback(uint32_t interval, void* userevent);
void gen_userevent(SDL_UserEvent* userevent, int32_t code, void* ent, void* game);
void_callback player_callback(int32_t code, void* ent, void* game);
void_callback player_hurting_callback(int32_t code, void* ent, void* game);
void_callback player_dying_callback(int32_t code, void* ent, void* game);
void_callback bouncer_callback(int32_t code, void* ent, void* game);

#endif
