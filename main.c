#include <stdbool.h>
#include <time.h>
#include <sys/param.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "fontx2.h"

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX bin_
#include "incbin.h"

#define PI 3.141592 // accurate enough
#define WIDTH 640
#define HEIGHT 480

INCBIN(f1nn5ter, "./assets/f1nn5ter.png");
INCBIN(icky, "./assets/icky.png");
INCBIN(tank, "./assets/tank.png");
INCBIN(logo, "./assets/logo.png");
INCBIN(controls, "./assets/controls.png");
INCBIN(background_1, "./assets/background_1.png");

INCBIN(unscii, "./assets/unscii.fnt");

INCBIN(mococo679, "./assets/mococo_abyssgard-679.mp3");
INCBIN(fireflies, "./assets/fireflies.mp3");

typedef struct {
	char *name;
	SDL_Texture *texture;
	int texture_width;
	int texture_height;
} character;

SDL_Texture *logo;
SDL_Texture *controls;
SDL_Texture *background_1;

fontx2_font unscii;

Mix_Music *music_mococo679;
Mix_Music *music_fireflies;

const SDL_Color blue = { .r = 91, .g = 206, .b = 250, .a = 255 };
const SDL_Color pink = { .r = 245, .g = 169, .b = 184, .a = 255 };
const SDL_Color white = { .r = 255, .g = 255, .b = 255, .a = 255 };

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

SDL_Texture *load_image(SDL_Renderer *renderer, const unsigned char *mem, int size)
{
	SDL_RWops *rwops;
	SDL_Texture *tex = NULL;

	rwops = SDL_RWFromConstMem(mem, size);
	tex = IMG_LoadTexture_RW(renderer, rwops, 1);
	return tex;
}

Mix_Music *load_music(const unsigned char *mem, int size)
{
	SDL_RWops *rwops;
	Mix_Music *music = NULL;

	rwops = SDL_RWFromConstMem(mem, size);
	music = Mix_LoadMUS_RW(rwops, 1);
	return music;
}

character *load_characters(SDL_Renderer *renderer, size_t *characters_size)
{
	size_t size = 2;
	unsigned int i = 0;
	
	character *characters = malloc(sizeof(character) * size);

	characters[i].name = "F1NN5TER";
	characters[i].texture = load_image(renderer, bin_f1nn5ter_data, bin_f1nn5ter_size);
	i++;
	
	characters[i].name = "Icky";
	characters[i].texture = load_image(renderer, bin_icky_data, bin_icky_size);
	i++;

	*characters_size = i;

	for (i = 0; i < *characters_size; i++) {
		SDL_QueryTexture(characters[i].texture, NULL, NULL,
			&characters[i].texture_width, &characters[i].texture_height);
	}
	
	return characters;
}

bool in_bounding_box(int x, int y, fontx2_bounding_box *box)
{
	if (x >= box->x1 && x <= box->x2 && y >= box->y1 && y <= box->y2) {
		return true;
	} else {
		return false;
	}
}

int main_menu(SDL_Renderer *renderer, unsigned int *players)
{
	bool quit = false;
	bool show_controls = false;
	bool show_credits = false;
	
	fontx2_bounding_box text_start_1p = { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0 };
	fontx2_bounding_box text_start_2p = { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0 };
	fontx2_bounding_box text_controls = { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0 };
	fontx2_bounding_box text_credits  = { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0 };
	fontx2_bounding_box text_quit     = { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0 };

	char credits_str[] = "Programming & Graphics:\n\tBIG KRIMPIN\n"
		"Music:\n"
		"\tMococo Abyssgard - 679 [AI Cover]\n"
		"\tOwl City - Fireflies [8-Bit Cover]\n"
		"Fonts:\n"
		"\tUnscii by Viznut\n"
		"\tPress Start 2P by CodeMan38\n"
		"\tGrape Soda by jeti\n"
		"Libraries:\n"
		"\tSDL2 by the SDL Community\n"
		"\tIncbin by Dale Weiler";
	
	bool mouse_button = false;
	int mouse_x, mouse_y;
	
	SDL_Rect overlay = { .x = 0, .y = 0, .w = WIDTH, .h = HEIGHT };

	while (!quit) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouse_button = true;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					if (show_controls || show_credits) {
						show_controls = false;
						show_credits = false;
					}
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							if (show_controls || show_credits) {
								show_controls = false;
								show_credits = false;
							} else {
								quit = true;
							}
							break;
					}
					if (show_controls) {
						show_controls = false;
					}
					break;
			}
		}

		if (mouse_button) {
			if (in_bounding_box(mouse_x, mouse_y, &text_start_1p)) {
				*players = 1;
				break;
			} else if (in_bounding_box(mouse_x, mouse_y, &text_start_2p)) {
				*players = 2;
				break;
			} else if (in_bounding_box(mouse_x, mouse_y, &text_controls)) {
				show_controls = true;
			} else if (in_bounding_box(mouse_x, mouse_y, &text_credits)) {
				show_credits = true;
			} else if (in_bounding_box(mouse_x, mouse_y, &text_quit)) {
				quit = true;
			}
		}

		mouse_button = false;

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, background_1, NULL, NULL);
		SDL_RenderCopy(renderer, logo, NULL, NULL);
		text_start_1p = fontx2_render_text(renderer, "START (1P)",
			20, 300, &unscii, blue, true);
		text_start_2p = fontx2_render_text(renderer, "START (2P)",
			text_start_1p.x1, text_start_1p.y2, &unscii, pink, true);
		text_controls = fontx2_render_text(renderer, "CONTROLS",
			text_start_2p.x1, text_start_2p.y2, &unscii, white, true);
		text_credits = fontx2_render_text(renderer, "CREDITS",
			text_controls.x1, text_controls.y2, &unscii, pink, true);
		text_quit = fontx2_render_text(renderer, "QUIT",
			text_credits.x1, text_credits.y2, &unscii, blue, true);
			
		if (show_controls) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 204); // (255 / 100% * 80% = 204)
			SDL_RenderFillRect(renderer, &overlay);
			SDL_RenderCopy(renderer, controls, NULL, NULL);
		}
		
		if (show_credits) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 204); // (255 / 100% * 80% = 204)
			SDL_RenderFillRect(renderer, &overlay);
			fontx2_render_text(renderer, credits_str,
				20, 10, &unscii, white, true);
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_RenderPresent(renderer);
	}
	
	if (quit) {
		return 1;
	} else {
		return 0;
	}
}

int character_selection(SDL_Renderer *renderer, unsigned int players,
	character *characters, size_t characters_size,
	unsigned int *p1, unsigned int *p2)
{
	bool quit = false;
	bool done = false;

	bool player1_up = false;
	bool player1_down = false;
	bool player2_up = false;
	bool player2_down = false;

	unsigned int p1_character_index = 0;
	unsigned int p2_character_index = 0;

	char character_select_text[] = "SELECT A CHARACTER";

	SDL_Rect p1_character_rect;
	SDL_Rect p2_character_rect;

	SDL_Rect overlay = { .x = 0, .y = 0, .w = WIDTH, .h = HEIGHT };
	
	while (!quit && !done) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							quit = true;
							break;
						case SDL_SCANCODE_RETURN:
							done = true;
							break;
						case SDL_SCANCODE_W:
							player1_up = true;
							break;
						case SDL_SCANCODE_S:
							player1_down = true;
							break;
						case SDL_SCANCODE_UP:
							player2_up = true;
							break;
						case SDL_SCANCODE_DOWN:
							player2_down = true;
							break;
					}
					break;
			}
		}

		if (player1_up) {
			p1_character_index = mod(p1_character_index - 1, characters_size);
		} else if (player1_down) {
			p1_character_index = mod(p1_character_index + 1, characters_size);
		}
		
		if (player2_up) {
			p2_character_index = mod(p2_character_index - 1, characters_size);
		} else if (player2_down) {
			p2_character_index = mod(p2_character_index + 1, characters_size);
		}

		player1_up = false;
		player1_down = false;
		player2_up = false;
		player2_down = false;

		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, background_1, NULL, NULL);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 204); // (255 / 100% * 80% = 204)
		SDL_RenderFillRect(renderer, &overlay);
		
		fontx2_render_text(renderer, character_select_text,
				(WIDTH / 2) - (strlen(character_select_text) * unscii.width * 2 / 2), 30, &unscii, white, true);

		if (players == 1) { /* Single-player character selection */
			p1_character_rect = (SDL_Rect) {
				.x = (WIDTH / 2) - ((250.0 / (float)characters[p1_character_index].texture_height * characters[p1_character_index].texture_width) / 2),
				.y = (HEIGHT / 2) - (250 / 2) + 30,
				.w = 250.0 / (float)characters[p1_character_index].texture_height * characters[p1_character_index].texture_width,
				.h = 250
			};
			SDL_RenderCopy(renderer, characters[p1_character_index].texture,
				NULL, &p1_character_rect);
			fontx2_render_text(renderer, characters[p1_character_index].name,
				(WIDTH / 2) - (strlen(characters[p1_character_index].name) * unscii.width * 2 / 2), 100, &unscii, white, true);
		} else { /* Multi-player character selection */
			p1_character_rect = (SDL_Rect) {
				.x = (WIDTH / 2) - ((250.0 / (float)characters[p1_character_index].texture_height * characters[p1_character_index].texture_width) / 2) - 100,
				.y = (HEIGHT / 2) - (250 / 2) + 30,
				.w = 250.0 / (float)characters[p1_character_index].texture_height * characters[p1_character_index].texture_width,
				.h = 250
			};
			p2_character_rect = (SDL_Rect) {
				.x = (WIDTH / 2) - ((250.0 / (float)characters[p2_character_index].texture_height * characters[p2_character_index].texture_width) / 2) + 100,
				.y = (HEIGHT / 2) - (250 / 2) + 30,
				.w = 250.0 / (float)characters[p2_character_index].texture_height * characters[p2_character_index].texture_width,
				.h = 250
			};
			
			SDL_RenderCopy(renderer, characters[p1_character_index].texture,
				NULL, &p1_character_rect);
			SDL_RenderCopy(renderer, characters[p2_character_index].texture,
				NULL, &p2_character_rect);
				
			fontx2_render_text(renderer, characters[p1_character_index].name,
				(WIDTH / 2) - (strlen(characters[p1_character_index].name) * unscii.width * 2 / 2) - 100, 100, &unscii, white, true);
			fontx2_render_text(renderer, characters[p2_character_index].name,
				(WIDTH / 2) - (strlen(characters[p2_character_index].name) * unscii.width * 2 / 2) + 100, 100, &unscii, white, true);
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderPresent(renderer);
	}

	*p1 = p1_character_index;
	*p2 = p2_character_index;
	
	if (quit) {
		return 1;
	} else {
		return 0;
	}
}

void pong_game(SDL_Renderer *renderer, character player1, character player2, unsigned int players)
{
	bool quit = false;

	SDL_Texture *paddle1_texture = player1.texture;
	SDL_Texture *paddle2_texture = player2.texture;
	SDL_Texture *ball_tank = load_image(renderer, bin_tank_data, bin_tank_size);

	SDL_Rect overlay = { .x = 0, .y = 0, .w = WIDTH, .h = HEIGHT };
	SDL_Rect paddle1 = {
		.x = 0,
		.y = 0,
		.w = (180.0 / (float)player1.texture_height * player1.texture_width),
		.h = 180 };

	SDL_Rect paddle2 = {
		.x = WIDTH - (180.0 / (float)player2.texture_height * player2.texture_width),
		.y = 0,
		.w = (180.0 / (float)player2.texture_height * player2.texture_width),
		.h = 180 };

	SDL_Rect ball = {
		.x = (WIDTH / 2) - (64 / 2),
		.y = (HEIGHT / 2) - (64 / 2),
		.w = 64,
		.h = 64 };

	paddle1.y = (HEIGHT / 2) - (paddle1.h / 2);
	paddle2.y = (HEIGHT / 2) - (paddle2.h / 2);
	int ball_angle;

	if (rand() % 2) {
		ball_angle = mod((rand() % 90) - 45, 360);
	} else {
		ball_angle = mod((rand() % 90) + 135, 360);
	}
	
	int speed = 5;

	const int fps = 60;
	const int ms_frame = 1000 / fps;
	int initial_ticks, elapsed_ms;

	// start with 3 second timer
	unsigned int start_timer = fps * 3;
	char start_timer_str[12];
	sprintf(start_timer_str, "%d", start_timer / fps);

	bool paddle1_up = false;
	bool paddle1_down = false;
	bool paddle2_up = false;
	bool paddle2_down = false;

	int player1_score = 0;
	int player2_score = 0;
	char player1_score_str[12];
	char player2_score_str[12];
	sprintf(player1_score_str, "%02d", player1_score);
	sprintf(player2_score_str, "%02d", player2_score);

	unsigned int paddle_speed = 5;

	unsigned int bot_timer = 0;
	int bot_y_pos = ball.y;

	Mix_PlayMusic(music_mococo679, -1);
	
	while (!quit) {
		SDL_Event e;
		initial_ticks = SDL_GetTicks();

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							quit = true;
							break;
						case SDL_SCANCODE_W:
							paddle1_up = true;
							break;
						case SDL_SCANCODE_S:
							paddle1_down = true;
							break;
						case SDL_SCANCODE_UP:
							paddle2_up = true;
							break;
						case SDL_SCANCODE_DOWN:
							paddle2_down = true;
							break;
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.scancode) {
						case SDL_SCANCODE_W:
							paddle1_up = false;
							break;
						case SDL_SCANCODE_S:
							paddle1_down = false;
							break;
						case SDL_SCANCODE_UP:
							paddle2_up = false;
							break;
						case SDL_SCANCODE_DOWN:
							paddle2_down = false;
							break;
					}
			}
		}

		/* Paddle movement */
		if (paddle1_up && paddle1.y > 0) {
			paddle1.y -= paddle_speed;
		}
		if (paddle1_down && paddle1.y + paddle1.h < HEIGHT) {
			paddle1.y += paddle_speed;
		}

		if (players == 1) {
			/* Bot */
			if (paddle2.y + paddle2.h < HEIGHT && paddle2.y + (paddle2.h / 2) < bot_y_pos - paddle_speed) {
				paddle2.y += paddle_speed;
			} else if (paddle2.y > 0 && paddle2.y + (paddle2.h / 2) > bot_y_pos + paddle_speed){
				paddle2.y -= paddle_speed;
			}

			if (bot_timer == 0) {
				bot_y_pos = ball.y + (ball.h / 2);
				if (bot_y_pos <= 0) {
					bot_y_pos = 1;
				}
			}
			bot_timer = (bot_timer + 1) % fps;
		} else {
			if (paddle2_up && paddle2.y > 0) {
				paddle2.y -= paddle_speed;
			}
			if (paddle2_down && paddle2.y + paddle2.h < HEIGHT) {
				paddle2.y += paddle_speed;
			}
		}

		/* Ball physics */
		if (ball.y <= 0 && ball_angle >= 180) {
			if (ball_angle > 90) {
				ball_angle = (90 - ball_angle) + 90;
			} else {
				ball_angle = 90 - (ball_angle - 90);
			}
			ball_angle = (ball_angle + 180) % 360;
		}
		if (ball.y + ball.h >= HEIGHT && ball_angle < 180) {
			if (ball_angle > 270) {
				ball_angle = (270 - ball_angle) + 270;
			} else {
				ball_angle = 270 - (ball_angle - 270);
			}
			ball_angle = (ball_angle + 180) % 360;
		}

		/* Paddle collisions */
		/* Paddle 1 */
		if ( ((ball.y >= paddle1.y && ball.y <= paddle1.y + paddle1.h)
			|| (ball.y + ball.h >= paddle1.y && ball.y + ball.h <= paddle1.y + paddle1.h))
			&& ball.x < paddle1.x + paddle1.w) {
			int ball_center_y = ball.y + (ball.h / 2);
			if (ball_angle > 90 && ball_angle < 270) {
				// ball_angle = (ball_angle + 180) % 360;
				float percentage = 100.0 / paddle1.h * (ball_center_y - paddle1.y);
				if (percentage > 80) {
					percentage = 80;
				} else if (percentage < 20) {
					percentage = 20;
				}
				percentage = 100 - percentage;
				ball_angle = mod(90 - (180.0 / 100.0 * percentage), 360);
			}
		}

		/* Paddle 2 */
		if ( ((ball.y >= paddle2.y && ball.y <= paddle2.y + paddle2.h)
			|| (ball.y + ball.h >= paddle2.y && ball.y + ball.h <= paddle2.y + paddle2.h))
			&& ball.x + ball.w > paddle2.x) {
			int ball_center_y = ball.y + (ball.h / 2);
			if (ball_angle < 90 || ball_angle > 270) {
				// ball_angle = (ball_angle + 180) % 360;
				float percentage = 100.0 / paddle2.h * (ball_center_y - paddle2.y);
				if (percentage > 80) {
					percentage = 80;
				} else if (percentage < 20) {
					percentage = 20;
				}
				percentage = 100 - percentage;
				ball_angle = mod(90 + (180.0 / 100.0 * percentage), 360);
			}
		}

		/* Wall collisions */
		if (ball.x <= 0) {
			player2_score += 1;
			sprintf(player2_score_str, "%02d", player2_score);
			ball.x = (WIDTH / 2) - (ball.w / 2);
			ball.y = (HEIGHT / 2) - (ball.h / 2);
			
			ball_angle = mod((rand() % 90) - 45, 360);
			start_timer = fps * 2;
		}

		if (ball.x + ball.w >= WIDTH) {
			player1_score += 1;
			sprintf(player1_score_str, "%02d", player1_score);
			ball.x = (WIDTH / 2) - (ball.w / 2);
			ball.y = (HEIGHT / 2) - (ball.h / 2);

			ball_angle = mod((rand() % 90) + 135, 360);
			start_timer = fps * 2;
		}

		/* Move ball */
		if (start_timer != 0) {
			start_timer -= 1;
		} else {
			ball.x = ball.x + speed * cos(ball_angle * (PI / 180.0));
			ball.y = ball.y + speed * sin(ball_angle * (PI / 180.0));
		}

		SDL_RenderClear(renderer);

		/* Render background */
		SDL_RenderCopy(renderer, background_1, NULL, NULL);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127); // (255 / 100% * 50% = 127)
		SDL_RenderFillRect(renderer, &overlay);

		/* Display score*/
		fontx2_render_text(renderer, player1_score_str,
			(WIDTH / 2) - ((strlen(player1_score_str) * (unscii.width * 2)) / 2) - 40,
			10, &unscii, white, true);
		fontx2_render_text(renderer, player2_score_str,
			(WIDTH / 2) - ((strlen(player2_score_str) * (unscii.width * 2)) / 2) + 40,
			10, &unscii, white, true);

		/* Display paddles and ball */
		SDL_RenderCopy(renderer, paddle1_texture, NULL, &paddle1);
		SDL_RenderCopy(renderer, paddle2_texture, NULL, &paddle2);
		SDL_RenderCopy(renderer, ball_tank, NULL, &ball);

		if (start_timer != 0) {
			sprintf(start_timer_str, "%d", (int) ceilf((float) start_timer / (float) fps));
			fontx2_render_text(renderer, start_timer_str,
				(WIDTH / 2) - ((strlen(start_timer_str) * (unscii.width * 2)) / 2),
				(HEIGHT / 2) - ((strlen(start_timer_str) * (unscii.height * 2)) / 2) - 40,
				&unscii, white, true);
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderPresent(renderer);

		elapsed_ms = SDL_GetTicks() - initial_ticks;
		if (elapsed_ms < ms_frame) SDL_Delay(ms_frame - elapsed_ms);
	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("F1NNICKY PONG",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	logo = load_image(renderer, bin_logo_data, bin_logo_size);
	controls = load_image(renderer, bin_controls_data, bin_controls_size);
	background_1 = load_image(renderer, bin_background_1_data, bin_background_1_size);

	Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048);
	music_mococo679 = load_music(bin_mococo679_data, bin_mococo679_size);
	music_fireflies = load_music(bin_fireflies_data, bin_fireflies_size);
	Mix_PlayMusic(music_fireflies, -1);

	fontx2_import_mem(&unscii, bin_unscii_data, bin_unscii_size);

	size_t characters_size;
	character *characters = load_characters(renderer, &characters_size);

	unsigned int players = 0;
	unsigned int p1_index = 0;
	unsigned int p2_index = 0;

	if (main_menu(renderer, &players)) {
		SDL_Quit();
		return 0;
	}

	if (character_selection(renderer, players, characters, characters_size, &p1_index, &p2_index)) {
		SDL_Quit();
		return 0;
	}

	if (players == 1) {
		p2_index = rand() % characters_size;
	}
	
	pong_game(renderer, characters[p1_index], characters[p2_index], players);
	SDL_Quit();
	return 0;
}
