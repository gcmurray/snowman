#include <SDL2/SDL.h>
#include "macro.h"
#include "structs.h"
#include "update.h"
#include <math.h>
#include <stdbool.h>

extern float FRAMES_PER_SEC;
extern double PI;
extern double ONE_OVER_180;

int update_entities(void* node, void* game)
{
	/* use node to perform list walk through.*/
	int count = 0;
    NODE* tmp = (NODE*) node; //recast (i don't think the cast is neccessary?)
	while (tmp != NULL)
	{
		ENTITY* e = tmp->data;
		if (e->alive)
		{
			
			e->update(tmp->data, game);
			count++;
			tmp=tmp->next;
		}
	}
        
    return count;
}

void discard_entities(LIST* lst)
{
	ENTITY* ent = NULL;
	NODE* prev_node = lst->firstNode;
	NODE* node = lst->firstNode->next;
	
	while (node != NULL)
	{
		ent = (ENTITY*) node->data; //do I need to explicit cast?
		if (ent->alive)
		{
			prev_node = node;
			node = node->next;
		}
		else
		{
			//free entity image texture
			printf("buried another entity! \n");
			SDL_DestroyTexture(ent->image);
			prev_node = del_node(lst, prev_node, node);
			node = prev_node->next;
		}
	} //end while
}//end function discard_entities

void delete_all_entities(GAME* game)
{
	
	NODE* node = NULL;
	ENTITY* ent = NULL;
	
	/** entity clearing **/
	node = game->entities->firstNode->next;
	while (node != NULL)
	{
		ent = node->data;
		if (ent != NULL)
		{
			printf("destroying image of ent...\n");
			SDL_DestroyTexture(ent->image);
		}
		else
		{
			printf("ent is null whilst deleting!\n");
		}
		//um, going to the next node is important. lol.
		node = node->next;
	}
	printf("deleting all entities in list!\n");
	clear_list(game->entities);
	/*************************************/
}
/***************************************************************/
// Entity Update Functions
/***************************************************************/
void update_PLAYER1(void* ent, void* game)
{
	ENTITY* e = ent;
	GAME* g = game;
	int collisions = 0;
	bool moveRequest = false;
	bool jumpPressed = false;
    
    /*********************************/
    //update alarms
    /*********************************/
    update_alarms(ent, game);
	
    /*********************************/
    //update sprite animations
    /*********************************/
    /*
    if (e->frame_duration > 0) {
        e->frame_ticks += g->currentTick - g->lastTick;
        if (e->frame_ticks > e->frame_duration) {
            e->frame_index++;
            if (e->frame_index > e->frame_max) {
                if (e->state.dying) {
                    e->frame_index = e->frame_max;
                    e->frame_duration = 0;
                }
                else {
                    e->frame_index = 0;
                }
            }
        }
    }//end sprite animate
    */
    
	//KEY CHECK
	/*********************************/
	if (!(g->free_camera) && e->state.interactive)
	{
		if (g->key_state[SDL_SCANCODE_X] && 
            !g->key_state[SDL_SCANCODE_DOWN] && !jumpPressed)
		{
			if (e->state.grounded == true)
			{
				e->state.grounded = false;
				jumpPressed = true;
				apply_jump(&(e->speedX), &(e->speedY), e->jumpSpeedY, 
				g->gravityDirection, g->currentTick, g->lastTick);
                
                if (Mix_PlayChannel(-1, g->effect[1], 0) == -1) {
                    printf("Mix_PlayChannel: %s\n",Mix_GetError());
                } 
				//e->speedY = -e->jumpSpeedY;
				
			}
		}//end if pressing jump
		if (!g->key_state[SDL_SCANCODE_X])
		{
			jumpPressed = false;
		}
	
		if (g->key_state[SDL_SCANCODE_RIGHT])
		{
			moveRequest = true;
            //tracking left and right for sprites
			e->state.solid_left = false;
            e->state.solid_right = true;
            
			if (g->key_state[SDL_SCANCODE_Z] && (e->state.grounded == true))
			{//run effect
				e->speedX += e->accX + 0.2 * FRAMES_PER_SEC;
			}
			else
			{
				e->speedX += e->accX;
			}
			
		}//end if pressing right
		
		if (g->key_state[SDL_SCANCODE_LEFT])
		{
			moveRequest = true;
            //tracking left and right for sprites
			e->state.solid_left = true;
            e->state.solid_right = false;
            
			if (g->key_state[SDL_SCANCODE_Z] && (e->state.grounded == true))
			{//run effect
				e->speedX -= e->accX + 0.2 * FRAMES_PER_SEC;
			}
			else
			{
				e->speedX -= e->accX;
			}
		}//end if pressing left
	} //end if not in free_camera mode or interactive
	
	/*********************************/
	//LIMIT SIDEWAYS SPEED
	/*********************************/
	if (e->speedX > e->maxSpeedX) e->speedX = e->maxSpeedX;
	if (e->speedX < -(e->maxSpeedX)) e->speedX = -(e->maxSpeedX);
	/*********************************/
	//APPLY GRAVITY
	/*********************************/
    if (e->state.grounded == false) {
        apply_gravity(&(e->speedX), &(e->speedY), e->accY, 
	              g->gravityDirection, g->currentTick, g->lastTick);
    }
    //limit gravity
	if (e->speedY > e->maxSpeedY) e->speedY = (e->maxSpeedY);
	/*********************************/
	//DEACCELERATE (if no sideways motion requested)
	/*********************************/
	if (!moveRequest) {
		if (e->speedX < 0) e->speedX += e->decX;
		if (e->speedX > 0) e->speedX -= e->decX;
		//fix little motions
		if (e->speedX > 0 && e->speedX < e->decX) e->speedX = 0;
		if (e->speedX < 0 && e->speedX > -(e->decX)) e->speedX = 0;
	}
    /*********************************/
	//PREDICTED LINEAR MOTIONS
	/*********************************/
	e->x += apply_linear_motion(e->speedX, g->currentTick, g->lastTick);
	e->y += apply_linear_motion(e->speedY, g->currentTick, g->lastTick);
    e->rect.x = (int) roundf(e->x);
    e->rect.y = (int) roundf(e->y);
    /*********************************/
	//COLLISION DETECTION
	/*********************************/
	collisions = update_player_collision(g, e, g->entities, g->current_level,  g->key_state);
    /*********************************/
    
    /********************moving left/right ***********/
    if (!(e->state.hurting || e->state.dying)) {
        
        if (e->state.solid_left && g->power == 0) {
            e->sprite = &g->animations[41];
            e->frame_duration = 0;
            e->frame_index = 0;
            e->frame_max = e->sprite->frames;
        }
        else if (e->state.solid_left && g->power == 1) {
            e->sprite = &g->animations[47];
            e->frame_duration = 0;
            e->frame_index = 0;
            e->frame_max = e->sprite->frames;
        }
        else if (e->state.solid_left && g->power == 2) {
            e->sprite = &g->animations[53];
            e->frame_duration = e->sprite->delay;
            e->frame_max = e->sprite->frames;
            if (e->frame_index > e->frame_max) {
                e->frame_index = 0;
            }
        }
        else if (e->state.solid_right && g->power == 0) {
            e->sprite = &g->animations[45];
            e->frame_duration = 0;
            e->frame_index = 0;
            e->frame_max = e->sprite->frames;
        }
        else if (e->state.solid_right && g->power == 1) {
            e->sprite = &g->animations[50];
            e->frame_duration = 0;
            e->frame_index = 0;
            e->frame_max = e->sprite->frames;
        }
        else if (e->state.solid_right && g->power == 2) {
            e->sprite = &g->animations[54];
            e->frame_duration = e->sprite->delay;
            e->frame_max = e->sprite->frames;
            if ((e->frame_index > e->frame_max) || 
                (e->frame_ticks > e->frame_duration)) {
                e->frame_index = 0;
                e->frame_ticks = 0;
            }
        }//end else if
    }//end the not hurting or dying

        /*********** turn off animations of walking *******/
        /*
    if (!moveRequest) {
        e->frame_duration = 0;
        e->frame_index = 0;
        e->frame_ticks = 0;
    }*/
    /***********change sprite for hurting*************/
    /*
    if (e->state.hurting) {
        if (e->state.solid_right) {
            e->sprite = &g->animations[44]; //hurt right
        }
        else {
            e->sprite = &g->animations[43]; //hurt left
        }
        
        if (!e->previous_state.hurting) {
            //make start from the beginning.
            e->frame_index = 0;
            e->frame_duration = e->sprite->delay;
            e->frame_max = e->sprite->frames;
            e->frame_ticks = 0;
            //reset collision rect to small rect
            e->rect = (SDL_Rect) {(int) e->x, (int) e->y, 16, 16};
            //reset collision points for small rect
            e->col_bottomA.y -= 15;
            e->col_bottomB.y -= 15;
            e->col_leftA.y -= 4;
            e->col_leftB.y -= 8;
            e->col_rightA.y -= 4;
            e->col_rightB.y -= 8;
            e->image_rect = (SDL_Rect) {e->image_rect.x, e->image_rect.y,
                        e->sprite->image_rect.w, e->sprite->image_rect.h};
        }
    } */
        /********** change sprite for death **********/
        /*
    if (e->state.dying && !(e->previous_state.dying)) {
        if (e->state.solid_right) {
            if (g->power > 0) {
                e->sprite = &g->animations[49];
            }
            else {
                e->sprite = &g->animations[44];
            }
        }//end if right
        else {
            if (g->power > 0) {
                e->sprite = &g->animations[48];
            }
            else {
                e->sprite = &g->animations[43];
            }
        }//end else left
        
        //make start from the beginning.
        e->frame_index = 0;
        e->frame_duration = e->sprite->delay;
        e->frame_max = e->sprite->frames;
        e->frame_ticks = 0;
        e->image_rect = (SDL_Rect) {e->image_rect.x, e->image_rect.y,
                        e->sprite->image_rect.w, e->sprite->image_rect.h};
        
    }//end if dying
    */
    /******************** END SPRITE UPDATES ***********/
   
    
    //TODO: fix pic rect. this is not necessarily true, but temp
	e->image_rect.x = e->rect.x;
	e->image_rect.y = e->rect.y;
	
    /***store previous state and rect***/
    e->previous_state = e->state;
    e->prev_rect = e->rect;
    
	//draw the image of the entity...
	drawEntity(e, g); //draw it on to the ent's image from sprite.
	
}

void update_BLOCK(void* ent, void* game)
{
	ENTITY* e = ent;
	GAME* g = game;
	bool check = false;
	
	check = collision_point(&(e->top_left), &(g->view_rect));
	if (check)
	{
		e->state.active = true;
	}
	else
	{
		e->state.active = false;
	}
	
	drawEntity(e, g); //draw that block! usually invisible.	
}

void update_BOUNCER(void* ent, void* game)
{
    ENTITY* e = ent;
	GAME* g = game;
	int collisions = 0;
    
    /*********************************/
    //update alarms
    /*********************************/
    update_alarms(ent, game);
	/*********************************/
	//LIMIT SIDEWAYS SPEED
	/*********************************/
	if (e->speedX > e->maxSpeedX) e->speedX = e->maxSpeedX;
	if (e->speedX < -(e->maxSpeedX)) e->speedX = -(e->maxSpeedX);
	/*********************************/
	//APPLY GRAVITY
	/*********************************/
    apply_gravity(&(e->speedX), &(e->speedY), e->accY, 
	              g->gravityDirection, g->currentTick, g->lastTick);
    //limit gravity
	if (e->speedY > e->maxSpeedY) e->speedY = (e->maxSpeedY);
	/*********************************/
	//DEACCELERATE (if bouncing done)
	/*********************************/
	if (e->jumpSpeedY < 1) {
		if (e->speedX < 0) e->speedX += e->decX;
		if (e->speedX > 0) e->speedX -= e->decX;
		//fix little motions
		if (e->speedX > 0 && e->speedX < e->decX) e->speedX = 0;
		if (e->speedX < 0 && e->speedX > -(e->decX)) e->speedX = 0;
	}
    /*********************************/
	//PREDICTED LINEAR MOTIONS
	/*********************************/
	e->x += apply_linear_motion(e->speedX, g->currentTick, g->lastTick);
	e->y += apply_linear_motion(e->speedY, g->currentTick, g->lastTick);
    e->rect.x = (int) roundf(e->x);
    e->rect.y = (int) roundf(e->y);
    /*********************************/
	//COLLISION DETECTION
	/*********************************/
	collisions = update_bouncer_collision(e, g->entities, g->current_level);
    /*********************************/
    
    //TODO: fix pic rect. this is not necessarily true, but temp
	e->image_rect.x = e->rect.x;
	e->image_rect.y = e->rect.y;
	
    /***store previous state and rect***/
    e->previous_state = e->state;
    e->prev_rect = e->rect;
    
	//draw the image of the entity...
	drawEntity(e, g); //draw it on to the ent's image from sprite.
}//end update bouncer function.

/*******************************************************************/
// Collision Functions
/*******************************************************************/

int update_player_collision(GAME* game, ENTITY* ent, LIST* lst, LEVEL* lvl, const uint8_t* key_state)
{
	//very basic, aabb.
	NODE* node = lst->firstNode->next;
	int count = 0;
    int i = 0;
    int j = 0;
    SDL_Rect rerect = {0,0,0,0};
	KIND kcheck = NONE;
	
    //tile collision
    /*get tiles that need collision checked as a SDL_Rect:
     * {first tile x, first tile y, last tile x, last tile y}
     * loop through, check these for solid code.
     * if solid, check what speed is (direction, left or right.)
     * then check if point for that direction is in the tile box.
     * if it is, set x speed to zero, and set entity position so
     * that it subtracts/adds the overlap of the rect and the tile
     * width. For example, player going left, check tiles,
     * one is solid on the left. Speed is negative (left) so check
     * if the rect intersects with a rect where that tile is. It
     * intersects, so take the width of that intersection, add it to
     * the player position, set the player speed to zero.
     */
     SDL_Rect range = get_tile_collisions(ent->rect, lvl);
     
     TILE code = T_EMPTY;
     bool has_ground = false;
     bool collide_right = false;
     bool collide_left = false;
     bool collide_up = false;
     bool collide_down = false;
     
     //it's over if kelvin is below the level.
     if ((ent->state.dying == false) && (ent->y > game->current_level->target_rect.h)) {
         ent->state.dying = true;
         ent->state.interactive = false;
         game->lives -= 1;
         //death noise, etc.
         ent->alarm_set[1] = true;
         ent->alarm[1] = 1000;
     }
     ///CHECK THE COLLISIONS///
     if (ent->speedY > 0 || ent->state.grounded) {//moving down or on ground(?)
         for (i = range.x;i <= range.w;i++) {
             code = (TILE) lvl->plot_map[range.h][i].code;
             SDL_Rect tile_rect = {i*TILESIZE, range.h*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID || code == T_ONEWAY_UP || code == T_GEM_BREAK ||
                 code == T_SNOWPILE) {
                 SDL_Point player_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(player_origin,ent->col_bottomA);
                 SDL_Point pointB = add_points(player_origin,ent->col_bottomB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    if (code == T_ONEWAY_UP || code == T_SNOWPILE) {
                        if ((ent->prev_rect.y + ent->prev_rect.h + 1) > tile_rect.y) {
                            if (key_state[SDL_SCANCODE_DOWN] && key_state[SDL_SCANCODE_X]){
                                if (false && code == T_SNOWPILE && game->power == 0) {//real artists ship.
                                    //remove the power up from map
                                    lvl->plot_map[range.h][i].code = T_EMPTY;
                                    lvl->plot_map[range.h][i].T1 = -1;
                                    lvl->plot_map[range.h-1][i].T1 = -1;
                                    //give me some info
                                    printf("powered up!");
                                    //play a diddy
                                    Mix_PlayChannel(-1, game->effect[7],0);
                                    //set some vars
                                    game->power = 1;
                                    ent->state.state0 = true; //hmm.. i was using this for something
                                    //set rect to larger version.
                                    ent->rect = (SDL_Rect) {ent->rect.x, ent->rect.y, ent->rect.w, 30};
                                    ent->col_bottomA.y += 15;
                                    ent->col_bottomB.y += 15;
                                    ent->col_leftA.y += 4;
                                    ent->col_leftB.y += 8;
                                    ent->col_rightA.y += 4;
                                    ent->col_rightB.y += 8; 
                                    ent->image_rect = (SDL_Rect) {ent->image_rect.x, ent->image_rect.y,
                                                                ent->sprite->image_rect.w, ent->sprite->image_rect.h};
                                } 
                                else {
                                    ent->y += 4;
                                    ent->rect.y = (int) ent->y;
                                    ent->state.grounded = false;
                                }
                            }
                            else {
                                collide_down = true;
                            }
                        }
                    }
                    else {
                            collide_down = true;
                    }
                 }//end a or b
             }//end code equ solid or oneway up etc
         }//end for
         if (collide_down) {
            ent->y = (range.h*TILESIZE) - ent->rect.h; //leave in one pixel
            ent->speedY = 0;
            ent->rect.y = (int) ent->y;
            ent->state.grounded = true;
         }
         else if (ent->state.grounded == true) {
            ent->state.grounded = false;
         }
     }//end moving down/ground check
     
     else if (ent->speedY < 0) {//moving up
         for (i = range.x;i <= range.w;i++) {
             code = (TILE) lvl->plot_map[range.y][i].code;
             SDL_Rect tile_rect = {i*TILESIZE, range.y*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID || code == T_ONEWAY_DOWN || code == T_GEM_BREAK) {
                 SDL_Point player_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(player_origin,ent->col_topA);
                 SDL_Point pointB = add_points(player_origin,ent->col_topB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                     if (code == T_ONEWAY_DOWN) {
                        if ((ent->prev_rect.y - 1) 
                                >= (tile_rect.y + tile_rect.h)) {
                                    collide_up = true;
                        }
                    }//end owd
                    else {
                        collide_up = true;
                    }
                }//end a or b
             }//end if solid, owd, gem...

         }//end for
         if (collide_up) {
            ent->y = (range.y+1) * TILESIZE;
            ent->speedY = 0;
            ent->rect.y = (int) ent->y;
         }
     }//end for speedY < 0
     if (ent->speedX < 0) {//moving left
         for (i = range.y;i <= range.h;i++) {
             code = (TILE) lvl->plot_map[i][range.x].code;
             SDL_Rect tile_rect = {range.x*TILESIZE, i*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID || code == T_GEM_BREAK || 
                 code == T_GATE1  || code == T_GATE2 || 
                 code == T_LOCKED_ENDLEVEL) {
                 SDL_Point player_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(player_origin,ent->col_leftA);
                 SDL_Point pointB = add_points(player_origin,ent->col_leftB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    if (code == T_GATE1 && game->smallkey > 0) {
                        int k;
                        for (k = 0;k < lvl->height;k++) {
                            //clear entire column of any gate1.
                            if (lvl->plot_map[k][range.x].code == T_GATE1) {
                                lvl->plot_map[k][range.x].code = T_EMPTY;
                                lvl->plot_map[k][range.x].T1 = -1;
                            }//end if
                        }//end for
                    }//end if gate1 and small key
                    else {
                        collide_left = true;
                    }
                 }//end a or b
             }//end all the stuffs
         }//end for left
         if (collide_left) {
            ent->x = (range.x+1) * TILESIZE;
            ent->speedX = 0;
            ent->rect.x = (int) ent->x;
         }
     }//end moving left
     
     else if (ent->speedX > 0) {//moving right
         for (i = range.y;i <= range.h;i++) {
             code = (TILE) lvl->plot_map[i][range.w].code;
             SDL_Rect tile_rect = {range.w*TILESIZE, i*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID || code == T_GEM_BREAK || 
                 code == T_GATE1 || code == T_GATE2 || code == T_LOCKED_ENDLEVEL) {
                 SDL_Point player_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(player_origin,ent->col_rightA);
                 SDL_Point pointB = add_points(player_origin,ent->col_rightB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    if (code == T_GATE1 && game->smallkey > 0) {
                        int k;
                        for (k = 0;k < lvl->height;k++) {
                            //clear entire column of any gate1.
                            if (lvl->plot_map[k][range.w].code == T_GATE1) {
                                lvl->plot_map[k][range.w].code = T_EMPTY;
                                lvl->plot_map[k][range.w].T1 = -1;
                            }//end if
                        }//end for
                    }//end if gate1 and small key
                    else if (code == T_LOCKED_ENDLEVEL && game->bigkey) {
                        int k;
                        int l;
                        for (k = 0;k < lvl->height;k++) {
                            for (l = 0;l < lvl->width;l++) {
                                if (lvl->plot_map[k][l].code == T_LOCKED_ENDLEVEL) {
                                    //remove any locked end level gate
                                    lvl->plot_map[k][l].T1 = -1;
                                    lvl->plot_map[k][l].code = T_EMPTY;
                                }//end if
                            }//end for width
                        }//end for height
                    }//end if endlevel and big key
                    else {
                        collide_right = true;
                    }
                 }//end a or b
             }//end solid,gembreak, gate..etc

         }//end for right
         if (collide_right) {
            ent->x = (range.w*TILESIZE) - ent->rect.w;
            ent->speedX = 0;
            ent->rect.x = (int) ent->x;
         }
     }//end moving right
     
     //general collision, not direction specific
     for (i = range.y;i <= range.h;i++) {
         for (j = range.x; j<= range.w;j++) {
             code = (TILE) lvl->plot_map[i][j].code;
             
             if (code == T_SMALL_GEM) {
                 //clear image
                 lvl->plot_map[i][j].T1 = -1;
                 //clear code
                 lvl->plot_map[i][j].code = T_EMPTY;
                 //give points
                 game->points += 1;
                 printf("gems collected: %d\n", game->points);
                 if (game->points == 30) {
                        game->mode = OVER;
                 }
                 Mix_PlayChannel(-1, game->effect[0], 0);
             }
             else if (code == T_LARGE_GEM) {
                 //clear image
                 lvl->plot_map[i][j].T1 = -1;
                 //clear code
                 lvl->plot_map[i][j].code = T_EMPTY;
                 //give points
                 game->points += 1;
                 printf("gems collected: %d\n", game->points);
                 if (game->points == 30) {
                    game->mode = OVER;
                 }
                 Mix_PlayChannel(-1, game->effect[0], 3);
             }
             else if (code == T_1UP) {
                 //clear image
                 lvl->plot_map[i][j].T1 = -1;
                 //clear code.. rinse repeat
                 lvl->plot_map[i][j].code = T_EMPTY;
                 game->lives += 1;
                 Mix_PlayChannel(-1, game->effect[2], 3);
             }
             else if (code == T_KEY_SMALL) {
                 //clear image
                 lvl->plot_map[i][j].T1 = -1;
                 //clear code.. rinse repeat
                 lvl->plot_map[i][j].code = T_EMPTY;
                 game->smallkey += 1;
                 Mix_PlayChannel(-1, game->effect[2], 0);
             }
             else if (code == T_KEY_BIG) {
                 //clear image
                 lvl->plot_map[i][j].T1 = -1;
                 //clear code.. rinse repeat
                 lvl->plot_map[i][j].code = T_EMPTY;
                 game->bigkey = true;
                 Mix_PlayChannel(-1, game->effect[0], 3);
             }
             else if ((code == T_LAVAFALL || code == T_LAVAPOOL) &&
                        ent->state.dying == false) {
                 //this is death!!
                 ent->state.dying = true;
                 ent->state.interactive = false;
                 //maybe set an alarm?
                 game->lives -= 1;
                 Mix_PlayChannel(-1, game->effect[5], 0);
                //death noise, etc.
                ent->alarm_set[1] = true;
                ent->alarm[1] = 1000;
             }//end else if
         }//end for width of tiles collided
     }//end for height of tiles collided
     
	while (node != NULL)
	{
		
        
		ENTITY* ent2 = node->data;
		/** make sure collision is with different entity **/
		if (ent->id == ent2->id) //asking: are these are the same thing?
		{
			node = node->next;
			continue;
		}
		
		kcheck = ent2->kind;
		
		/** ok, we have a different entity, check for collision
         * BUT ONLY IF IT IS AN OBJECT WE CARE IF WE COLLIDED WITH!
         * Do this for all the other collision routines as well.
         * Not everything needs to collide with everything! */
         //TODO: set so that only collide with entities we care about
         //TODO: Implement tile-based lookup for primary collision
         // routines that deal with slopes, platforms, etc.
        if (kcheck == BOUNCER_ENEMY) {
            if (collision_rect(&ent->rect,&ent2->rect, &rerect, true)) 
            {
                Mix_PlayChannel(-1, game->effect[3], 0);
                if (!ent->state.hurting && ent2->state.attacking) {
                    printf("player hurt.\n");
                    ent->state.hurting = true;
                    if (game->power > 0) {
                        game->power = 0;
                    }
                    else {
                        game->lives -= 1;
                        ent->state.dying = true;
                        ent->state.interactive = false;
                        //death noise, etc.
                        ent->alarm_set[1] = true;
                        ent->alarm[1] = 1000;
                    }
                }//end if not hurting and other thing attacking
                else {
                    Mix_PlayChannel(-1, game->effect[4], 0);
                    ent->alarm_set[0] = true;
                    ent->alarm[0] = 1000;
                }
            }//end if collision rect
        }//end kind check
		
		node = node->next;
	}//while node != NULL
	
	return count;
	
}//end of function update_player collision.

int update_bouncer_collision(ENTITY* ent, LIST* lst, LEVEL* lvl)
{
	//very basic, aabb.
	NODE* node = lst->firstNode->next;
	int count = 0;
    int i = 0;
    int j = 0;
    SDL_Rect rerect = {0,0,0,0};
	KIND kcheck = NONE;
	
    //tile collision
    /*get tiles that need collision checked as a SDL_Rect:
     * {first tile x, first tile y, last tile x, last tile y}
     * loop through, check these for solid code.
     * if solid, check what speed is (direction, left or right.)
     * then check if point for that direction is in the tile box.
     * if it is, set x speed to zero, and set entity position so
     * that it subtracts/adds the overlap of the rect and the tile
     * width. For example, player going left, check tiles,
     * one is solid on the left. Speed is negative (left) so check
     * if the rect intersects with a rect where that tile is. It
     * intersects, so take the width of that intersection, add it to
     * the player position, set the player speed to zero.
     */
     SDL_Rect range = get_tile_collisions(ent->rect, lvl);
     
     TILE code = T_EMPTY;
     bool has_ground = false;
     bool collide_right = false;
     bool collide_left = false;
     bool collide_up = false;
     bool collide_down = false;

     ///CHECK THE COLLISIONS///
     if (ent->speedY > 0 || ent->state.grounded) {//moving down
         for (i = range.x;i <= range.w;i++) {
             code = (TILE) lvl->plot_map[range.h][i].code;
             SDL_Rect tile_rect = {i*TILESIZE, range.h*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID || code == T_ONEWAY_UP) {
                 SDL_Point ent_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(ent_origin,ent->col_bottomA);
                 SDL_Point pointB = add_points(ent_origin,ent->col_bottomB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    if (code == T_ONEWAY_UP) {
                        if ((ent->prev_rect.y + ent->prev_rect.h + 1) > tile_rect.y) {
                            collide_down = true;
                        }
                    }
                    else {
                        collide_down = true;
                    }
                 }
             }//end code equ solid or oneway up
         }//end for
         if (collide_down) {
            ent->y = (range.h*TILESIZE) - ent->rect.h; //leave in one pixel
            ent->speedY = -(ent->jumpSpeedY); //this guy's gonna jump! Unless it's zero.
            ent->rect.y = (int) ent->y;
         }
     }//end moving down
     
     else if (ent->speedY < 0) {//moving up
         for (i = range.x;i <= range.w;i++) {
             code = (TILE) lvl->plot_map[range.y][i].code;
             SDL_Rect tile_rect = {i*TILESIZE, range.y*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID) {
                 SDL_Point ent_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(ent_origin,ent->col_topA);
                 SDL_Point pointB = add_points(ent_origin,ent->col_topB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    collide_up = true;
                 }
             }

         }//end for
         if (collide_up) {
            ent->y = (range.y+1) * TILESIZE;
            ent->speedY = -(ent->speedY); //bounce off the top.
            ent->rect.y = (int) ent->y;
         }
     }//end for speedY < 0
     if (ent->speedX < 0) {//moving left
         for (i = range.y;i <= range.h;i++) {
             code = (TILE) lvl->plot_map[i][range.x].code;
             SDL_Rect tile_rect = {range.x*TILESIZE, i*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID) {
                 SDL_Point ent_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(ent_origin,ent->col_leftA);
                 SDL_Point pointB = add_points(ent_origin,ent->col_leftB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    collide_left = true;
                 }
             }

         }//end for
         if (collide_left) {
            ent->x = (range.x+1) * TILESIZE;
            ent->speedX = -(ent->speedX);
            ent->rect.x = (int) ent->x;
         }
     }//end moving left
     
     else if (ent->speedX > 0) {//moving right
         for (i = range.y;i <= range.h;i++) {
             code = (TILE) lvl->plot_map[i][range.w].code;
             SDL_Rect tile_rect = {range.w*TILESIZE, i*TILESIZE, TILESIZE, TILESIZE};
             if (code == T_SOLID) {
                 SDL_Point ent_origin = {ent->rect.x,ent->rect.y};
                 SDL_Point pointA = add_points(ent_origin,ent->col_rightA);
                 SDL_Point pointB = add_points(ent_origin,ent->col_rightB);
                 bool a = collision_point(&pointA, &tile_rect);
                 bool b = collision_point(&pointB, &tile_rect);
                 if (a || b) {
                    collide_right = true;
                 }
             }

         }//end for
         if (collide_right) {
            ent->x = (range.w*TILESIZE) - ent->rect.w - 1;
            ent->speedX = -(ent->speedX);
            ent->rect.x = (int) ent->x;
         }
     }//end moving right
     //Cut the motion of the bouncing enemy for every bounce
     if (collide_left || collide_right || collide_up || collide_down) {
         if (!ent->state.grounded) {
             ent->jumpSpeedY = ent->jumpSpeedY / 2;
             ent->speedX = ent->speedX / 2;
         }
     }
     if (ent->jumpSpeedY < 1) {
         ent->jumpSpeedY = 0;
         ent->state.grounded = true;
         ent->state.attacking = false;
     }
     
	while (node != NULL)
	{
		
        
		ENTITY* ent2 = node->data;
		/** make sure collision is with different entity **/
		if (ent->id == ent2->id) //asking: are these are the same thing?
		{
			node = node->next;
			continue;
		}
		
		kcheck = ent2->kind;
		
		/** ok, we have a different entity, check for collision
         * BUT ONLY IF IT IS AN OBJECT WE CARE IF WE COLLIDED WITH!
         * Do this for all the other collision routines as well.
         * Not everything needs to collide with everything! */
         //TODO: set so that only collide with entities we care about
         //TODO: Implement tile-based lookup for primary collision
         // routines that deal with slopes, platforms, etc.
        if (kcheck == PLAYER1) {
            if (collision_rect(&ent->rect,&ent2->rect, &rerect, true)) 
            {
                if (!ent->state.grounded && ent->state.attacking) {
                    ent->alive = false;
                }
                else if (ent->state.grounded && !ent->state.attacking) {
                    ent->speedX = ent2->speedX * 2;
                    ent->jumpSpeedY = fabsf(ent2->speedX * 2);
                    ent->speedY = -ent->jumpSpeedY;
                    ent->state.grounded = false;
                    ent->alarm_set[0] = true;
                    ent->alarm[0] = 500;
                }
            }
        }
		
		node = node->next;
	}//while node != NULL
	
	return count;
	
}//end of function update_bouncer_collision.

/*****************************************************************/
// Utility Functions
/*****************************************************************/
SDL_Point add_points(SDL_Point a, SDL_Point b) 
{
    SDL_Point r = {a.x + b.x, a.y + b.y};
    return r;
}

bool collision_point(const SDL_Point* point, const SDL_Rect* rect)
{
    SDL_bool result = SDL_FALSE;
	
    result = SDL_PointInRect(point, rect);
    
    if (result == SDL_TRUE)
    {
        return true;
    }
    
    return false;
}

bool collision_rect(const SDL_Rect* a, const SDL_Rect* b, 
                                       SDL_Rect* rerect, bool intersect)
/* a = a rectangle to check, b = the other rectangle to check
 * rerect = if you so wish, a SDL_Rect struct to receive the resulting
 * rect overlap, otherwise leave NULL and use: intersect true for just
 * whether or not there is an overlap.*/
{
	SDL_Rect empty_rect = {0,0,0,0};
	SDL_bool result = SDL_FALSE;
	if (intersect)
	{
		result = SDL_IntersectRect(a,b,rerect);
		if (result == SDL_TRUE)
		return true; //rerect has the intersection.
		else
		{
            if (rerect) {
                (*rerect) = empty_rect;
            }
		    return false; //rerect now has an empty intersection.
		}
	}
	else
	{
		result = SDL_HasIntersection(a,b);
		if (result == SDL_TRUE)
		return true;
		else
		return false;
	}
}

SDL_Rect get_tile_collisions(SDL_Rect er, LEVEL* lvl)
{
    SDL_Rect rg = {0};
    
    rg.x = (int) er.x / TILESIZE;
    rg.y = (int) er.y / TILESIZE;
    rg.w = (int) (er.x + er.w) / TILESIZE;
    rg.h = (int) (er.y + er.h) / TILESIZE;
    if (rg.x < 0)
        rg.x = 0;
    if (rg.y < 0)
        rg.y = 0;
    if (rg.x > lvl->width - 1)
        rg.x = lvl->width - 1;
    if (rg.y > lvl->height - 1)
        rg.y = lvl->height - 1;
    if (rg.w > lvl->width - 1)
        rg.w = lvl->width - 1;
    if (rg.w < 0)
        rg.w = 0;
    if (rg.h > lvl->height - 1)
        rg.h = lvl->height - 1;
    if (rg.h < 0)
        rg.h = 0;
        
    return rg;
}

void drawEntity(ENTITY* ent, GAME* game)
{

	SDL_Rect draw_rect = {0,0,TILESIZE, TILESIZE};
	SDL_Rect** tile_rect = game->current_tileset->tile_number;
	int16_t* frame_array = ent->sprite->frame_array;
	int frame_index = ent->frame_index;
	int sz = ent->sprite->size;
	SDL_Texture* prev_render_target = NULL;
	
	int x = 0;
	int y = 0;
    
    SDL_DestroyTexture(ent->image);
    //create anew.
	ent->image = SDL_CreateTexture(game->gRenderer,
                                       SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_TARGET,
                                       sz*TILESIZE, sz*TILESIZE);
	//preserve target
	prev_render_target = SDL_GetRenderTarget(game->gRenderer);
        //set to ent
	SDL_SetRenderTarget(game->gRenderer, ent->image);
        //draw on this ents image
    SDL_SetTextureBlendMode(ent->image, SDL_BLENDMODE_BLEND);
    //preserve render color
    SDL_Color col;
    SDL_GetRenderDrawColor(game->gRenderer, &col.r,&col.g,&col.b,&col.a);
    //set render color to clear
    SDL_SetRenderDrawColor(game->gRenderer,0,0,0,0); //totally transparent

    SDL_SetTextureBlendMode(game->current_tileset->image, SDL_BLENDMODE_BLEND);
    //clean slate
    SDL_RenderClear(game->gRenderer);

	for (y = 0; y < sz; y++)
	{
		draw_rect.y = y * TILESIZE;
		for (x = 0; x < sz; x++)
		{
			draw_rect.x = x * TILESIZE;
            //printf("x + y*size + frame_index*size*size = %d\n", x + y*sz + frame_index*sz*sz);
            if (frame_array[0] >= 0) { //x + y*sz + frame_index*sz*sz
                //printf("inside: frame_array[x + y*size + frame_index*size*size] = %"PRId16"\n", frame_array[x + y*sz + frame_index*sz*sz]);
                SDL_RenderCopy(game->gRenderer, game->current_tileset->image,
                tile_rect[frame_array[0]], &draw_rect);//x + y*sz + frame_index*sz*sz
            }else {
                printf("not >= 0: frame_array[x + y*size + frame_index*size*size] = %"PRId16"\n", frame_array[x + y*sz + frame_index*sz*sz]);
            }
			//printf("  ...success!\n");
		} //end for x
	}//end for y
	
	//reset render target
	SDL_SetRenderTarget(game->gRenderer, prev_render_target);
    //restore color
    SDL_SetRenderDrawColor(game->gRenderer,col.r,col.g,col.b,col.a);
	
}

int player_get_clid_priority(KIND kind)
{
    int priority;
    if (kind >= FIRST_PLATFORM && kind <= LAST_PLATFORM) {
        priority = 0;
    }
    else if (kind >= FIRST_ENEMY && kind <= LAST_ENEMY) {
        priority = 1;
    }
    else if (kind >= FIRST_LOOT && kind <= LAST_LOOT) {
        priority = 2;
    }
    else {
        priority = 100;
    }
    return priority;
}

/*****************************************************************/
// Physics Functions
/*****************************************************************/
void xy_delta(float* dx, float* dy, float speed, double angle, bool add)
{	
	if (add)
	{
		(*dx) += cos(angle) * speed;
		(*dy) += sin(angle) * speed;
    }
    else
    {
		(*dx) = cos(angle) * speed;
		(*dy) = sin(angle) * speed;
	}
}

void apply_jump(float * spX, float * spY, float jumpSpeed, 
     double gravityDirection, uint32_t updateTick, uint32_t lastUpdate)
{
	//float secs_elapsed = 0;
	
	//if (!updateTick)
	//	updateTick = SDL_GetTicks();
		
	//secs_elapsed = (float) (updateTick - lastUpdate) / 1000.0;
	gravityDirection = gravityDirection * PI * ONE_OVER_180;
	float tempspX = (*spX);
	float tempspY = (*spY);
	(*spX) = -cos(gravityDirection) * jumpSpeed + tempspX;
	(*spY) = sin(gravityDirection) * jumpSpeed + tempspY;
}

void apply_gravity(float * spX, float * spY, float gravity, 
      double gravityDirection, uint32_t updateTick, uint32_t lastUpdate)
{
	//float secs_elapsed = 0;
	
	//if (!updateTick)
	//	updateTick = SDL_GetTicks();
	//	
	//secs_elapsed = (float) (updateTick - lastUpdate) / 1000.0;
	gravityDirection = gravityDirection * PI * ONE_OVER_180;
	
	(*spX) += cos(gravityDirection) * gravity;
	(*spY) -= sin(gravityDirection) * gravity;
}

float apply_linear_motion(float pixelsPerSecond, uint32_t updateTick, uint32_t lastUpdate) {
	
	float secs_elapsed = 0;
	
	if (!updateTick)
		updateTick = SDL_GetTicks();
		
	secs_elapsed = (float) (updateTick - lastUpdate) / 1000.0;
	return secs_elapsed * pixelsPerSecond;
	
}
/************************************************************/
// Camera, misc graphical functions
/***********************************************************/
void center_camera(GAME* game, int x, int y)
{
    /*get center of view*/
    int new_x = x - (int) game->view_rect.w / 2;
    int new_y = y - (int) game->view_rect.h / 2;
    
    if (new_x < 0)
        new_x = 0;
    if (new_y < 0)
        new_y = 0;
    if ((new_x + game->view_rect.w) > game->current_level->target_rect.w)
        new_x = game->current_level->target_rect.w - game->view_rect.w;
    if ((new_y + game->view_rect.h) > game->current_level->target_rect.h)
        new_y = game->current_level->target_rect.h - game->view_rect.h;
        
    /*apply it*/
    game->view_rect.x = new_x;
    game->view_rect.y = new_y;
}

void move_camera(GAME* game, int x, int y)
{
    bool rightward = false;
    bool downward = false;
    
    /*get center of view*/
    int view_center_x = game->view_rect.x + (int) game->view_rect.w / 2;
    int view_center_y = game->view_rect.y + (int) game->view_rect.h / 2;
    
    /*get direction to centered position, and see if closer than speed*/
    if (view_center_x < x)
        rightward = true;
    if (view_center_y < y)
        downward = true;
    
    //slower, i know, but how slow, is too slow?
    int distance_x = abs(view_center_x - x);
    int distance_y = abs(view_center_y - y);
    
    //only do this if the center-to-be is at least 3 'speed' away.
    if (distance_x > game->view_speed*3) {
        if (rightward) {
                int greatest_x = game->current_level->target_rect.w-game->view_rect.w;
            if (game->view_rect.x <= greatest_x - game->view_speed)
				game->view_rect.x += game->view_speed;
            else
                game->view_rect.x = greatest_x;
        }
        else {//if leftward
            if (game->view_rect.x >= game->view_speed)
				game->view_rect.x -= game->view_speed;
            else
                game->view_rect.x = 0;
        }
    }//end if dist x close enough
    if (distance_y > game->view_speed*3) {
        if (downward) {
            int greatest_y = game->current_level->target_rect.h-game->view_rect.h;
            if (game->view_rect.y < greatest_y - game->view_speed)
				game->view_rect.y += game->view_speed;
            else
                game->view_rect.y = greatest_y;
        }
        else {//upward
            if (game->view_rect.y >= game->view_speed)
				game->view_rect.y -= game->view_speed;
            else
                game->view_rect.y = 0;
        }
    }//end if dist y close enough
}

/*************************************************************/
//Alarm Callbacks
/*************************************************************/
void update_alarms(void* ent, void* game) {
    ENTITY* e = ent;
	GAME* g = game;
    double elapsed = g->currentTick - g->lastTick;
    int numAlarms = sizeof(e->alarm_set)/sizeof(bool);
    
    for (int i=0;i<numAlarms;i++) {
        if (e->alarm_set[i] == true) {
            if (e->alarm[i] <= 0) {
                //clear the alarm
                printf("alarm function %d called, reached zero.\n", i);
                e->alarm_set[i] = false;
                e->alarm[i] = 0;
                
                //trigger the alarm after, in case need to reset
                if (e->callback[i] != NULL) {
                    e->callback[i](i, ent, game);
                }
            } 
            else {
                //printf("alarm decreasing.\n");
                e->alarm[i] -= elapsed;
            }
        }//end if alarm set is true
    }//end for all alarms
}//end function update_alarms

void_callback player_callback(int32_t code, void* ent, void* game)
{
    ENTITY* e = ent;
    printf("alarm triggered. reseting alarm.\n");
    e->alarm_set[code] = true;
    e->alarm[code] = 1000;
    return 0;
}

void_callback player_hurting_callback(int32_t code, void* ent, void* game)
{
    ENTITY* e = ent;
    e->state.hurting = false;
    printf("player no longer hurting.\n");
    return 0;
}

void_callback player_dying_callback(int32_t code, void* ent, void* game)
{
    ENTITY* e = ent;
    GAME* g = game;
    g->power = 0;
    Mix_PlayChannel(-1,g->effect[6],0);
    printf("player fully dead.\n");
    e->alive = false;
    g->mode = OVER;
    return 0;
}

void_callback bouncer_callback(int32_t code, void* ent, void* game)
{
    ENTITY* e = ent;
    e->state.attacking = true; //on an alarm so it doesn't attack immediately
    printf("now enemy %"PRIu32" is dangerous!.\n", e->id);
    return 0;
}
