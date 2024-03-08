#ifndef FONTX2_H
#define FONTX2_H
#include <stdbool.h>

typedef struct {
	char name[8];
	uint8_t width;
	uint8_t height;
	uint8_t *image;
} fontx2_font;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
} fontx2_bounding_box;

int fontx2_import_mem(fontx2_font *font, const uint8_t *mem, size_t size);
void fontx2_import_file(fontx2_font *font, char *filename);
void fontx2_render_char(SDL_Renderer *renderer, char character,
						int origx, int origy, fontx2_font *font, bool scale);
fontx2_bounding_box fontx2_render_text(SDL_Renderer *renderer, char *text, int x, int y,
						fontx2_font *font, SDL_Color color, bool scale);
#endif /* FONTX2_H */
