#include <stdbool.h>
#include <SDL2/SDL.h>
#include "fontx2.h"

static uint8_t *load_file_to_mem(char *filename, long *size)
{
	FILE *f;
	uint8_t *mem;
	
	f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	*size = ftell(f);

	mem = malloc(*size);

	fseek(f, 0, SEEK_SET);
	fread(mem, *size, 1, f);
	fclose(f);
	
	return mem;
}

int fontx2_import_mem(fontx2_font *font, const uint8_t *mem, size_t size)
{
	// memcpy(font->name, mem + 6, 8);
	font->width = mem[14];
	font->height = mem[15];

	font->image = malloc(size - 17);

	memcpy(font->image, &mem[17], size - 17);
	
	return 0;
}

void fontx2_import_file(fontx2_font *font, char *filename)
{
	long size;
	uint8_t *mem = load_file_to_mem(filename, &size);

	fontx2_import_mem(font, mem, size);
	free(mem);
}

void fontx2_render_char(SDL_Renderer *renderer, char character,
						int origx, int origy, fontx2_font *font, bool scale)
{
	uint32_t fsz = (font->width + 7) / 8 * font->height;
	for (int y = 0; y < font->height; y++) {
		for (int x = 0; x < font->width; x++) {
			if (((font->image[character * fsz + y] >> (font->width - x)) & 1) == 1) {
				if (scale) {
					SDL_RenderDrawPoint(renderer, origx + (x * 2), origy + (y * 2));
					SDL_RenderDrawPoint(renderer, origx + (x * 2) + 1, origy + (y * 2));
					SDL_RenderDrawPoint(renderer, origx + (x * 2), origy + (y * 2) + 1);
					SDL_RenderDrawPoint(renderer, origx + (x * 2) + 1, origy + (y * 2) + 1);
				} else {
					SDL_RenderDrawPoint(renderer, origx + x, origy + y);
				}
			}
		}
	}
}

fontx2_bounding_box fontx2_render_text(SDL_Renderer *renderer, char *text, int x, int y,
						fontx2_font *font, SDL_Color color, bool scale)
{
	int maxposx = 0;
	int maxposy = font->height;
	int posx = 0;
	int posy = 0;

	if (scale) {
		maxposy += font->height;
	}
	
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (; *text; text++) {
		switch (*text) {
			case '\n':
				if (scale) {
					posy += font->height * 2;
					maxposy += font->height * 2;
				} else {
					posy += font->height;
					maxposy += font->height;
				}
				posx = 0;
				continue;
			case '\t':
				if (scale) {
					posx += (font->width * 2) * 4;
				} else {
					posx += font->width * 4;
				}
				continue;
		}
		
		fontx2_render_char(renderer, *text, x + posx, y + posy, font, scale);
		
		if (scale)
			posx += font->width * 2;
		else
			posx += font->width;

		if (posx > maxposx)
			maxposx = posx;
	}

	// printf("maxposx: %d maxposy: %d\n", maxposx, maxposy);
	
	return (fontx2_bounding_box){
		.x1 = x,
		.y1 = y,
		.x2 = x + maxposx,
		.y2 = y + maxposy
	};
}
