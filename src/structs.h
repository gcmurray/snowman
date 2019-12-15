#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "slist.h"
#include <stdbool.h>

typedef void (*void_callback) (uint32_t, void*, void*);

typedef enum Tile {
    T_EMPTY = -1,      //nothing
    T_SOLID = 0,       //just a block
    T_ONEWAY_UP = 1,       //jump up through platform
    T_ONEWAY_DOWN = 2,     //sliding off branches, etc
    T_ONEWAY_LEFT = 3,     //not used
    T_ONEWAY_RIGHT = 4,    //not used
    T_LEFT_45 = 5,         //not used
    T_RIGHT_45 = 6,        //not used
    T_BRIDGE = 7,          
    T_LEVER_BRIDGE = 8,    
    T_LEVER_WATER = 9,
    T_LEVER_LAVA = 10,
    T_LEVER_USED = 11,
    T_SPIKE = 12,
    T_WATERPOOL = 13,
    T_LAVAPOOL = 14,
    T_WATERFALL = 15,
    T_LAVAFALL = 16,
    T_SNOWPILE = 17,
    T_SMALL_GEM = 18,
    T_LARGE_GEM = 19,
    T_STICK = 20,
    T_CARROT = 21,
    T_LEEK = 22,
    T_VINYL = 23,
    T_1UP = 24,
    T_GEM_BREAK = 25,
    T_LOCKED_CAVE = 26,     //a big key
    T_GATE1 = 27,           //a small key
    T_GATE2 = 28,           //a small key
    T_LOCKED_ENDLEVEL = 29, //big key
    T_LOCKED_ENDGAME = 30,  //heart key
    T_KEY_SMALL = 31,
    T_KEY_BIG = 32,
    T_KEY_HEART = 33
} TILE;

typedef enum Kind {
    NONE = 0,
    /** 1 through 7 free **/
    
    /** players (starts at 8 goes to 9) **/
    PLAYER1 = 8,
    PLAYER2 = 9,
    /** Enemies (starts at 10, goes to 49)**/
    FIRST_ENEMY = 10,
    BOUNCER_ENEMY = 11,
    BAT_ENEMY = 12,
    BAT_GENERATOR = 13,
    EYES_ENEMY = 14,
    RABBIT_ENEMY = 15,
    LEEK_ENEMY = 16,
    MONKEY_ENEMY = 17,
    GORILLA_ENEMY = 18,
    BANANA_ENEMY = 19,
    SAVAGE_ENEMY = 20,
    FIRE_ENEMY = 21,
    FIREWALK_ENEMY = 22,
    FIREBULLET_ENEMY = 23,
    FIREBALL_ENEMY = 24,
    LIZARD_ENEMY = 25,
    LIZARD_BODY = 26,
    DRAGON_ENEMY = 27,
    DRAGON_BODY = 28,
    ANT_ENEMY = 29,
    ICEBALL_ENEMY = 30,
    OWL_ENEMY = 31,
    CELSIUS_ENEMY =32,
    LAST_ENEMY = 49,
    /** LOOT (starts at 50 goes to 79)**/
    FIRST_LOOT = 50,
    SMALL_GEM = 51,
    LARGE_GEM = 52,
    LAST_LOOT = 79,
    /** platforms, utilities **/
    FIRST_PLATFORM = 80,
    BLOCK,
    MOVING_HORIZONTAL,
    MOVING_VERTICAL,
    LAST_PLATFORM = 128,
    LAST_KIND = 255
    } KIND;
/*put actual enemy names in for better description.*/

typedef enum gmode
{
	EXIT, INTRO, PLAY, PAUSE, MAP, OVER, CTRL
} GMODE;
 
typedef struct ent_state {
    bool stateless;
    bool grounded;
    bool airborn;
    bool standing;
    bool jumping;
    bool flying;
    bool ducking;
    bool walking;
    bool running;
    bool swimming;
    bool attacking;
    bool dying;
    bool hurting;
    bool recovering;
    bool crashing;
    bool falling;
    bool fleeing;
    bool winning;
    bool interactive;
    bool active;
    bool solid_above;
    bool solid_below;
    bool solid_left;
    bool solid_right;
    bool state0;
    bool state1;
    bool state2;
    bool state3;
    bool state4;
    bool state5;
    bool state6;
    bool state7;
    bool state8;
    bool state9;
    
} STATE;

typedef struct Animation
{
	/** ANIMATION operations **/
	/* frame_count is how many full image frames there are.
	 * frame index is what full image frame we are currently to draw.
	 * size is 1=small (frame is one rect) 2=medium (4 rects per frame
	 * defined left to right, top to bottom) 3=large (9 rects defined
	 * left to right, top to bottom.)
	 * frame_array holds number refering to a particular rect of a
	 * texture, as defined by the tileset loaded. 
	 * You can place -1 in for frames, if you don't need to draw in
	 * all rects, e.g. a platform three blocks wide and 1 block tall
	 * with image reference (5) can have it's frame represented as:
	 * -1,-1,-1, 5, 5, 5, -1,-1,-1 .*/
	 
    int16_t size;
    int16_t frames;
    int16_t delay;
    SDL_Rect image_rect;
    int16_t* frame_array;
    
} ANIMATION;

typedef struct entity
{
	//physics
	float x, y;
    float accX, accY;
    float decX, decY;
    float speedX;
    float speedY;
    float maxSpeedX;
    float maxSpeedY;
    double angle;
    bool solid;
    SDL_Rect rect;
    SDL_Rect prev_rect;
    //collision check points
    SDL_Point col_leftA;
    SDL_Point col_leftB;
    SDL_Point col_rightA;
    SDL_Point col_rightB;
    SDL_Point col_topA;
    SDL_Point col_topB;
    SDL_Point col_bottomA;
    SDL_Point col_bottomB;
    //positions for generating items, etc.
    SDL_Point top_left;
    SDL_Point top;
    SDL_Point top_right;
    SDL_Point left;
    SDL_Point center;
    SDL_Point right;
    SDL_Point bottom_left;
    SDL_Point bottom;
    SDL_Point bottom_right;
    
    //drawing
    ANIMATION* sprite;
    bool visible;
    double image_angle;
    int frame_index;
    int frame_max;
    int frame_duration;
    int frame_ticks;
    SDL_Texture* image;
    SDL_Rect image_rect;
    
    //logic
    void (*update)(void*, void*);
    uint32_t id;
    KIND kind;
    STATE state;
    STATE previous_state;
    bool alarm_set[10];
    double alarm[10];
    void_callback callback[10];
    //stats
    int hits;
    int lives;
    int points;
    bool alive;
    
    //messages, text, etc.
    char text[160];
    union {
        int32_t connect_id[2];
		float aux;
		float jumpSpeedY;
		//put any other specific entity value here. Add other unions if needed.
	};
	
} ENTITY;

typedef struct plot_s
{
	int16_t code;      //collision code
	int16_t T0;        //base tile
	int16_t T1;        //overlay, behind player
	int16_t T2;        //overlay, front of player
	int16_t TR;        //tile reference, reserve, etc
	int16_t Entity[12];//entity pile
	int16_t ER[12];    //object reference for editor
	
} PLOT_S;

typedef struct Level
{
/*main levels*/
    int index;
    SDL_Rect target_rect;
    int16_t width;
    int16_t height;
    PLOT_S ** plot_map;
    SDL_Texture* targetRender;

} LEVEL;

typedef struct Tileset
{
	int index;
	int width;
	int height;
	int count;
    SDL_Surface* surface_image;
    SDL_Color* pal;
	SDL_Texture* image;
	SDL_Rect image_rect;
	SDL_Rect** tile_number;
 
} TILESET;

typedef struct Game
{
    /*The Game World */
    TTF_Font* font1;
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    GMODE mode;
    const uint8_t* key_state;
    SDL_Event ev;
    /*user defined events*/
    uint32_t USER_CALLBACKEVENT;
    /*time tracking */
    uint32_t currentTick;
    uint32_t lastTick;
    /*physics stuff*/
    double gravityDirection;
    float gravity;
    /******* DEBUG STUFF************/
    int T0visible;
    int T1visible;
    int T2visible;
    uint8_t T2opacity;
    bool free_camera;
    /******* LEVEL STUFF ***********/
    LIST* levels;
    LEVEL* current_level;
    bool levelsloaded;
    LIST* tilesets;
    TILESET* current_tileset;
    LIST* palettes;
    SDL_Color* current_palette;
    bool palettesloaded;
    int cycle_ticks;
    bool tilesloaded;
    bool startflag;
    /****** VIEW STUFF ************/
    SDL_Rect screen_rect; //entire screen.
    SDL_Rect view_rect;  //actual pixel-by-pixel drawing of view.
    SDL_Rect tiling_rect; //x,y,w,h reflect tile in level data, in view.
    int screen_angle;
    int view_speed;
    uint32_t screen_timer;
    bool screen_shake;
    SDL_RendererFlip flip;
    SDL_Color bgcolor;
    int background_index;
    /*********Entities ************/
    LIST* entities;
    uint32_t next_id;
		/* Player 1 Stuff*/
    ENTITY* player1;
    int lives;
    int points;
    int power;
    int smallkey;
    bool bigkey;
    float p1JumpSpeedY;
    
    /****** Other Resources ********/
    SDL_Surface* surfaces[10]; /*this is needed for palette cycling*/
    SDL_Texture* textures[10]; /*TODO: num of images to load?*/
    SDL_Rect texture_rect[10]; /*this holds the total image rect for each texture.*/
    Mix_Music* music[10];      
    Mix_Chunk* effect[10];
    ANIMATION* animations; /* TODO: number of animations in array?*/
    int animation_count;

}GAME;

#endif
