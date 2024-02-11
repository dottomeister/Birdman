
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL_image.h>

struct Entity
{
	int width;
	int height;

	// -2 => UP
	// -1 => RIGHT
	//  1 => DOWN
	//  2 => LEFT
	int facing;

	float xPos;
	float yPos;
	float velocity;

	int health;
	int maxHealth;

	bool spawned;

	int xSprite;
	int ySprite;
	int spriteWidth;
	int spriteHeight;

	SDL_Texture* sprite;
};

struct Projectile
{
	int width;
	int height;

	float xPos;
	float yPos;

	// -2 => UP
	// -1 => RIGHT
	//  1 => DOWN
	//  2 => LEFT
	float facing;
	float velocity;

	int xSprite;
	int ySprite;
	int spriteWidth;
	int spriteHeight;

	SDL_Texture* sprite;
};

Entity loadPlayer();
Entity loadMinion();
Projectile loadArrow();

void spawn(Entity*, float, float);
void move(Entity*, float, float);
void move(Projectile*, float, float);

#endif
