#ifndef MAIN_H
#define MAIN_H

#include "macro.h"
#include "structs.h"
#include "slist.h"
#include <stdbool.h>

extern char IMAGE_FILEPATH[5];
extern char MUSIC_FILEPATH[5];

//Game Overview functions
void gameInit(GAME* game);
void gameClose(GAME* game);
void gameIntro(GAME* game);
void gamePlay(GAME* game);
void gamePause(GAME* game);
void gameMap(GAME* game);
void gameCtrl(GAME* game);
void gameOver(GAME* game);
void gameRender(GAME* game, double fraction);
void gamePopulate(GAME* game);

//Resource functions
int deleteLevel(void* data);
int deleteTileset(void* data);
int deletePalette(void* data);

void loadLevels(GAME* game, const char * filepath);
void loadTextures(GAME* game, const char * filepath);
void loadTilesets(GAME* game, const char * filepath);
void loadPalettes(GAME* game, const char * filepath);
void loadAnimations(GAME* game, const char * filepath);
void loadSounds(GAME* game, const char * filepath);

void freeLevels(GAME* game);
void freeTextures(GAME* game);
void freeTilesets(GAME* game);
void freePalettes(GAME* game);
void freeAnimations(ANIMATION* animations, int count);
void freeSounds(Mix_Music* music[], Mix_Chunk* effect[]);

int printtext(void* data);
int lvl_indexmatch(void* data1, void* data2);
int tset_indexmatch(void* data1, void* data2);

//Entity Stuff
STATE newState(STATE* s); /*create state holder for entities.*/

ENTITY* newEntity(); /* just create a default entity. */

NODE* initEntity(NODE* node, float x, float y, float accX, 
                  float accY, float decX, float decY,
                  float speedX, float speedY, float maxSpeedX, float maxSpeedY,
                  double angle, bool solid, int rx, int ry, int rw, int rh,
                  ANIMATION *sprite, bool visible, double image_angle,
                  int frame_index, KIND kind, STATE state, int hits, 
                  int lives, int points, bool alive, char text[160],
                  void (*funct)(void*, void*));
                  
ENTITY* cloneEntity(ENTITY* prototype);    
NODE* instantiate_entity(int code, float x, float y, GAME* game);
void next_level(GAME* game);
void play_level_music(GAME* game);
void next_tileset(GAME* game);
void issue_id(uint32_t* id, uint32_t* pool);
          
#endif
