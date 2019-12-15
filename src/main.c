/*
 * Created load levels function.
 * A certain file didn't load correctly. Possibly due to incorrect
 * read values for the height and width? Not an issue with any
 * levels created on linux map editor.
 * Need to make a port view through rendering, and have that centered around
 * the player entity. even simple movement for entity just to see
 * around in the level.
 * Next, working on the tile checks with rect collision for physics..
 * do through an object? possibly. Maybe the tiles themselves. objects
 * is more flexible. need to create sloped ones, so collision makes
 * entity slide.
 * 
 * 22 MAY 2015
 * Working populating level with entities, and rendering all entities visible
 * and active to the screen. Next need to put population function into
 * the beginning of a level and need to create animation file and function
 * to load them into the game. Added a draw_angle data in animation,
 * but realized that that would only rotate individual tiles in drawing
 * stage. Would need to draw all of animation to a animation target
 * then draw that onto screen with animation rotation.
 * 
 * Ready to add objects to a level file and load that into the game;
 * need to have population function run still, not currently putting 
 * anything into the game, and also those objects need to have animations
 * that can be rendered to the screen. The animation drawing function
 * is done, based on tiles in the tileset, allows for tileset swapping
 * for different enemies and locations with minimal effort.
 * 
 * have implemented collision physics for a block and the player,
 * but have yet to test them to find problems. Additionally, the player
 * is not in the main entity list, so we need to make sure we run a test
 * for collision with the list separately for the player characters.
 * 
 * 6 JUL 15
 * 
 * Finished the sprite creation function in the level editor; made it
 * so that it writes size then the frames, then the data. working
 * on the loading of the sprite in the game. the load animations function
 * is completed, need to make the free animations function.
 * 
 * still left is the part to draw the animation, added texture for
 * animation to draw to first to present to renderer, so the image can
 * be manipulated.
 *
 * Did a lot of work, don't remember what the heck that all was,
 * but was last working on deleting an entity on the trip around to clear
 * out the non-alive entities. need to destroy the image texture then
 * get rid of it. On the cusp of actually running with animations,
 * need to use the level editor, make some temp sprites for player
 * and block and make a list of the sprite files for loading.
 * oh yes, did the loading animations function, the free animations
 * function, input the entity functions into the header of the function
 * to be set when instantiated, changed action button for player to
 * space in update function, so as to not conflict with surfing around 
 * the level. but big focus on the entity loop and clearing it out,
 * and actually doing the function stuff for each entity and drawing each.
 * 
 * 
 * 18 July
 * 
 *Supposedly the program is loading the sprites, but nothing is showing
 * on the screen. there is a freeze when trying to advance the level,
 * but i don't have another level right now to advance to. could be 
 * that the entity check going on where it deletes all the entities
 * and repopulates .. could be level related.
 * next step is to try and get the objects to actually show up, or at
 * least get proof they are getting into the game.
 * its not drawing them anyway, it should at least be able to do that.
 * 
 * Ok, sprites are loading, objects are loading, but now we are working
 * on physics, and there are some major hurdles here. Just a total
 * cluster, not really getting how that goes together. Not trying to make
 * a physics engine, just a platformer... need basic acceleration,
 * gravity, and to know whether the player is on the ground or in the
 * air. keep it simple. collision detection just isn't picking up at all,
 * even when an object clearly would have collided. maybe draw a box
 * where the collision rects are, see if there is some error going on
 * with that. Need to look REALL close at the logic, probably a flaw there.
 * 
 * 10 Aug
 * Worked out the jumping logic dy collisions, but still need work on
 * colliding left/right and also for falling off ledges. last left at
 * line 178 in update.c working on trying to make collision on dy only
 * occur if indeed there was not a collision from the side first? problem
 * occurs whenever jumping at the side of a block and it determines there
 * has been a dy collision first, pushing it up and through or down before
 * there has been a side push. maybe do side collisions first, then 
 * dy. I like the jump power and walking power. must implement a run
 * mode so that we can do a faster movement and farther jump. Want to 
 * change jumping so that it is mario like, having momentum and not like
 * megaman with quick in air changes. last save not compiled.
 * 
 * wardriving
 * ethereal tcpdump free protocol analyzer
 * spectrum analyzer
 * 
 * */
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "slist.h"
#include "macro.h"
#include "update.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <inttypes.h>
#include "palette.h"

char IMAGE_FILEPATH[] = "img/";
char SOUND_FILEPATH[] = "snd/";
char LEVEL_FILEPATH[] = "lvl/";
char ANI_FILEPATH[] = "spr/";
const int MS_PER_UPDATE = 16;
const float FRAMES_PER_SEC = 60.0;
const int SMALL_FRAME = 1;
const int MEDIUM_FRAME = 4;
const int LARGE_FRAME = 9;
//why? because I can. ;-)
const double PI = acos(-1.0);
//why not? close enough. lol
const double ONE_OVER_180 = 0.00555555555555555555;

int main(int argc, char * argv[])
{
	double previous = SDL_GetTicks();
	double lag = 0.0;
    GAME game;
    SDL_Event* event = &(game.ev);
    gameInit(&game);
    game.key_state = SDL_GetKeyboardState(NULL);
    
    while (game.mode)
    {
		double current = SDL_GetTicks();
		double elapsed = current - previous;
        game.cycle_ticks += elapsed;
		previous = current;
		lag += elapsed;
		
		SDL_PollEvent(event);
		if (event->type)
		{
			if (event->type == SDL_QUIT)
		    {
			    game.mode = EXIT;
		    }
            if (event->type == game.USER_CALLBACKEVENT)
            {
                printf("detecting user event.\n");
                fflush(stdout);
                ENTITY* ent = event->user.data1;
                void_callback p = ent->callback[event->user.code];
                p(event->user.code, event->user.data1, event->user.data2);
            }
		}
		
		//while (lag >= MS_PER_UPDATE)
		//{
			game.currentTick = SDL_GetTicks();
			
			switch (game.mode)
			{
				case INTRO:
				{
					gameIntro(&game);
					break;
				}
				
				case PLAY:
				{
					gamePlay(&game);
					break;
				} 
				
				case PAUSE:
				{
					gamePause(&game);
					break;
				}
				
				case OVER:
				{
					gameOver(&game);
					break;
				}
				
				case MAP:
				{
					gameMap(&game);
					break;
				}
				
				case CTRL:
				{
					//something
					break;
				}
				
				default:
				{
					//really only exit at this point
				}
			}//end switch game->mode
			//lag -= MS_PER_UPDATE;
	    //}//end while lag >= MS_PER_UPDATE
	    game.lastTick = game.currentTick;
		gameRender(&game, (elapsed/current));
	}//end while game->mode
    
    gameClose(&game);
    return 0;
}

void gameInit(GAME* game)
{
	int imgInit;
	int iflags = IMG_INIT_PNG;
	int musInit;
	int mflags = MIX_INIT_MOD;
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Unable to initialize SDL:   %s", 
		        SDL_GetError());
		exit(1);
	}
	
    game->gWindow = SDL_CreateWindow("platformer", 
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                    768, 576, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
                    
    if (!game->gWindow)
    {
		fprintf(stderr, "SDL unable to create window:   %s",
		        SDL_GetError());
		exit(1);
	}
	
	game->gRenderer = SDL_CreateRenderer(game->gWindow, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE); //SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	
	if(!game->gRenderer)
	{
		fprintf(stderr, "SDL unable to create renderer:   %s",
		        SDL_GetError());
		exit(1);
	}
	
	imgInit = IMG_Init(iflags);
	if (imgInit&iflags != iflags)
	{
		printf("Error initializing SDL2_image. Error: %s", IMG_GetError());
		exit(1);
	}
	
    //hardware hints and settings
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_RenderSetScale(game->gRenderer, 3.0, 3.0);
	SDL_SetRenderDrawBlendMode(game->gRenderer, SDL_BLENDMODE_BLEND);
	
    //USER EVENTS
    game->USER_CALLBACKEVENT = SDL_RegisterEvents(1);
    printf("event number: [ %"PRIu32" ]\n", game->USER_CALLBACKEVENT);
    
	musInit = Mix_Init(mflags);
	if (musInit&iflags != mflags)
	{
		printf("Error initializing SDL_mixer. Error: %s", Mix_GetError());
		exit(1);
	}
	
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Error calling Mix_OpenAudio. Error: %s", Mix_GetError());
		exit(1);
	}
	
	Mix_AllocateChannels(3);
		
	if (TTF_Init() == -1)
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(1);
	}
	
	/*** world stuff ***/
	game->mode = INTRO;
	game->gravityDirection = 270.0;
	game->gravity = 0.001 * FRAMES_PER_SEC;
	// not a pointer now game->ev = NULL;
	/***debug stuff ***/
	game->T0visible = 1;
	game->T1visible = 1;
	game->T2visible = 1;
	game->T2opacity = 255;
	game->free_camera = false;
	/*** level stuff ***/
	game->levels = new_list();
	game->current_level = NULL;
	game->levelsloaded = false;
	game->tilesets = new_list();
    game->current_tileset = NULL;
    game->palettes = new_list();
    game->current_palette = NULL;
    game->cycle_ticks = 0;
    game->tilesloaded = false;
    game->palettesloaded = false;
    game->startflag = false;
	/*** view stuff ***/
	//screen
	game->screen_rect.x = 0;
	game->screen_rect.y = 0;
	game->screen_rect.w = 256;
	game->screen_rect.h = 192;
	//view
	game->view_rect.x = 0;
	game->view_rect.y = 0;
	game->view_rect.w = 256;
	game->view_rect.h = 192;
	//tiling
	game->tiling_rect.x = 0;
	game->tiling_rect.y = 0;
	game->tiling_rect.w = 16;
	game->tiling_rect.h = 12;
	
	//angle
	game->screen_angle = 0;
	//view speed
	game->view_speed = 2;
	game->currentTick = 0;
	game->lastTick = game->screen_timer = SDL_GetTicks();
	game->screen_shake = false;
	//flip
	game->flip = SDL_FLIP_NONE;
    //bgcolor make this load the right color at level load
	game->bgcolor = (SDL_Color) {0,0,0,255};
    game->background_index = 16;
	/****Entities *******/
	game->entities = new_list();
	game->next_id = 10000; //this give us 9999 special id's to use.
		/*Player 1 Stuff*/
	game->player1 = NULL;
	game->p1JumpSpeedY = 8 * FRAMES_PER_SEC;
    int lives = 3;
    int points = 0;
    int power = 0;
    int smallkey = 0;
    bool bigkey = false;
    float p1JumpSpeedY;
	/****Other Resources ****/ //eventually, more palettes.
	game->animations = NULL;
	game->animation_count = 0;
    //set surfaces and textures to zero?
    
	loadTextures(game, "img/");
	loadSounds(game, "snd/"); 
	loadAnimations(game, "spr/");  
    loadPalettes(game,"img/");
    game->palettesloaded = true;
    loadTilesets(game, "img/");
    game->tilesloaded = true;
    loadLevels(game, "lvl/");
    game->levelsloaded = true;             
}

void freeLevels(GAME* game)
{
	printf("Deleting all levels.\n");
	foreach_list(game->levels, deleteLevel);
	clear_list(game->levels);
	free(game->levels);
}

//
int deleteLevel(void* data)
{
	printf("deleting level.\n");
	int y = 0;
	LEVEL* lvl = data;
	
	if (lvl)
	{
		for (y = 0; y < lvl->height; y++)
		{
			free(lvl->plot_map[y]);
		}//end for
		free(lvl->plot_map);
		SDL_DestroyTexture(lvl->targetRender);
    }
    else
    {
		return 0;
	}
	
	return 1;
}
void gameClose(GAME* game)
{
	
	SDL_CloseAudio();
	SDL_DestroyWindow(game->gWindow);
	game->gWindow = NULL;
	SDL_DestroyRenderer(game->gRenderer);
	game->gRenderer = NULL;
	delete_all_entities(game);
	free(game->entities);
	freeTextures(game);
	freeSounds(game->music, game->effect);
	freeAnimations(game->animations, game->animation_count);
	freeLevels(game);
	freeTilesets(game);
    freePalettes(game);
    TTF_CloseFont(game->font1);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void loadTextures(GAME* game, const char * filepath)
{
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
    //also load surfaces here, for cutscenes. change name?
	SDL_Surface* tmp_headicon;
    
    strcpy(dir, "blank.png");
	tmp_headicon = IMG_Load(fullpath);
	
	if (!tmp_headicon)
	{
		printf("error loading file %s. Error: %s\n", fullpath, IMG_GetError());
		exit(1);
	}
    
	game->textures[0] = SDL_CreateTextureFromSurface(game->gRenderer,
	                                                  tmp_headicon);
	game->texture_rect[0] = 
    (SDL_Rect) {0, (game->screen_rect.h/2)-tmp_headicon->h,
                            tmp_headicon->w, tmp_headicon->h};
    
    strcpy(dir, "font1.ttf");
    game->font1 = TTF_OpenFont(fullpath, 8);
	if (!game->font1)
	{
		printf("TTF_OpenFont: Error loading %s - %s\n", fullpath, TTF_GetError());
		exit(1);
	}
    
	free(tmp_headicon);                                              		
}


void freeTextures(GAME* game)
{
	SDL_DestroyTexture(game->textures[0]);
	//SDL_DestroyTexture(textures[1]);
	game->textures[0] = NULL;
	//textures[1] = NULL;
}

void loadSounds(GAME* game, const char * filepath)
{
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
/*****************MUSIC ********************************************/
    int i;
    for (i = 0;i < NUMMUS;i++) {
        switch (i) {
            case 0: strcpy(dir, "tinkermusic.xm");
            break;
            //case 1: strcpy(dir, "romanic.mod");
            //break;
            //case 2: strcpy(dir, "romanic.mod");
            //break;
            //case 3: strcpy(dir, "romanic.mod");
            //break;
            //case 4: strcpy(dir, "romanic.mod");
            //break;
            //case 5: strcpy(dir, "romanic.mod");
            //break;
            //case 6: strcpy(dir, "romanic.mod");
            //break;
            default: 
            printf("error loading music, check num of music.\n "); exit(1);
        }
        game->music[i] = Mix_LoadMUS(fullpath);
        if (!game->music[i]) {
            printf("error loading %s. Error: %s", fullpath, Mix_GetError());
            exit(1);
        }
    }//end for

    
/**********************SOUND EFFECTS*******************************/
    for (i = 0;i < NUMSND;i++) {
        switch (i) {
            case 0: strcpy(dir, "gem.wav");
            break;
            case 1: strcpy(dir, "jump.wav");
            break;
            case 2: strcpy(dir, "key.wav");
            break;
            case 3: strcpy(dir, "thump.wav");
            break;
            case 4: strcpy(dir, "whack.wav");
            break;
            case 5: strcpy(dir, "woosh.wav");
            break;
            case 6: strcpy(dir, "die.wav");
            break;
            case 7: strcpy(dir, "power.wav");
            break;
            default: 
            printf("error loading sounds, check num of sounds.\n "); exit(1);
        }
        game->effect[i] = Mix_LoadWAV(fullpath);
        if (!game->effect[i]) {
            printf("error loading %s. Error: %s", fullpath, Mix_GetError());
            exit(1);
        }
    }//end for
}//end function

void freeSounds(Mix_Music* music[], Mix_Chunk* effect[])
{
    
    int i;
    for (i = 0;i < NUMSND;i++) {
        Mix_FreeChunk(effect[i]);
        effect[i] = NULL;
    }
    
    for (i = 0;i < NUMMUS;i++) {
        Mix_FreeMusic(music[i]);
        music[i] = NULL;
    }
}

void loadLevels(GAME* game, const char * filepath)
{
    //TODO: fix clear buffer problems in all loading functions
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
	char filename[64] = {'\0'};
	char* strptr;
    char* nameptr;
	LIST* list_levels = new_list();
	FILE* pFile;
	SDL_RWops* bFile;
	NODE* accessor;
	int ind = 0;
	int y = 0;
	int x = 0;
    strcpy(dir, "lvl.txt");
	pFile = fopen(fullpath, "r");
	
	if (pFile == NULL)
	{
		fprintf(stderr, "Error opening file. %s - %s\n", fullpath, strerror(errno));
		exit(1);
	}
	else
	{
		while (1)
		{
			strptr = fgets(filename, sizeof(filename), pFile);
			if (strptr != NULL)
			{
				NODE* added = new_node();
                strcpy(dir, strptr);
                added->data = malloc(sizeof(fullpath));
                fullpath[strlen(fullpath)-1] = '\0';
				strcpy(added->data, fullpath);
				ins_node(list_levels, list_levels->lastNode, added);
			}
			else
			{
				break;
			}
	    }//end while
	}//end else
	
	fclose(pFile);
	
	accessor = list_levels->firstNode->next;
	while (accessor != NULL)
	{
		nameptr = accessor->data;
		bFile = SDL_RWFromFile(nameptr, "rb");
		
		if (bFile != NULL)
		{
			NODE* lvl = new_node();

			if (lvl)
			{
				LEVEL* lvl_ptr;
				lvl_ptr = malloc(sizeof(LEVEL));
				lvl->data = lvl_ptr;
				
				if (lvl_ptr)
				{
					printf("Adding level - %s \n", nameptr);
					ins_node(game->levels, game->levels->lastNode, lvl);
					lvl_ptr->index = ind++;
					lvl_ptr->height = SDL_ReadLE16(bFile);
					lvl_ptr->width = SDL_ReadLE16(bFile);
					lvl_ptr->target_rect.x = 0;
					lvl_ptr->target_rect.y = 0;
					lvl_ptr->target_rect.w = lvl_ptr->width*TILESIZE;
					lvl_ptr->target_rect.h = lvl_ptr->height*TILESIZE;
					//this is the level's texture to render to.
					lvl_ptr->targetRender = SDL_CreateTexture(game->gRenderer, 
					                                  SDL_PIXELFORMAT_RGBA32,
					                                  SDL_TEXTUREACCESS_TARGET,
					                                  lvl_ptr->width*TILESIZE,
					                                  lvl_ptr->height*TILESIZE);

					lvl_ptr->plot_map = malloc(lvl_ptr->height * sizeof(PLOT_S*));
			        
			        for (y = 0; y < lvl_ptr->height; y++)
					{
					
						lvl_ptr->plot_map[y] = calloc(lvl_ptr->width, sizeof(PLOT_S));
					}
					
					//read in plots
					for (y = 0; y < lvl_ptr->height; y++)
					{
						PLOT_S* data = lvl_ptr->plot_map[y];
						SDL_RWread(bFile, data, sizeof(PLOT_S), lvl_ptr->width);
					}//end for

				}//end if lvl->data
			}//end if lvl
			
			SDL_RWclose(bFile); //wrap up that file
		}//end if bFile != NULL
		else
		{
			fprintf(stderr, "Could not open file %s - %s \n", nameptr, strerror(errno)); 
		}
    accessor = accessor->next; //go to next name in list
	}//end while accessor != NULL
	
	//foreach_list(list_levels, printtext);
	//printf("\n");
    game->current_level = game->levels->firstNode->next->data; //set lvl
	clear_list(list_levels);
	free(list_levels);
}

void loadTilesets(GAME* game, const char * filepath)
{
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
	SDL_Surface* image = NULL;
	SDL_Surface* tmp = NULL;
    SDL_PixelFormat* fmt = NULL;
	int image_width = 0;
	int image_height = 0;
	int image_columns = 0;
	int image_rows = 0;
	bool resizeFlag = false;
	int image_remainder = 0;
	int tilespace = 0; // the number of tiles malloc'd for.
	int tilecount = 0; //tile we are on when looping to set the SDL_Rects.
	char filename[64] = {'\0'};
	char* nameptr;
	LIST* list_tilesets = new_list();
	FILE* pFile;
	SDL_RWops* bFile;
	NODE* accessor;
	int ind = 0;
	int y = 0;
	int x = 0;
    strcpy(dir, "tilesets.txt");
	pFile = fopen(fullpath, "r");
	
	if (pFile == NULL)
	{
		fprintf(stderr, "Error opening file. %s - %s\n", fullpath, strerror(errno));
		exit(1);
	}
	else
	{
		while (1)
		{
			nameptr = fgets(filename, sizeof(filename), pFile);
			if (nameptr != NULL)
			{
				NODE* added = new_node();
                strcpy(dir, nameptr);
				added->data = malloc(sizeof(fullpath));
			    fullpath[strlen(fullpath)-1] = '\0';
				strcpy(added->data, fullpath);
				ins_node(list_tilesets, list_tilesets->lastNode, added); 
			}
			else
			{
				break;
			}
	    }//end while
	}//end else
	
	fclose(pFile);
	
	accessor = list_tilesets->firstNode->next;
	while (accessor != NULL)
	{
		nameptr = accessor->data;
		bFile = SDL_RWFromFile(nameptr, "rb");
		
		if (bFile != NULL)
		{
			NODE* tset = new_node();
			
			if (tset)
			{
				TILESET* tset_ptr;
				tset_ptr = malloc(sizeof(TILESET));
				tset->data = tset_ptr;
				
				if (tset_ptr)
				{
					ins_node(game->tilesets, game->tilesets->lastNode, tset);
					//do the tileset loading here.
					image = IMG_Load_RW(bFile, 0);
                    //SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);
                    SDL_SetPaletteColors(image->format->palette, 
                                      game->current_palette, 0, COLORS_IN_PALETTE);
                    fmt = image->format;
                    SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(fmt,0,0,0));
                    
					if (image == NULL)
					{
						printf("Surface error. Error: %s\n", IMG_GetError());
						exit(1);
					}
					else
					{
						image_width = image->w;
						printf("\nimage_width: %d", image_width);
						image_height = image->h;
						printf("\nimage_height: %d", image_height);
						image_remainder = (image_width % TILESIZE);
						if (image_remainder)
						{
							image_width += image_remainder;
							resizeFlag = true;
							printf("\nthere was a remainder of %d.", image_remainder);
						}
						
						image_columns = image_width / TILESIZE;
						image_remainder = (image_height % TILESIZE);
						if (image_remainder)
						{
							image_height += image_remainder;
							resizeFlag = true;
						}
						
						//set image rect for tileset image.
						tset_ptr->image_rect.x = 0;
						tset_ptr->image_rect.y = 0;
						tset_ptr->image_rect.w = image_width;
						tset_ptr->image_rect.h = image_height;
						
						image_rows = image_height / TILESIZE;
						
						if (resizeFlag) //wasn't evenly divisble by tilesize...
						{ //so we make one that is, and blit the tiles to it.
							SDL_Rect rect = {0,0,image->w,image->h};
							
							tmp = SDL_CreateRGBSurface(0,image_width,
													 image_height, 8, 0,0,0,0);
                            //SDL_SetSurfaceBlendMode(tmp, SDL_BLENDMODE_NONE);
                            SDL_SetPaletteColors(tmp->format->palette, 
                                      game->current_palette, 0, COLORS_IN_PALETTE);
                            fmt = tmp->format;
                            SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(fmt,0,0,0));
							if(SDL_BlitSurface(image, NULL, tmp, &rect))
							{
								printf("resize error. Error: %s", SDL_GetError());
								SDL_FreeSurface(tmp);
								SDL_FreeSurface(image);
								exit(1);
							}
						}//end if resizeflag
						
                        //we want to save the image for later manipulation (cycling!)
                        tset_ptr->surface_image = 
                            SDL_CreateRGBSurfaceWithFormat(0, image_width, image_height, 8, SDL_PIXELFORMAT_INDEX8);
                        //SDL_SetSurfaceBlendMode(tset_ptr->surface_image, SDL_BLENDMODE_NONE);
                        
                        //set palette
                        SDL_SetPaletteColors(tset_ptr->surface_image->format->palette, 
                                  game->current_palette, 0, COLORS_IN_PALETTE);
                                  
                        //set the first index color as the color key.
                        fmt = tset_ptr->surface_image->format;
                        SDL_SetColorKey(tset_ptr->surface_image, SDL_TRUE, SDL_MapRGB(fmt,0,0,0));
                            
						if (resizeFlag)
						{
                            //SDL_SetSurfaceBlendMode(tmp, SDL_BLENDMODE_NONE);
                            //blit
                            SDL_BlitSurface(tmp, NULL, tset_ptr->surface_image, NULL);
                            
                            //also create a texture.
							tset_ptr->image = SDL_CreateTextureFromSurface(
							game->gRenderer, tset_ptr->surface_image);
							printf("resized texture successful.\n");
						}
						else
						{
                            //SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);
                            //blit
                            SDL_BlitSurface(image, NULL, tset_ptr->surface_image, NULL);
                            
                            //also create a texture.
							tset_ptr->image = SDL_CreateTextureFromSurface(
							game->gRenderer, tset_ptr->surface_image);
							printf("no resize necessary.\n");
						}
							
						if (tset_ptr->image == NULL)
						{
							printf("\nError creating texture from surface. Error: %s\n", SDL_GetError());
							exit(1);
						}
						else
						{
							printf("\nTexture Successful.\n");
							SDL_FreeSurface(image);
							if (tmp != NULL)
								SDL_FreeSurface(tmp);
						}
                        
						/*create SDL_Rects for numberedTiles, to draw tile selections.*/
						
						tilespace = image_rows * image_columns;
						printf("tilespaces:%d\nimage_rows:%d\nimage_columns:%d\n",
											 tilespace, image_rows, image_columns);
						tset_ptr->count = tilespace;
						tset_ptr->index = ind++;
						tset_ptr->tile_number = malloc(tilespace * sizeof(SDL_Rect*));
						if (tset_ptr->tile_number == NULL)
						{
							printf("\nmalloc error. Cannot make space for tilerects.");
							exit(1);
						}
						else
						{
							
							//printf("tiles memory allocation successful.");

						}	
						
						for (y = 0; y < image_rows; y++)
						{
							for (x = 0; x < image_columns; x++)
							{
								tset_ptr->tile_number[tilecount] = malloc(sizeof(SDL_Rect));
								tset_ptr->tile_number[tilecount]->x = x*TILESIZE;
								tset_ptr->tile_number[tilecount]->y = y*TILESIZE;
								tset_ptr->tile_number[tilecount]->w = TILESIZE;
								tset_ptr->tile_number[tilecount]->h = TILESIZE;
						
								tilecount++;			
							}//end loop left to right
						}//end loop top to bottom	
					}//end tileloading part
				}
			}//end if tset node
			SDL_RWclose(bFile); //wrap up that file
		}//end if bFile != NULL
		else
		{
			fprintf(stderr, "Could not open file %s - %s \n", nameptr, strerror(errno)); 
		}
    accessor = accessor->next; //go to next name in list
	}//end while accessor != NULL
	
	//foreach_list(list_levels, printtext);
	//printf("\n");
	if (game->tilesets->firstNode->next == NULL)
	{
		printf("data is null.");
	}
    game->current_tileset = game->tilesets->firstNode->next->data; //set tileset
    
        /*set initial background color*/
    if (game->background_index < COLORS_IN_PALETTE - 1) {
        uint8_t r = 
        game->current_tileset->surface_image->format->palette->colors[game->background_index].r;
        uint8_t g = 
        game->current_tileset->surface_image->format->palette->colors[game->background_index].g;
        uint8_t b = 
        game->current_tileset->surface_image->format->palette->colors[game->background_index].b;
        game->bgcolor = (SDL_Color) {r,g,b,255};
    }
    
	clear_list(list_tilesets);
	free(list_tilesets);
	
}

void freeTilesets(GAME* game)
{
	printf("freeing tilesets.\n");
	foreach_list(game->tilesets, deleteTileset);
	clear_list(game->tilesets);
	free(game->tilesets);
}

int deleteTileset(void* data)
{
	int x;
	TILESET* tset = data;
	
    if (tset) {
        if (tset->image)
            SDL_DestroyTexture(tset->image);
        if (tset->surface_image)
            SDL_FreeSurface(tset->surface_image);
    
        for (x = 0; x < tset->count; x++)
        {
            free(tset->tile_number[x]);
        }
        printf("level deleted.\n");
        return 1;
    }
    return 0;
}

void loadPalettes(GAME* game, const char * filepath)
{
    //TODO: fix clear buffer problems in all loading functions
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
    char filename[64] = {'\0'};
	char* nameptr;
	LIST* list_palettes = new_list();
	FILE* pFile;
	SDL_RWops* bFile;
	NODE* accessor;
	int ind = 0;
	int y = 0;
	int x = 0;
    strcpy(dir, "pal.txt");
	pFile = fopen(fullpath, "r");
	
	if (pFile == NULL)
	{
		fprintf(stderr, "Error opening file. %s - %s\n", fullpath, strerror(errno));
		exit(1);
	}
	else
	{
		while (1)
		{
			nameptr = fgets(filename, sizeof(filename), pFile);
			if (nameptr != NULL)
			{
				NODE* added = new_node();
                strcpy(dir, nameptr);
				added->data = malloc(sizeof(fullpath));
			    fullpath[strlen(fullpath)-1] = '\0';
				strcpy(added->data, fullpath);
				ins_node(list_palettes, list_palettes->lastNode, added); 
			}
			else
			{
				break;
			}
	    }//end while
	}//end else
	
	fclose(pFile);
	
	accessor = list_palettes->firstNode->next;
    
	while (accessor != NULL)
	{
		nameptr = accessor->data;
        
		if (nameptr != NULL)
		{
			NODE* palette = new_node();
			
			if (palette)
			{
				SDL_Color* palette_ptr;
				palette_ptr = malloc(COLORS_IN_PALETTE * sizeof(SDL_Color));
                if (palette_ptr) {
                    loadPalette(nameptr, palette_ptr, COLORS_IN_PALETTE);
                    palette->data = palette_ptr;
                    ins_node(game->palettes, game->palettes->lastNode, palette);
                }
            }
        }//end if nameptr not null
        accessor = accessor->next; //go to next name in list
    }//end while accessor != NULL
    game->current_palette = game->palettes->firstNode->next->data; //set palette
	clear_list(list_palettes);
	free(list_palettes);
}

void freePalettes(GAME* game)
{
	printf("freeing palettes.\n");
	foreach_list(game->palettes, deletePalette);
	clear_list(game->palettes);
	free(game->palettes);
}

int deletePalette(void* data)
{
	SDL_Color* palette = data;
	free(palette);
	printf("palette deleted.\n");
	return 1;   
}

int printtext(void* data)
{
	int err = 0;
	err = printf("%s", (char*) data);
	if (err < 0)
	return 0;
	else
	return 1;
}

int lvl_indexmatch(void* data1, void* data2)
{
	LEVEL* d1 = data1;
	int* d2 = data2;
	if (*d2 == d1->index)
	{
		return 1;
	}
	
	return 0;
}

int tset_indexmatch(void* data1, void* data2)
{
	TILESET* d1 = data1;
	int* d2 = data2;
	if (*d2 == d1->index)
	{
		return 1;
	}
	
	return 0;
}
void loadAnimations(GAME* game, const char * filepath)
{
    //TODO: fix clear buffer problems in all loading functions
    char fullpath[64] = {'\0'};
    strcpy(fullpath, filepath);
    //pointer to null at end
    char * dir = &fullpath[strlen(fullpath)];
    
	char filename[64] = {'\0'};
	char* nameptr;
	LIST* list_sprites = new_list();
	FILE* pFile;
	SDL_RWops* bFile;
	NODE* accessor;
	int x = 0;
	int sprite_count = 0;
	int sprite_loop = 0;
	int16_t size = 0;
    int16_t frames = 0;
    int16_t delay = 0;
	int array_size = 0;
	
/************************ADDING SPRITE NAMES TO LIST ****************/	
    strcpy(dir, "sprites.txt");
	pFile = fopen(fullpath, "r");
	
	if (pFile == NULL)
	{
		fprintf(stderr, "Error opening file. %s - %s\n", fullpath, strerror(errno));
		exit(1);
	}
	else
	{
		while (1)
		{
			nameptr = fgets(filename, sizeof(filename), pFile);
			if (nameptr != NULL)
			{
				NODE* added = new_node();
                strcpy(dir, nameptr);
				added->data = malloc(sizeof(fullpath));
			    fullpath[strlen(fullpath)-1] = '\0';
				strcpy(added->data, fullpath);
				ins_node(list_sprites, list_sprites->lastNode, added);
				sprite_count++;
			}
			else
			{
				printf("couldn't read a sprite name from file.\n");
				break;
			}
	    }//end while
	}//end else
	
	fclose(pFile);
/*****************************END ADDING SPRITE NAME LIST ***********/	
	
	accessor = list_sprites->firstNode->next;
	game->animations = malloc(sprite_count * sizeof(ANIMATION));
	game->animation_count = sprite_count;

	while (accessor != NULL)
	{
		nameptr = accessor->data;
		bFile = SDL_RWFromFile(nameptr, "rb");
		
		if (bFile != NULL)
		{
				
			printf("Adding sprite - %s \n", nameptr);
			/** setup the animation at the given index. **/
			//size and frames
			size = SDL_ReadLE16(bFile);
			frames = SDL_ReadLE16(bFile);
            delay = SDL_ReadLE16(bFile);
			game->animations[sprite_loop].size = size;
			game->animations[sprite_loop].frames = frames;
            game->animations[sprite_loop].delay = delay;
			
			//malloc and read in array for image.
			array_size = size*size*frames;
			game->animations[sprite_loop].frame_array = 
			malloc(sizeof(int16_t)*array_size);
			
			//read in the values to the array...
			for (x = 0; x < array_size; x++)
			{
				game->animations[sprite_loop].frame_array[x] =
				SDL_ReadLE16(bFile);
			}
			
		}//end if bFile != NULL (sprite can't be read in)
		else
		{
			fprintf(stderr, "Could not open file %s - %s \n", nameptr, strerror(errno)); 
		}
	
	SDL_RWclose(bFile); //wrap up that file
    accessor = accessor->next; //go to next name in list
    sprite_loop++; //go to the next animation index
    
	}//end while accessor != NULL (no more sprites to be read in)
	
	clear_list(list_sprites);
	free(list_sprites);	
}//end of loadAnimations

void freeAnimations(ANIMATION* animations, int count)
{
	int current = 0;
	
	for (current = 0; current < count; current++)
	{
		printf("next animation frame freed...");
		free(animations[current].frame_array);
	}
	printf("freeing animation pointer...");
	free(animations);
}

void gameIntro(GAME* game)
{	
	SDL_Event* event = &(game->ev);
    
	SDL_SetRenderDrawColor(game->gRenderer, 20, 20, 100, 255);
	SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
    
	if (game->texture_rect) {
        if (game->texture_rect[0].x > game->screen_rect.w)
        {
            game->texture_rect[0].x = -game->texture_rect[0].w;
        }
        else
        {
            game->texture_rect[0].x += 2;
        }
        
        if (event->type)
        {
            if (event->type == SDL_KEYDOWN)
            {
                if (event->key.keysym.sym == SDLK_RETURN)
                {
                    game->mode = PLAY;
                    game->startflag = true;
                    //playin music is temporary. The main menu will actually
                    //be after the intro, (once I program it in, that is)
                    play_level_music(game);
                    gamePopulate(game);
                    center_camera(game, 
                        (int) game->player1->x + game->player1->top.x,
                        (int) game->player1->y);
                    game->player1->state.interactive = true;
                    
                }
            }
        }//end if game->event != NULL
    }//end if textures exist
}//end function

void gamePlay(GAME* game)
{
	double xval = 0;
	double yval = 0;

	if (game->free_camera)
	{
		if (game->key_state[SDL_SCANCODE_LEFT])
		{
			printf("left ");
			if (game->view_rect.x >= game->view_speed)
			//{
				game->view_rect.x -= game->view_speed;
			//}
		}
		
		if (game->key_state[SDL_SCANCODE_RIGHT])
		{
			printf("right ");
			if (game->view_rect.x <= game->current_level->target_rect.w-game->view_rect.w-game->view_speed)
			//{
				game->view_rect.x += game->view_speed;
			//}
		}
		
		if (game->key_state[SDL_SCANCODE_UP])
		{
			printf("up ");
			if (game->view_rect.y >= game->view_speed)
			//{
				game->view_rect.y -= game->view_speed;
			//}
		}
		
		if (game->key_state[SDL_SCANCODE_DOWN])
		{
			printf("down ");
			if (game->view_rect.y <= game->current_level->target_rect.h-game->view_rect.h-game->view_speed)
			//{
				game->view_rect.y += game->view_speed;
			//}
		}
    }//end if free_camera
	
	if (game->ev.type == SDL_KEYDOWN)
	{
		if (game->ev.key.keysym.sym == SDLK_t)
		{
			game->free_camera = !(game->free_camera);
            if (game->free_camera && game->player1 && game->player1->alive) {
                center_camera(game,
                        (int) game->player1->x + game->player1->top.x, 
                                                (int) game->player1->y); 
            }
		}
		
		if (game->ev.key.keysym.sym == SDLK_EQUALS)
		{
			printf("equals ");
			next_level(game);
		}
		
		if (game->ev.key.keysym.sym == SDLK_MINUS)
		{
			printf("minus ");
			next_tileset(game);
		}
		
	}//end key pressed...
	
	if (game->key_state[SDL_SCANCODE_S])
	{
		if (game->screen_angle > 0)
		{
			game->screen_angle -= 1;
		}
		else
		{
			game->screen_angle = 359;
		}
	}
	
	if (game->key_state[SDL_SCANCODE_F])
	{
		game->screen_angle += 1;
	}
	
	if (game->key_state[SDL_SCANCODE_U] && !(game->screen_shake))
	{
		game->screen_shake = true;
		game->screen_timer = SDL_GetTicks() + 500; //half second shake
	}
	
	if (game->screen_shake)
	{
		if (game->screen_rect.y > (-3) && game->screen_rect.y < 3)
		{
			game->screen_rect.y -= 3;
			if (game->screen_rect.y == -3)
			{
				game->screen_rect.y = 3;
			}
		}
		else
		{
			game->screen_rect.y += 3;
			if (game->screen_rect.y == 6)
			{
				game->screen_rect.y = 0;
			}
		}
		
		if (SDL_GetTicks() > game->screen_timer)
		{
			game->screen_rect.y = 0;
			game->screen_shake = false;
		}
	}

	xval = game->view_rect.x / TILESIZE; //get portions not round number
	yval = game->view_rect.y / TILESIZE;
	xval = floor(xval); //bring them down to the lower round number
	yval = floor(yval); 
	game->tiling_rect.x = xval; //that's the tiling square.
	game->tiling_rect.y = yval;
	
    if (!game->free_camera) {
        if (game->player1 && game->player1->alive) {
            move_camera(game, 
                        (int) game->player1->x + game->player1->top.x, 
                                                (int) game->player1->y);
        }
    }
    
    if (game->points == 30) {
        game->mode = OVER;
    }
	/****** UPDATE THOSE ENTITIES! *******/
	update_entities(game->entities->firstNode->next, game);
	discard_entities(game->entities);
	
}//end function gamePlay

//
void gamePopulate(GAME* game)
{
	/*future note: possible to make this work based on chunks,
	 * so many tiles wide and tall. Once the view enters into this
	 * space, this function runs again and resets the current chunk.
	 */
	 
	int x = 0;
	int y = 0;
	int z = 0;
	int countx = 0;
	int county = 0;
	float plotx = 0.0;
	float ploty = 0.0;
	NODE* node = 0;
	
	//Entity Data from Map
	for (y = 0; y < game->current_level->height;y++)
	{
		countx = 0;
		//this for loop does the same for the left and right.
		for (x = 0; x < game->current_level->width;x++)
		{   
			for (z = 0; z < ENTITYSIZE;z++)
			{
				int16_t data = game->current_level->plot_map[y][x].Entity[z];
				plotx = countx*(TILESIZE);
				ploty = county*(TILESIZE);
				//draw tiles to map here.
				if (data != -1)
				{
					/**** THIS IS WHERE THE MAGIC HAPPENS ****/
					//create the node
					node = instantiate_entity( data, plotx, ploty, game);
					//add node to list
					
					if (node != NULL)
					{
						//printf("node not null! adding to list.\n");                      
						ins_node(game->entities,
								 game->entities->lastNode,node);
					}
					else
					{
						//printf("apparently ent node null.\n");
					}
				}
			}//end for z
			countx++;
		}//end for x
		county++;
	}//end for y
}//end function


//most parts in here will be dealt with using codes.
//parts to remain are moving objects like moving platforms, enemies, etc
NODE* instantiate_entity(int code, float x, float y, GAME* game)
{
	//For reference only
	/*initEntity(NODE* node, float x, float y, float accX, 
                  float accY, float decX, float decY,
                  float speedX, float speedY, float maxSpeedX, float maxSpeedY,
                  double angle, bool solid, int rx, int ry, int rw, int rh,
                  ANIMATION *sprite, bool visible, double image_angle,
                  int frame_index, KIND kind, STATE state, int hits, 
                  int lives, int points, bool alive, char text[160],
                  void (*funct)(void*, void*))*/
	NODE* node = NULL;
	ENTITY* ent = NULL;
	
	switch (code)
	{
		case BLOCK: //just a solid block.
		{	
			node = initEntity(NULL, x, y, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 
			true, (int) x, (int) y, TILESIZE, TILESIZE, &(game->animations[0]),
			false, 0.0, 0, BLOCK, newState(NULL), 0, 0, 0, true, "",
			update_BLOCK);
			
			ent = node->data;
			ent->state.active = true;
			
			//special points
			ent->top_left.x = 0;
			ent->top_left.y = 0;
			ent->top.x = 7;
			ent->top.y = 0;
			ent->top_right.x = 15;
			ent->top_right.y = 0;
			ent->left.x = 0;
			ent->left.y = 7;
			ent->center.x = 7;
			ent->center.y = 7;
			ent->right.x = 15;
			ent->right.y = 7;
			ent->bottom_left.x = 0;
			ent->bottom_left.y = 15;
			ent->bottom.x = 7;
			ent->bottom.y = 15;
			ent->bottom_right.x = 15;
			ent->bottom_right.y = 15;
			
			break;
		}
		
		case MOVING_HORIZONTAL: 
		//moving platform ([explain kind here, in this case horizontal])
		{
			node = initEntity(NULL, x, y, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 
			true, (int) x, (int) y, TILESIZE, TILESIZE, &(game->animations[0]),
			false, 0, 0, MOVING_HORIZONTAL, newState(NULL), 0, 0, 0, true, "",
			update_BLOCK); //different functions indicate different movements
			
			ent = node->data;
			ent->state.active = true;
			
			//special points
			ent->top_left.x = 0;
			ent->top_left.y = 0;
			ent->top.x = 7;
			ent->top.y = 0;
			ent->top_right.x = 15;
			ent->top_right.y = 0;
			ent->left.x = 0;
			ent->left.y = 7;
			ent->center.x = 7;
			ent->center.y = 7;
			ent->right.x = 15;
			ent->right.y = 7;
			ent->bottom_left.x = 0;
			ent->bottom_left.y = 15;
			ent->bottom.x = 7;
			ent->bottom.y = 15;
			ent->bottom_right.x = 15;
			ent->bottom_right.y = 15;
			
			break;
		}
		
		case PLAYER1: //player 1
		{
			float accX = 0.2 * FRAMES_PER_SEC;
			float accY = 0.5 * FRAMES_PER_SEC;
			float decX = 0.3 * FRAMES_PER_SEC;
			float decY = 0;
			float maxSpeedX = 2.0 * FRAMES_PER_SEC;
			float maxSpeedY = 3.0 * FRAMES_PER_SEC;
			float speedX = 0;
			float speedY = 0;
			
			node = initEntity(NULL, x, y, accX, accY, decX, decY, 
			speedX, speedY, maxSpeedX, maxSpeedY, 0.0, false, 
			(int) x, (int) y, TILESIZE, TILESIZE, 
			&(game->animations[45]), true, 0, 0, PLAYER1, newState(NULL), 
			5, 3, 0, true, "", update_PLAYER1);
			
			ent = node->data;
            //attach this to the game-wide player1 variable. Not owned, just ref.
            game->player1 = ent;
			ent->state.stateless = false;
			ent->state.active = true;
            ent->state.solid_right = true;
			ent->jumpSpeedY = game->p1JumpSpeedY;
			
            //setup callback
            ent->callback[0] = (void_callback) player_hurting_callback;
            ent->callback[1] = (void_callback) player_dying_callback;
                //hurting callback
            ent->alarm_set[0] = false;
            ent->alarm[0] = 0;
                //dying callback
            ent->alarm_set[1] = false;
            ent->alarm[1] = 0;
            
			//drawing
			ent->frame_index = 0;
			ent->frame_max = ent->sprite->frames;
			ent->frame_duration = ent->sprite->delay;
            //player's collisin points
            ent->col_leftA = (SDL_Point) {0,4};
            ent->col_leftB = (SDL_Point) {0,12};
            ent->col_rightA = (SDL_Point) {15,4};
            ent->col_rightB = (SDL_Point) {15,12};
            ent->col_topA = (SDL_Point) {4,0};
            ent->col_topB = (SDL_Point) {12,0};
            ent->col_bottomA = (SDL_Point) {4,15};
            ent->col_bottomB = (SDL_Point) {12,15};
			//player's special points
			ent->top_left = (SDL_Point) {0,0};
			ent->top = (SDL_Point) {7,0};
			ent->top_right = (SDL_Point) {15,0};
			ent->left = (SDL_Point) {0,7};
			ent->center = (SDL_Point) {7,7};
			ent->right = (SDL_Point) {15,7};
			ent->bottom_left = (SDL_Point) {0,15};
			ent->bottom = (SDL_Point) {7,15};
			ent->bottom_right = (SDL_Point) {15,15};
			
			//set rect
			ent->rect.x = (int) ent->x;
			ent->rect.y = (int) ent->y;
			ent->image_rect.x = ent->rect.x;
			ent->image_rect.y = ent->rect.y;
		    break;
		}
        
		case BOUNCER_ENEMY: //my first enemy, yay
		{
			float accX = 0;
			float accY = 0.1 * FRAMES_PER_SEC;
			float decX = 0.3 * FRAMES_PER_SEC;
			float decY = 0;
			float maxSpeedX = 2.0 * FRAMES_PER_SEC;
			float maxSpeedY = 3.0 * FRAMES_PER_SEC;
			float speedX = 0;
			float speedY = 0;
			
			node = initEntity(NULL, x, y, accX, accY, decX, decY, 
			speedX, speedY, maxSpeedX, maxSpeedY, 0.0, false, 
			(int) x, (int) y, TILESIZE, TILESIZE, 
			&(game->animations[68]), true, 0, 0, BOUNCER_ENEMY, newState(NULL), 
			5, 3, 0, true, "", update_BOUNCER);
			
			ent = node->data;
			ent->state.stateless = false;
			ent->state.active = true;
            ent->state.attacking = false;
            ent->state.grounded = true;
			ent->jumpSpeedY = 0;
			
            //setup callback
            ent->callback[0] = (void_callback) bouncer_callback;
            
            //alarm test

			//drawing
			ent->frame_index = 0;
			ent->frame_max = ent->sprite->frames;
			ent->frame_duration = ent->sprite->delay;
            
            //bouncer's collision points
            ent->col_leftA = (SDL_Point) {0,4};
            ent->col_leftB = (SDL_Point) {0,12};
            ent->col_rightA = (SDL_Point) {15,4};
            ent->col_rightB = (SDL_Point) {15,12};
            ent->col_topA = (SDL_Point) {4,0};
            ent->col_topB = (SDL_Point) {12,0};
            ent->col_bottomA = (SDL_Point) {4,15};
            ent->col_bottomB = (SDL_Point) {12,15};
			//player's special points
			ent->top_left = (SDL_Point) {0,0};
			ent->top = (SDL_Point) {7,0};
			ent->top_right = (SDL_Point) {15,0};
			ent->left = (SDL_Point) {0,7};
			ent->center = (SDL_Point) {7,7};
			ent->right = (SDL_Point) {15,7};
			ent->bottom_left = (SDL_Point) {0,15};
			ent->bottom = (SDL_Point) {7,15};
			ent->bottom_right = (SDL_Point) {15,15};
			
			//set rect
			ent->rect.x = (int) ent->x;
			ent->rect.y = (int) ent->y;
			ent->image_rect.x = ent->rect.x;
			ent->image_rect.y = ent->rect.y;
		    break;
		}
		
		default:
		{
			node = NULL;
		}
		
	}//end switch
	if (node != NULL) {
		//printf("node not null.");
		/** get your brand-new shiny id you special entity, you! **/
		if (ent->id == 0)
		{
			issue_id(&(ent->id), &(game->next_id));
		}
		
		int sz = ent->sprite->size;
                      
		ent->image = SDL_CreateTexture(game->gRenderer,
                                       SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_TARGET,
                                       sz*TILESIZE, sz*TILESIZE);
        //prepare a clear texture
        SDL_SetRenderTarget(game->gRenderer,ent->image);
        SDL_SetTextureBlendMode(ent->image, SDL_BLENDMODE_BLEND);
        //preserve render color
        SDL_Color col;
        SDL_GetRenderDrawColor(game->gRenderer, &col.r,&col.g,&col.b,&col.a);
        SDL_SetRenderDrawColor(game->gRenderer,0,0,0,0); //totally transparent
        SDL_RenderClear(game->gRenderer);
        //restore color and target
        SDL_SetRenderDrawColor(game->gRenderer,col.r,col.g,col.b,col.a);
        SDL_SetRenderTarget(game->gRenderer, 0);
        
	}//end if node not null
			                       
	return node;
}

void gamePause(GAME* game)
{
}

void gameOver(GAME* game)
{
	SDL_Event* event = &(game->ev);
    
	SDL_SetRenderDrawColor(game->gRenderer, 100, 20, 50, 255);
	SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
    
    if (game->points < 30) {
        Mix_HaltMusic();
    }
    
	if (game->texture_rect) {
        if (game->texture_rect[0].x > game->screen_rect.w)
        {
            game->texture_rect[0].x = -game->texture_rect[0].w;
        }
        else
        {
            game->texture_rect[0].x += 2;
        }
        
        if (event->type)
        {
            if (event->type == SDL_KEYDOWN)
            {
                if (event->key.keysym.sym == SDLK_RETURN)
                {
                    game->mode = EXIT;
                }
            }
        }//end if game->event != NULL
    }//end if textures exist
}//end function

void gameMap(GAME* game)
{
}

//
void gameRender(GAME* game, double fraction)
{	
	//render all things here.
	switch (game->mode)
	{
		case INTRO:
		{
			SDL_SetRenderDrawColor(game->gRenderer, 0, 0, 0, 255);
	        SDL_RenderClear(game->gRenderer);
	        SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
			//make sure to fix this so we aren't creating/freeing billion times
			SDL_Color color = {0, 128, 255};
			SDL_Surface* title_surface;
			SDL_Rect title_rect;
			SDL_Texture* title_texture;
			SDL_Rect temp_headicon_rect = game->texture_rect[0];
			temp_headicon_rect.x += 2*fraction;
			if (!(title_surface = TTF_RenderText_Solid(game->font1, "Collect All 30 Gems", color)))
			{
				printf("TTF_RenderText_Solid: %s\n", TTF_GetError());
				exit(1);
			}

			title_texture = SDL_CreateTextureFromSurface(game->gRenderer, title_surface);
			title_rect.w = title_surface->w;
			title_rect.h = title_surface->h;
			title_rect.x = (game->screen_rect.w/2)-(title_rect.w/2);
			title_rect.y = (game->screen_rect.h/2)+title_rect.h;

			SDL_SetRenderDrawColor(game->gRenderer, 20, 20, 100, 255);
			SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
            
			SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
							game->current_tileset->tile_number[64],&(temp_headicon_rect));
			SDL_RenderCopy(game->gRenderer, title_texture, NULL, &title_rect);
			
			SDL_RenderPresent(game->gRenderer);
			SDL_DestroyTexture(title_texture);
			SDL_FreeSurface(title_surface);
					
			break;
		}
		case PLAY:
		{
            SDL_SetRenderDrawColor(game->gRenderer, game->bgcolor.r,
                                                    game->bgcolor.g,
                                                    game->bgcolor.b,
                                                                255);
			//SDL_RenderClear(game->gRenderer);
			int x = 0;
			int y = 0;
			int county = 0;
			int countx = 0;
			SDL_Rect draw_rect;
			draw_rect.w = TILESIZE;
			draw_rect.h = TILESIZE;
			NODE* node = NULL;
			
			//SDL_SetRenderDrawColor(game->gRenderer, 20, 20, 50, 255);
			//SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
            
			/********Do Palette Manipulations on Surfaces ********/
            /*manipulations done here are simply palette cycling,
             * because palette swaps (unless we do a lightning simulation
             * or something like that.. oooohh) are done prior to a level
             * being started.*/

             if (game->cycle_ticks > 100) {
                 //create new texture
                 SDL_DestroyTexture(game->current_tileset->image);
                 
                 //water
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 250, 247, 1);
                 //snow
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 252, 255, 1);
                 //lava
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 240, 246, 1);
                 //smoke
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 220, 223, 1);
                 //leaves
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 206, 207, 1);
                 //gems, etc
                 cyclePaletteRange(game->current_tileset->surface_image, 
                                    game->current_palette, 19, 31, 1);
                game->current_tileset->image =
                    SDL_CreateTextureFromSurface(game->gRenderer,
                                  game->current_tileset->surface_image);
                 //reset
                 game->cycle_ticks = 0;
             }

            /******DRAW THE LEVEL*********/
			//set render target to the level's render texture.
			SDL_SetRenderTarget(game->gRenderer, game->current_level->targetRender);
            SDL_RenderClear(game->gRenderer);
			/*********************************************************/
			//SDL_SetRenderDrawColor(game->gRenderer, 20, 20, 50, 255);
			//SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
			//draw tiles of the game...

			//layer one of tiles
			county = 0;
			//this for loop ensures a tile drawn one block above and below view when drawing.
			for (y = 0; y < game->current_level->height;y++)
			{
				countx = 0;
				//this for loop does the same for the left and right.
				for (x = 0; x < game->current_level->width;x++)
				{                                                 
					int16_t tile0 = game->current_level->plot_map[y][x].T0;
					int16_t tile1 = game->current_level->plot_map[y][x].T1;
					draw_rect.x = countx*(TILESIZE);
					draw_rect.y = county*(TILESIZE);
					//draw tiles to map here.
					if (game->T0visible)
					{
						if (tile0 > -1)
						{
							SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
							game->current_tileset->tile_number[tile0], &draw_rect);
						}	
				    }
				    if (game->T1visible)
					{
						if (tile1 > -1)
						{
							SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
							game->current_tileset->tile_number[tile1], &draw_rect);
						}	
				    }
					countx++;
				}//end for x
				county++;
			}//end for y
			
            /******************Render Entities to the Screen *********/
            
			//draws each entity if it is active and visible.
			node = game->entities->firstNode->next;
			while (node != NULL)
			{
				ENTITY* ent = node->data;
				
				if (ent->state.active == true)//ent->state.active == true
				{	
					if (ent->visible == true)//ent->visible == true
					{
						
						SDL_RenderCopyEx(game->gRenderer, ent->image, NULL,
						&(ent->image_rect), ent->image_angle, NULL, SDL_FLIP_NONE);
						
					}
				}
				
				if (false)//game->free_camera
				{
					SDL_SetRenderDrawColor(game->gRenderer, 255, 0, 0, 255);
					SDL_RenderDrawRect(game->gRenderer, &(ent->rect)); // collision rect
					SDL_SetRenderDrawColor(game->gRenderer, 127, 0, 0, 255);
					SDL_RenderDrawRect(game->gRenderer, &(ent->prev_rect)); // previous collision rect location
					SDL_SetRenderDrawColor(game->gRenderer, 0, 255, 0, 255);
					//SDL_RenderDrawRect(game->gRenderer, &(ent->image_rect)); // image rect
					
				}
				node = node->next;
			}
			
			/************** Tile layer 2 *************/
			if (game->T2visible)
			{
				//layer two of tiles
				county = 0;
				//this for loop ensures a tile drawn one block above and below view when drawing.
				for (y = 0; y < game->current_level->height;y++)
				{
					countx = 0;
					//this for loop does the same for the left and right.
					for (x = 0; x < game->current_level->width;x++)
					{   
						//SDL_SetTextureAlphaMod(game->current_tileset->image, game->T2opacity);
						//make sure this exists (possibility of being out of bounds)                                                
						int16_t tile2 = game->current_level->plot_map[y][x].T2;
						draw_rect.x = countx*(TILESIZE);
						draw_rect.y = county*(TILESIZE);
						//draw tiles to map here.
						if (tile2 != -1)
						{
							SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
							game->current_tileset->tile_number[tile2], &draw_rect);
						}
						//SDL_SetTextureAlphaMod(game->current_tileset->image, 255);
						countx++;
					}//end for x
					county++;
				}//end for y
			}// end if T2 is visible
			
			/*******************Set up to Render to Screen ***********/
			SDL_SetRenderTarget(game->gRenderer, NULL);
			SDL_RenderCopyEx(game->gRenderer, game->current_level->targetRender,
			                 &(game->view_rect), &(game->screen_rect), game->screen_angle,
			                 NULL, game->flip);

			SDL_RenderPresent(game->gRenderer);
			
			/********************************************************/

			break;
		}
		case OVER:
		{			
            char str[100];
            SDL_SetRenderDrawColor(game->gRenderer, 0, 0, 0, 255);
	        SDL_RenderClear(game->gRenderer);
	        SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
			//make sure to fix this so we aren't creating/freeing billion times
			SDL_Color color = {255, 255, 255};
			SDL_Surface* title_surface;
			SDL_Rect title_rect;
			SDL_Texture* title_texture;
			SDL_Rect temp_headicon_rect = game->texture_rect[0];
			temp_headicon_rect.x += 2*fraction;
            if (game->points == 30) {
                sprintf(str, "You Collected All The Gems You Win");
            }
            else {
                sprintf(str, "You Collected %d out of 30 Gems Too bad", game->points);
            }
			if (!(title_surface = TTF_RenderText_Solid(game->font1, str, color)))
			{
				printf("TTF_RenderText_Solid: %s\n", TTF_GetError());
				exit(1);
			}

			title_texture = SDL_CreateTextureFromSurface(game->gRenderer, title_surface);
			title_rect.w = title_surface->w;
			title_rect.h = title_surface->h;
			title_rect.x = (game->screen_rect.w/2)-(title_rect.w/2);
			title_rect.y = (game->screen_rect.h/2)+title_rect.h;

			SDL_SetRenderDrawColor(game->gRenderer, 100, 20, 80, 255);
			SDL_RenderFillRect(game->gRenderer, &(game->screen_rect));
            
			SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
							game->current_tileset->tile_number[64],&(temp_headicon_rect));
			SDL_RenderCopy(game->gRenderer, title_texture, NULL, &title_rect);
			
			SDL_RenderPresent(game->gRenderer);
			SDL_DestroyTexture(title_texture);
			SDL_FreeSurface(title_surface);
            break;
            }
		case PAUSE:
		{break;}
		case MAP:
		{break;}
		case CTRL:
		{break;}
		case EXIT:
		{break;}
		default:
		{}
	}//end switch game mode
}

STATE newState(STATE* s)
{
	STATE tmp;
	tmp.stateless = true;
	tmp.standing = false;
	tmp.jumping = false;
	tmp.flying = false;
	tmp.ducking = false;
	tmp.walking = false;
	tmp.running = false;
	tmp.swimming = false;
	tmp.attacking = false;
	tmp.dying = false;
	tmp.hurting = false;
	tmp.recovering = false;
	tmp.falling = false;
	tmp.fleeing = false;
	tmp.winning = false;
	
	if (s != NULL)
	{
		*s = tmp; //copy tmp into s.
    }
    
    return tmp;   
}

ENTITY* newEntity()
{
	int loop = 0;
	
    ENTITY* ent = malloc(sizeof(ENTITY));
    //physics
    ent->x = 0.0;
    ent->y = 0.0;
    ent->accX = 0;
    ent->accY = 0;
    ent->decX = 0;
    ent->decY = 0;
    ent->speedX = 0;
    ent->speedY = 0;
    ent->maxSpeedX = 0;
    ent->maxSpeedY = 0;
    ent->angle = 0;
    ent->solid = false;
    ent->rect.x = 0;
    ent->rect.y = 0;
    ent->rect.w = 0;
    ent->rect.h = 0;
    
    //drawing
    ent->sprite = NULL;
    ent->visible = true;
    ent->image_angle = 0.0;
    ent->frame_index = 0;
    ent->frame_max = 0;
    ent->frame_duration = 0;
    ent->frame_ticks = 0;
    ent->image = NULL;
    
    //logic
    ent->id = 0;
    ent->kind = NONE;
    ent->state = newState(NULL);
    ent->previous_state = newState(NULL);
    for (loop = 0; loop < sizeof(ent->alarm_set)/sizeof(bool); loop++)
    {
		ent->alarm_set[loop] = false;
	}
    for (loop = 0; loop < sizeof(ent->alarm)/sizeof(double); loop++)
    {
		ent->alarm[loop] = 0;
	}
    for (loop = 0; loop < sizeof(ent->callback)/sizeof(void_callback); loop++)
    {
		ent->callback[loop] = 0;
	}
    //stats
    ent->hits = 0;
    ent->lives = 0;
    ent->points = 0;
    ent->alive = 1;
    
    //text, messages, etc
    strcpy(ent->text, "");
    
    //this is a union, so make sure that we've got the biggest.
    ent->aux = 0;
    
    return ent;
}

NODE* initEntity(NODE* node, float x, float y, float accX, 
                  float accY, float decX, float decY,
                  float speedX, float speedY, float maxSpeedX, float maxSpeedY,
                  double angle, bool solid, int rx, int ry, int rw, int rh,
                  ANIMATION *sprite, bool visible, double image_angle,
                  int frame_index, KIND kind, STATE state, int hits, 
                  int lives, int points, bool alive, char text[160],
                  void (*funct)(void*, void*))
{
    NODE* tmp = NULL;
    ENTITY* ent = newEntity();
    ent->update = funct; //function pointer...
    //physics
    ent->solid = solid;
    ent->x = x;
    ent->y = y;
    ent->accX = accX;
    ent->accY = accY;
    ent->decX = decX;
    ent->decY = decY;
    ent->speedX = speedX;
    ent->speedY = speedY;
    ent->maxSpeedX = maxSpeedX;
    ent->maxSpeedY = maxSpeedY;
    ent->angle = angle;
    
    //****rect
    ent->rect.x = rx;
    ent->rect.y = ry;
    ent->rect.w = rw;
    ent->rect.h = rh;
    ent->prev_rect.x = rx;
    ent->prev_rect.y = ry;
    ent->prev_rect.w = rw;
    ent->prev_rect.h = rh;
    
    //SDL_Point's of rect
	ent->top_left.x = 0;
	ent->top_left.y = 0;
	ent->top.x = 7;
	ent->top.y = 0;
	ent->top_right.x = 15;
	ent->top_right.y = 0;
	ent->left.x = 0;
	ent->left.y = 7;
	ent->center.x = 7;
	ent->center.y = 7;
	ent->right.x = 15;
	ent->right.y = 7;
	ent->bottom_left.x = 0;
	ent->bottom_left.y = 15;
	ent->bottom.x = 7;
	ent->bottom.y = 15;
	ent->bottom_right.x = 15;
	ent->bottom_right.y = 15;
    
    //drawing
    ent->sprite = sprite;
    ent->image = NULL; //create texture outside...
    ent->visible = visible;
    ent->image_angle = image_angle;
    ent->frame_index = frame_index;
    ent->frame_max = sprite->frames;
    ent->frame_duration = sprite->delay;
    ent->frame_ticks = 0;
    ent->image_rect.w = (sprite->size)*TILESIZE;
    ent->image_rect.h = (sprite->size)*TILESIZE;
    ent->image_rect.x = x;
    ent->image_rect.y = y;
 
    //logic
    ent->kind = kind;
    ent->state = state;
    ent->previous_state = state;
    ent->hits = hits;
    ent->lives = lives;
    ent->points = points;
    ent->alive = alive;
    if (!(strcmp(ent->text, ""))) //i.e. the name is blank
    {
        strcpy(ent->text, text);
    }
    if (node != NULL)
    {
        node->data = ent;
        return node;
    }
    else
    {
        tmp = new_node();
        tmp->data = ent;
        return tmp;
    }
}

ENTITY* cloneEntity(ENTITY* prototype)
{
    //TODO: check for unique id
	ENTITY* new_entity = malloc(sizeof(ENTITY));
	if (new_entity && prototype)
	{
	    *new_entity = *prototype;
	    return new_entity;
    }
    return NULL;
}



void next_level(GAME* game)
{
	if (game->current_level != NULL)
	{
		NODE* current_node = find_list(game->levels, 
							 lvl_indexmatch, &(game->current_level->index));
		if (current_node->next != NULL)
		{
			game->current_level = current_node->next->data;
			game->view_rect.x = 0;
			game->view_rect.y = 0;
			delete_all_entities(game); //clear the entities list and then
	        gamePopulate(game);        //add all the objects from the new level.
		}
		else
		{
			game->current_level = game->levels->firstNode->next->data;
			game->view_rect.x = 0;
			game->view_rect.y = 0;
			delete_all_entities(game); //clear the entities list and then
	        gamePopulate(game);       //add all the objects from the new level.
		}
	}//end if game->current_level != NULL
}//end function next_level

void play_level_music(GAME* game)
{
    int level_ind = game->current_level->index;
    
    switch (level_ind) {
        case 0: Mix_PlayMusic(game->music[0], -1); break;
        default: Mix_HaltMusic();
    }
}

void next_tileset(GAME* game)
{
	if (game->current_tileset != NULL)
	{
		NODE* current_node = find_list(game->tilesets, 
							 tset_indexmatch, &(game->current_tileset->index));
		if (current_node->next != NULL)
		{
			game->current_tileset = current_node->next->data;
		}
		else
		{
			game->current_tileset = game->tilesets->firstNode->next->data;
		}
	}//end if game->current_tileset!= NULL
}//end function next_tileset

void issue_id(uint32_t* id, uint32_t* pool)
{
	if ((id != NULL) && (pool != NULL))
	{
		(*id) = (*pool); //get id from the id pool.
		(*pool)++;
	}
}
