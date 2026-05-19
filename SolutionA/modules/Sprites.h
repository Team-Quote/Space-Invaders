// Sprites.h
// Course number:
// Term: 
// Project number: 
// Feature description: A brief summary on feature responsibility and team member assignment
// Team #:
// Team members:
#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum{
  ENEMY_30 = 0,
  ENEMY_20 = 1,
  ENEMY_10 = 2
} EnemyType_t;

typedef enum{
  POSTURE_CLOSE = 0,
  POSTURE_OPEN = 1
} EnemyPosture_t;

typedef struct{
  uint8_t x;
  uint8_t y;
  const uint8_t *image;
  bool life;
} Sprite_t;

#define ENEMYW    16
#define ENEMYH    10
#define PLAYERW   18
#define PLAYERH    8
#define BULLETW    2
#define BULLETH    9
#define EXPLODEW  16
#define EXPLODEH  10

extern const uint8_t SmallEnemyPointA[3][200];
extern const uint8_t SmallEnemyPointB[3][200];
extern const uint8_t PlayerShip0[];
extern const uint8_t Laser0[];
extern const uint8_t SmallExplosion0[];

void Sprites_SetEnemyPosture(Sprite_t *sprite, EnemyType_t type, EnemyPosture_t posture);
void Sprites_InitEnemy(Sprite_t *sprite, uint8_t x, uint8_t y, EnemyType_t type, EnemyPosture_t posture);
void Sprites_InitPlayer(Sprite_t *sprite, uint8_t x, uint8_t y);
void Sprites_InitBullet(Sprite_t *sprite, uint8_t x, uint8_t y);
void Sprites_InitExplosion(Sprite_t *sprite, uint8_t x, uint8_t y);

#endif