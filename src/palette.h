#ifndef PALETTE_H
#define PALETTE_H

void loadPalette(const char * fname, SDL_Color* colarray, int max_size);
void cyclePaletteRange(SDL_Surface* surf, SDL_Color* colors, int start, int end, int rep);

#endif
