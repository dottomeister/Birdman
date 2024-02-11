
#include "Entity.h"

Entity loadPlayer()
{
	struct Entity player = {};

	player.width = 60;
	player.height = 75;

	player.velocity = 0.25f;

	player.health = 3;
	player.maxHealth = 3;

	player.xSprite = 7;
	player.ySprite = 6;
	player.spriteWidth = 18;
	player.spriteHeight = 21;

	player.spawned = true;

	return player;
}

Entity loadMinion()
{
	struct Entity minion = {};

	minion.width = 45;
	minion.height = 45;

	minion.velocity = 0.2f * 0.25f;

	minion.xSprite = 65;
	minion.ySprite = 65;
	minion.spriteWidth = 60;
	minion.spriteHeight = 65;

	minion.spawned = false;

	return minion;
}

Projectile loadArrow()
{
	struct Projectile arrow = {};

	arrow.width = 40;
	arrow.height = 25;
	arrow.velocity = 0.75f;

	arrow.xSprite = 200;
	arrow.ySprite = 300;
	arrow.spriteWidth = 575;
	arrow.spriteHeight = 300;

	return arrow;
}

void spawn(Entity* entity, float x, float y)
{
	entity->facing = 1;

	entity->xPos = x;
	entity->yPos = y;
}

void move(Entity* entity, float x, float y)
{
	entity->xPos += x * entity->velocity;
	entity->yPos += y * entity->velocity;
}

void move(Projectile* projectile, float x, float y)
{
	projectile->xPos += x * projectile->velocity;
	projectile->yPos += y * projectile->velocity;
}
