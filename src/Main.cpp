
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 768

#define LEN(arr) ((int) (sizeof(arr) / sizeof(arr)[0]))
#define COMP(n, m) (m > n) ? 1.0f : ((m < n) ? -1.0f : 0.0f) 

SDL_Texture* GRASS_SPRITE;
SDL_Texture* TREE_SPRITE;
SDL_Texture* ARROW_SPRITE;
SDL_Texture* HEART_FULL_SPRITE;
SDL_Texture* HEART_EMPTY_SPRITE;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// SDL_Texture* loadSprite(SDL_Renderer* renderer, std::string path);

#include "Entity.h"
#include "World.h"

struct Entity player = { };
struct World world = { };

SDL_Texture* loadSprite(SDL_Renderer* renderer, std::string path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return texture;
	}

	texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

	if (texture == NULL)
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(loadedSurface);
	return texture;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! Error code: %c", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow("Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Could not open SDL window");
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL)
	{
		printf("Could not init renderer");
		return -1;
	}

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		printf("Could not init IMG");
		return -1;
	}

	GRASS_SPRITE = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\Tilemap_Grass.png");
	TREE_SPRITE = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\Tilemap_Tree.png");
	HEART_FULL_SPRITE = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\heart_full.png");
	HEART_EMPTY_SPRITE = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\heart_empty.png");

	if ((GRASS_SPRITE == NULL) || (TREE_SPRITE == NULL) || (HEART_EMPTY_SPRITE == NULL) || (HEART_FULL_SPRITE == NULL))
	{
		printf("Could not load sprites");
		return -1;
	}

	int world_tile_rows = LEN(world.tiles);
	int world_tile_cols = LEN(world.tiles[0]);

	world.enemies = 0;
	world.maxEnemies = 8;

	for (int i = 0; i < world_tile_rows; i++)
	{
		for (int j = 0; j < world_tile_cols; j++)
		{
			if ((i == 0) || (j == 0) || (i >= world_tile_rows - 1) || (j >= world_tile_cols - 1))
				world.tiles[i][j] = { 1, false, GRASS_SPRITE };
			else if ((rand() % 10) == (rand() % 20))
				world.tiles[i][j] = { 2, true, TREE_SPRITE };
		}
	}

	player = loadPlayer();
	spawn(&player, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	player.sprite = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\Tilemap_Bird.png");

	bool hasFired = false;
	int activeArrows = 0;
	struct Projectile arrows[8];
	ARROW_SPRITE = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\Tilemap_Arrow.png");

	if ((ARROW_SPRITE == NULL) || (player.sprite == NULL))
	{
		printf("Failed to load texture image!\n");
		return -1;
	}

	SDL_Event e;
	bool quit = false;

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}
		
		SDL_RenderClear(renderer);

		for (int i = 0; i < world_tile_rows; i++)
		{
			for (int j = 0; j < world_tile_cols; j++)
			{
				float angle = 0.0f;

				SDL_Rect src = { 0, 0, GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT };
				SDL_Rect dst = { (i * GRASS_TILE_WIDTH), (j * GRASS_TILE_HEIGHT), GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT };

				SDL_Point center = { GRASS_TILE_WIDTH / 2, GRASS_TILE_HEIGHT / 2 };

				if ((i == 0) && (j == 0))
					SDL_RenderCopyEx(renderer, GRASS_SPRITE, &src, &dst, angle, &center, SDL_FLIP_NONE);

				else if ((i == 0) && (j == world_tile_cols - 1))
					SDL_RenderCopyEx(renderer, GRASS_SPRITE, &src, &dst, angle, &center, SDL_FLIP_VERTICAL);

				else if ((i == world_tile_rows - 1) && (j == 0))
					SDL_RenderCopyEx(renderer, GRASS_SPRITE, &src, &dst, angle, &center, SDL_FLIP_HORIZONTAL);

				else if ((i == world_tile_rows - 1) && (j == world_tile_cols - 1))
					SDL_RenderCopyEx(renderer, GRASS_SPRITE, &src, &dst, angle, &center, (SDL_RendererFlip) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));

				else if (world.tiles[i][j].type == 1)
				{
					src = { 0, GRASS_TILE_HEIGHT, GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT };

					if ((j == 0) || (j == world_tile_cols - 1))
						src = { 10, 0, GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT };
					
					if ((i == world_tile_rows - 1) || (j == world_tile_cols - 1))
						angle = 180.0f;

					SDL_RenderCopyEx(renderer, GRASS_SPRITE, &src, &dst, angle, &center, SDL_FLIP_NONE);
				}
				else
				{
					SDL_Rect src = { GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT, GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT };
					SDL_RenderCopy(renderer, GRASS_SPRITE, &src, &dst);
				}
			}
		}

		SDL_Rect playerSrc = { player.xSprite, player.ySprite, player.spriteWidth, player.spriteHeight };
		SDL_Rect playerDst = { player.xPos, player.yPos, player.width, player.height };

		SDL_RenderCopy(renderer, player.sprite, &playerSrc, &playerDst);

#if 0

		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

		SDL_Rect fillRect = { player.xPos, player.yPos, player.width, 1 };
		SDL_RenderFillRect(renderer, &fillRect);

		fillRect = { (int) player.xPos, (int)player.yPos + player.height, player.width, 1 };
		SDL_RenderFillRect(renderer, &fillRect);

		fillRect = { (int) player.xPos, (int)player.yPos, 1, player.height };
		SDL_RenderFillRect(renderer, &fillRect);

		fillRect = { (int) player.xPos + player.width, (int)player.yPos, 1, player.height };
		SDL_RenderFillRect(renderer, &fillRect);

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

#endif

		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

		if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
		{
			// MOVE CHARACTER UP
			player.facing = -2;
			player.xSprite = 35 + (2 * player.spriteWidth);

			if (player.yPos > 0)
				move(&player, 0.0f, -1.0f);
		}

		if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
		{
			// MOVE CHARACTER RIGHT
			player.facing = -1;
			player.xSprite = 20 + player.spriteWidth;

			if (player.xPos < ((world_tile_rows - 1) * GRASS_TILE_WIDTH))
				move(&player, 1.0f, 0.0f);
		}

		if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
		{
			// MOVE CHARACTER DOWN
			player.facing = 1;
			player.xSprite = 7;

			if (player.yPos < ((world_tile_cols - 1) * GRASS_TILE_HEIGHT - 20))
				move(&player, 0.0f, 1.0f);
		}

		if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
		{
			// MOVE CHARACTER LEFT
			player.facing = 2;
			player.xSprite = 50 + (3 * player.spriteWidth);

			if (player.xPos > 0)
				move(&player, -1.0f, 0.0f);
		}

		if (currentKeyStates[SDL_SCANCODE_SPACE])
		{
			if (!hasFired)
			{
				if (activeArrows < LEN(arrows))
				{
					arrows[activeArrows] = loadArrow();

					arrows[activeArrows].facing = player.facing;

					arrows[activeArrows].xPos = player.xPos;
					arrows[activeArrows].yPos = player.yPos;

					activeArrows++;
				}

				hasFired = true;
			}
		}
		else
			hasFired = false;

		for (int i = 0; i < LEN(arrows); i++)
		{
			if (i >= activeArrows)
				break;

			if ((arrows[i].yPos > 0) &&
				(arrows[i].yPos < ((world_tile_cols - 1) * GRASS_TILE_HEIGHT - 20)) &&
				(arrows[i].xPos < ((world_tile_rows - 1) * GRASS_TILE_WIDTH)) &&
				(arrows[i].xPos > 0))
			{
				// MOVE ARROW
				int xFacing = 0;
				int yFacing = 0;
				int angle = 0;
				SDL_RendererFlip flip = SDL_FLIP_NONE;

				if (arrows[i].facing == -1)
					xFacing = 1;
				else if (arrows[i].facing == 2)
				{
					xFacing = -1;
					flip = SDL_FLIP_HORIZONTAL;
				}
				else if (arrows[i].facing == -2)
				{
					angle = 270.0f;
					yFacing = -1;
				}
				else if (arrows[i].facing == 1)
				{
					angle = 90.0f;
					yFacing = 1;
				}

				move(&arrows[i], xFacing, yFacing);

				for (int i = 0; i < world.maxEnemies; i++)
				{
					if (world.enemy[i].spawned)
					{
						if (((arrows[i].xPos >= world.enemy[i].xPos) &&
							(arrows[i].xPos <= (world.enemy[i].xPos + world.enemy[i].width)) ||
							(((arrows[i].xPos + arrows[i].width) >= world.enemy[i].xPos) &&
								((arrows[i].xPos + arrows[i].width) <= (world.enemy[i].xPos + world.enemy[i].width)))))
						{
							if (((arrows[i].yPos >= world.enemy[i].yPos) &&
								(arrows[i].yPos <= (world.enemy[i].yPos + world.enemy[i].height)) ||
								(((arrows[i].yPos + arrows[i].height) >= world.enemy[i].yPos) &&
									((arrows[i].yPos + arrows[i].height) <= (world.enemy[i].yPos + world.enemy[i].height)))))
							{

								world.enemy[i].spawned = false;
								world.enemies--;
								activeArrows--;
								continue;
							}
						}
							

						/*
						if ((arrows[i].xPos + arrows[i].width >= world.enemy[i].xPos) &&
							(arrows[i].xPos <= world.enemy[i].xPos + world.enemy[i].width) &&
							(arrows[i].yPos + arrows[i].height >= world.enemy[i].yPos) &&
							(arrows[i].yPos <= world.enemy[i].xPos + world.enemy[i].height))
						{
							world.enemy[i].spawned = false;
							world.enemies--;
							activeArrows--;
							continue;
						}
						*/
					}
				}

				// DRAW ARROW.
				SDL_Point center = { 0, 0 };
				SDL_Rect arrowSrc = { arrows[i].xSprite, arrows[i].ySprite, arrows[i].spriteWidth, arrows[i].spriteHeight };
				SDL_Rect arrowDst = { arrows[i].xPos, arrows[i].yPos, arrows[i].width, arrows[i].height };

				SDL_RenderCopyEx(renderer, ARROW_SPRITE, &arrowSrc, &arrowDst, angle, &center, flip);
			}
			else
				activeArrows--;
		}

		for (int i = 0; i < world.maxEnemies; i++)
		{
			if (!world.enemy[i].spawned && (world.enemies < world.maxEnemies))
			{
				world.enemy[i] = loadMinion();
				
				world.enemy[i].sprite = loadSprite(renderer, "C:\\Users\\fruct\\Documents\\VRM\\Project\\data\\Tilemap_Minion.png");
				world.enemy[i].spawned = true;
				world.enemies++;

				spawn(&world.enemy[i],
					rand() % ((world_tile_rows - 1) * GRASS_TILE_WIDTH),
					rand() % 2 == 1 ? 0 : (world_tile_cols - 1) * GRASS_TILE_HEIGHT - 20);
			}

			if (world.enemy[i].spawned)
			{
				move(&world.enemy[i], COMP(world.enemy[i].xPos, player.xPos), COMP(world.enemy[i].yPos, player.yPos));

				if (((world.enemy[i].xPos + world.enemy[i].width) >= player.xPos) &&
					(world.enemy[i].xPos <= (player.xPos + player.width)) &&
					((world.enemy[i].yPos + world.enemy[i].height) >= player.yPos) &&
					(world.enemy[i].yPos <= (player.yPos + player.height)))
				{
					world.enemy[i].spawned = false;

					world.enemies--;
					player.health--;
					continue;
				}

				SDL_Rect enemySrc = { world.enemy[i].xSprite, world.enemy[i].ySprite, world.enemy[i].spriteWidth, world.enemy[i].spriteHeight };
				SDL_Rect enemyDst = { world.enemy[i].xPos, world.enemy[i].yPos, world.enemy[i].width, world.enemy[i].height };

#if 0

				SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

				SDL_Rect fillRect = { world.enemy[i].xPos, world.enemy[i].yPos, world.enemy[i].width, 1 };
				SDL_RenderFillRect(renderer, &fillRect);

				fillRect = { (int)world.enemy[i].xPos, (int)world.enemy[i].yPos + world.enemy[i].height, world.enemy[i].width, 1 };
				SDL_RenderFillRect(renderer, &fillRect);

				fillRect = { (int)world.enemy[i].xPos, (int)world.enemy[i].yPos, 1, world.enemy[i].height };
				SDL_RenderFillRect(renderer, &fillRect);

				fillRect = { (int)world.enemy[i].xPos + world.enemy[i].width, (int)world.enemy[i].yPos, 1, world.enemy[i].height };
				SDL_RenderFillRect(renderer, &fillRect);

				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

#endif

				SDL_RenderCopy(renderer, world.enemy[i].sprite, &enemySrc, &enemyDst);
			}
		}

#if 1

		for (int i = 0; i < world_tile_rows; i++)
		{
			for (int j = 0; j < world_tile_cols; j++)
			{
				if (world.tiles[i][j].type == 2)
				{
					int wSrc = 110;
					int hSrc = 175;
					
					int wDst = wSrc * 0.75f;
					int hDst = hSrc * 0.75f;
					int xDst = (i * GRASS_TILE_WIDTH) - (wDst / 4);
					int yDst = (j * GRASS_TILE_HEIGHT) - (hDst - GRASS_TILE_HEIGHT);

					SDL_Rect src = { 45, 0, wSrc, hSrc };
					SDL_Rect dst = { xDst, yDst, wDst, hDst};

#if 0

					SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

					SDL_Rect fillRect = { xDst, yDst, wDst, 1 };
					SDL_RenderFillRect(renderer, &fillRect);
					
					fillRect = { xDst, yDst + hDst, wDst, 1 };
					SDL_RenderFillRect(renderer, &fillRect);

					fillRect = { xDst, yDst, 1, hDst };
					SDL_RenderFillRect(renderer, &fillRect);

					fillRect = { xDst + wDst, yDst, 1, hDst };
					SDL_RenderFillRect(renderer, &fillRect);
					
					SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

#endif

					SDL_RenderCopy(renderer, TREE_SPRITE, &src, &dst);
				}
			}
		}
		
#endif

#if 1

		for (int i = 0; i < player.maxHealth; i++)
		{
			int hSize = 50;

			SDL_Rect src = { 0, 0, 17, 17 };
			SDL_Rect dst = { (i * hSize) + 10, 10, hSize, hSize };

			if (i < player.health)
				SDL_RenderCopy(renderer, HEART_FULL_SPRITE, &src, &dst);
			else
				SDL_RenderCopy(renderer, HEART_EMPTY_SPRITE, &src, &dst);
		}

#endif
	
		SDL_RenderPresent(renderer);
	}


	SDL_DestroyTexture(GRASS_SPRITE);
	SDL_DestroyTexture(TREE_SPRITE);
	SDL_DestroyTexture(ARROW_SPRITE);
	SDL_DestroyTexture(HEART_FULL_SPRITE);
	SDL_DestroyTexture(HEART_EMPTY_SPRITE);
	SDL_DestroyTexture(player.sprite);
	
	for (int i = 0; i < LEN(world.enemy); i++)
		SDL_DestroyTexture(world.enemy[i].sprite);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return 0;
}