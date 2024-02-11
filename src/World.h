
#ifndef WORLD_H
#define WORLD_H

#define NUM_TILES_X 16
#define NUM_TILES_Y 12

#define GRASS_TILE_WIDTH (640 / 8)
#define GRASS_TILE_HEIGHT (256 / 8 * (5 / 2))

struct Tile
{
	// 0 => Grass (Center)
	// 1 => Grass (Edge)
	// 2 => Tree
	int type;
	bool obstacle;

	SDL_Texture* sprite;
};

struct World
{
	int enemies;
	int maxEnemies;

	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
	Entity enemy[32];
};

#endif
