#include <SDL2/SDL.h>
#include <stdbool.h>

void cyclePaletteRange(SDL_Surface* surf, SDL_Color* colors, int start, int end, int rep)
{
    int ncolors = 0;
    bool forward = (end-start > 0);
    bool empty = (end-start == 0);
    
    if (!empty) {
        if (forward) {
            ncolors = end-start + 1;
            for (int j=rep;j>0;j--) {
                SDL_Color last = colors[end];
                for (int i=end;i>start;i--) {
                    colors[i] = colors[i - 1];
                }
            colors[start] = last;
            }
            SDL_SetPaletteColors(surf->format->palette, &colors[start], start, ncolors);
        }//end if forward
        if (!forward) {
            ncolors = start-end + 1;
            for (int j=rep;j>0;j--) {
                SDL_Color last = colors[end];
                for (int i=end;i<start;i++) {
                    colors[i] = colors[i + 1];
                }
            colors[start] = last;
            }
            SDL_SetPaletteColors(surf->format->palette, &colors[end], end, ncolors);
        }//end if not forward
    }//end if not empty
}

void loadPalette(const char * fname, SDL_Color* colarray, int max_size)
{
    int pal_size = 0;
    int end_size = max_size;
    char filestr[13] = {0};
    FILE* fptr = fopen(fname, "r");
    fgets(filestr, 13, fptr);
    fgets(filestr, 13, fptr);
    fgets(filestr, 13, fptr);
    sscanf(filestr, "%d", &pal_size);
    if (pal_size < max_size) {
        end_size = pal_size;
    }
    for (int i = 0; i < end_size;i++) {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        fgets(filestr, 13, fptr);
        sscanf(filestr, "%"SCNu8" %"SCNu8" %"SCNu8"", &r, &g, &b);
        SDL_Color temp = {r, g, b, 255};
        colarray[i] = temp;
    }
    
    fclose(fptr);
}
