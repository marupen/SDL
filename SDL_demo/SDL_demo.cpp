#include "stdafx.h"
#include "draw.h"

using namespace std;

bool init();
void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;
SDL_Surface* loadedSurface = NULL;

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("AOKG Lab 3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			SDL_RaiseWindow(gWindow);
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}
	return success;
}

int gcd(int a, int b) {
	if (b == 0)
		return a;
	else
		return gcd(b, a % b);
}

void cycles(float a, float b, int * num_cycles) {
	int nod;
	if (a - (int)a != 0 || b - (int)b != 0) {
		printf("Enter number of cycles:");
		scanf_s("%d", num_cycles);
	}
	else {
		nod = gcd((int)a, (int)b);
		*num_cycles = (int)a / nod;
		printf("Number of cycles = %d\n", *num_cycles);
	}
}

void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();
}

void animation(SDL_Renderer *renderer, double angle_master, double angle_slave, int size_x, int size_y, int master_x1, int master_y1, int slave_x1, int slave_y1) {
	//double zero_angle_master = 90 * 180 / M_PI, zero_angle_slave = 90 * 180 / M_PI;
	double zero_angle_master = 90, zero_angle_slave = 90;
	int zero_master_x1 = 20, zero_master_y1 = 20, zero_slave_x1 = 800, zero_slave_y1 = 800;
	while (zero_angle_master != angle_master || zero_angle_slave != angle_slave || zero_master_x1 != master_x1 || zero_master_y1 != master_y1 || zero_slave_x1 != slave_x1 || zero_slave_y1 != slave_y1) {
		if (zero_angle_master != angle_master) {
			zero_angle_master -= 0.25;
		}
		if (zero_angle_slave != angle_slave) {
			zero_angle_slave -= 0.25;
		}
		if (zero_master_x1 != master_x1) {
			zero_master_x1 += 1;
		}
		if (zero_master_y1 != master_y1) {
			zero_master_y1 += 1;
		}
		if (zero_slave_x1 != slave_x1) {
			zero_slave_x1 -= 1;
		}
		if (zero_slave_y1 != slave_y1) {
			zero_slave_y1 -= 1;
		}

		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);
		DrawScene(gRenderer, zero_angle_master, zero_angle_slave, size_x, size_y, zero_master_x1, zero_master_y1, zero_slave_x1, zero_slave_y1);
		SDL_RenderPresent(gRenderer);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	int fl_draw = 0, fl_rect = 0;
	int x0, y0;
	int size_x, size_y;
	int master_x1 = 150, master_y1 = 150, slave_x1 = 200, slave_y1 = 100;

	double angle = 0, angle_master = 0, angle_slave = 0;

	printf("Enter sizes of clipping window: ");
	rewind(stdin);
	scanf_s("%d%d", &size_x, &size_y);


	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		{
			bool quit = false;
			SDL_Event e;

			animation(gRenderer, angle_master, angle_slave, size_x, size_y, master_x1, master_y1, slave_x1, slave_y1);

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if ( SDL_QUIT == e.type )
					{
						quit = true;
					}
					switch (e.button.type)
					{
					case SDL_MOUSEBUTTONDOWN:
						fl_draw = 1;
						break;
					case SDL_MOUSEBUTTONUP:
						fl_draw = 0;
						break;
					default:
						break;
					}
					if ( SDL_KEYDOWN == e.type )
					{
						switch (e.key.keysym.scancode)
						{
						case SDL_SCANCODE_1:
							fl_rect = 0;
							break;
						case SDL_SCANCODE_2:
							fl_rect = 1;
							break;
						case SDL_SCANCODE_3:
							angle += 5;
							break;
						case SDL_SCANCODE_4:
							angle -= 5;
							break; 
						
						default:
							break;
						}
					}
					
				}
				
				if (!fl_rect) {
					angle_master += angle;
					angle = 0;
					if (fl_draw) {
						SDL_GetMouseState(&master_x1, &master_y1);
					}
				}
				if (fl_rect) {
					angle_slave += angle;
					angle = 0;
					if (fl_draw) {
						SDL_GetMouseState(&slave_x1, &slave_y1);
					}
				}



				/*if (fl_rect == 0)
					printf("\nRectangle number 1");
				if (fl_rect == 1)
					printf("\nRectangle number 2");
				if (fl_draw == 1)
					printf("\nMoving");
				SDL_GetMouseState(&x0, &y0);
				printf("\nCoordinates of mouse: %d   %d", x0, y0);*/

				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
				SDL_RenderClear(gRenderer);
				DrawScene(gRenderer, angle_master, angle_slave, size_x, size_y, master_x1, master_y1, slave_x1, slave_y1);
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}