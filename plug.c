#include "raylib.h"
#include <stdint.h>
#include <math.h>
#include "plug.h"

#define player_size 20
#define WIDTH 800
#define SCENE_WIDTH 1920
#define HEIGHT 1080
#define mapX 8
#define mapY 8
#define DR 0.017453292519943295f
#define BACKGROUND 0x101010FF
#define PLAYER_ANGLE 10
#define PROJECTION_DIST 1662
#define WALL_HEIGHT 692.8f
#define FOV 60.0f

int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,1,0,0,1,
	1,0,1,0,1,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,0,1,0,1,
	1,0,1,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawRays(Player *p)
{
	float yo, xo;
	float ra = p->angle - 30.0f * DR;
	for (int r = 0; r < SCENE_WIDTH; r++, ra += FOV * DR/SCENE_WIDTH)
	{
		int dof = 0;
		float aTan = -1/tanf(ra);
		float hx, hy;
		if (ra < 0)
			ra += 2*PI;
		if (ra > 2*PI)
			ra -= 2*PI;
		// NOTE: find horizontal intersection
		// looking up
		if (ra > PI) {
			hy = floorf(p->y) - 0.001f;
			hx = (p->y - hy)*aTan + p->x;
			yo = -1.0f;
			xo = aTan;
		}
		// looking down
		else if (ra < PI && ra > 0) {
			hy = floorf(p->y) + 1.0f;
			hx = (p->y - hy)*aTan + p->x;
			yo = 1.0f;
			xo = -aTan;
		}
		else {
			hx = p->x;
			hy = p->y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(hx);
			int my = (int)(hy);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				hx += xo;
				hy += yo;
				dof++;
			}
		}
		// NOTE: find vertical intersection
		aTan = -tanf(ra);
		dof = 0;
		float vx = p->x, vy = p->y;
		// looking right
		if ((ra > 3*PI/2 && ra < 2*PI) || (ra < PI/2 && ra > 0))
		{
			vx = floorf(p->x) + 1.0f;
			vy = p->y + (p->x - vx)*aTan;
			xo = 1.0f;
			yo = -aTan;
		}
		// looking left
		else if (ra > PI/2 && ra < 3*PI/2)
		{
			vx = floorf(p->x) - 0.001f;
			vy = p->y + (p->x - vx)*aTan;
			xo = -1.0f;
			yo = aTan;
		}
		else {
			vx = p->x;
			vy = p->y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(vx);
			int my = (int)(vy);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				vx += xo;
				vy += yo;
				dof++;
			}
		}

		float distH = (hy - p->y)*(hy - p->y) + (hx - p->x)*(hx - p->x);
		float distV = (vy - p->y)*(vy - p->y) + (vx - p->x)*(vx - p->x);
		float dist = distV > distH ? distH : distV;
		dist = sqrtf(dist);
		float corrected_dist = dist * cosf(ra - p->angle);

		// NOTE: draw wall
		float wall_height = WALL_HEIGHT/corrected_dist;
		if (wall_height > HEIGHT)
			wall_height = HEIGHT;
		float line_offset = (HEIGHT - wall_height)/2.0f;
		DrawRectangle(r, (int)line_offset, 1, (int)wall_height, SKYBLUE);
	}
}

void render(Player *player) {
	BeginDrawing();
	ClearBackground(GetColor(BACKGROUND));
	drawRays(player);
	EndDrawing();
}
