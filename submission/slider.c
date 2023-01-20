// Zach Vincent
// Fundcomp lab 11
// slider.c | simple 3D game where the user plays as a cube that slides around the map

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "gfx.h"


// STRUCTS
typedef struct {
	double x;
	double y;
	double z;

	double dx;
	double dy;
	double dz;
} Vector;

typedef struct {
	double x;
	double y;
} Projection;



// MATRIX MATH FUNCTIONS
void mult_matrix(double *m1, double *m2, double *res, int dim1, int dim2, int dim3);
void add_matrix(double *m1, double *m2, double *res, int rows, int cols);
void scalar_mult(double *m1, int scalar, int rows, int cols);
void print_matrix(double *m, int rows, int cols);

// CAMERA CALCULATION FUNCTIONS
Vector camera_transform(Vector cam_pos, Vector cam_theta, Vector point);
Projection proj_point(Vector disp, Vector plane);

// DRAWING FUNCTIONS
void draw_edges(Projection *pts, int pts_len, int *edges, int edges_len, int type);
void draw_cube(int cx, int cy, int z, int r, int type, Vector cam_pos, Vector cam_theta, Vector disp_surface);
int draw_floor(int floor[10][10], int z, int r, Vector cam_pos, Vector cam_theta, Vector disp_surface, Vector player, int* coins);
void draw_player(Vector player, Vector cam_pos, Vector cam_theta, Vector disp_surface);

// OTHER FUNCTIONS
int is_within(Vector player, int x, int y, int r);





int main(int argc, char *argv) {


	gfx_open(650, 650, "Engine");

	
	// CAMERA DATA
	// controls angle
	Vector cam_pos		= {-125, -400, 600};

	// controls orbiting
	Vector cam_theta	= {0, 0, 0};

	// controls panning
	Vector disp_surface	= {650, 1050, 825};

	// PLAYER DATA
	Vector player = {60, 200, 0};



	// LEVEL REPRESENTATION
	int floor[10][10] = {{0,0,0,0,0,1,1,1,3,0}, //1
						 {0,0,0,0,0,1,0,0,0,0}, //2
						 {0,0,6,1,1,1,0,1,1,1},
						 {0,0,0,1,0,0,0,1,0,1},
						 {0,0,0,1,1,1,1,1,0,1}, //5
						 {6,1,0,0,6,0,0,0,0,1},
						 {0,1,0,0,0,0,1,1,1,1},
						 {0,1,1,1,1,0,1,0,0,1},
						 {0,1,0,0,1,1,1,0,0,1},
						 {0,1,2,0,0,0,0,0,6,1}};//10

	
	// how much each parameter changes when a key is pressed
	player.dx = -2;
	player.dy = -2;
	player.dz = 0;

	cam_pos.dx = 4;
	cam_pos.dy = 4;
	cam_pos.dz = 4;


	Vector* param = &player;
	
	// print instructions to screen
	gfx_text(250,200,"CUBE SLIDER");
	gfx_text(180,250,"press any key to begin, ESC to quit");
	gfx_text(180,300,"use WASD to move and collect the coins!");
	gfx_text(180,350,"make your way to the green cube to win.");
	gfx_text(180,400,"use the Q and E keys to perspective zoom.");

	char c, msg[40];
	int coins = 0, res;

	// main loop
	while (1){

		c = gfx_wait();

		// quit
		if (c == 27) {
			break;

		} 
		
		// movements; change parameter, which is implemented as a pointer
		// to save time editing during debugging
		else if (c == 'w') {
			param->y -= param->dy;

		} else if (c == 's') {
			param->y += param->dy;

		} else if (c == 'a') {
			param->x -= param->dx;

		} else if (c == 'd') {
			param->x += param->dx;

		} else if (c == 'q') {
			cam_pos.z -= cam_pos.dz;

		} else if (c == 'e') {
			cam_pos.z += cam_pos.dz;

		}


		// redraw everything
		gfx_clear();

		res = draw_floor(floor, 0, 20, cam_pos, cam_theta, disp_surface, player, &coins);
		
		// show number of coins user has collected
		sprintf(msg, "coins: %d", coins);
		gfx_color(255, 255, 0);
		gfx_text(200, 150, msg);

		// draw the player
		draw_player(player, cam_pos, cam_theta, disp_surface);

		gfx_flush();

		// after drawing the screen, handle if user fell off the edge or won
		if (res == 2) {
			player.x = 60;
			player.y = 200;
			usleep(500000);
			while (gfx_event_waiting()) c = gfx_wait();
		} else if (res == 1) {
			gfx_clear();
			gfx_color(200, 200, 100);
			gfx_text(270, 300, "YOU WIN! Press any key to exit.");
			usleep(500000);
			c = gfx_wait();

			break;
		}

	}
	
	return 0;
}


// 
// MATRIX MULTIPLICATION
//
void mult_matrix(double *m1, double *m2, double *res, int dim1, int dim2, int dim3) {

	double sum = 0;

	// i : row of m1
	for (int i = 0; i < dim1; i++) {

		// j : col of m2
		for (int j = 0; j < dim3; j++) {
			sum = 0;

			// k : col of m1, row of m2
			for (int k = 0; k < dim2; k++) {
				sum += (m1[i*dim1 + k] * m2[k*dim3 + j]);
			}

			*res = sum;
			res++;
			
		}
	}

}



// 
// SCALAR MULTIPLICATION TO MATRIX
//
void scalar_mult(double *m1, int scalar, int rows, int cols) {

	for (int i = 0; i < rows*cols; i++) {
		m1[i] *= scalar;
	}

}


//
// MATRIX ADDITION
//
void add_matrix(double *m1, double *m2, double *res, int rows, int cols) {
	
	for (int i = 0; i < rows*cols; i++) {
		res[i] = m1[i] + m2[i];
	}

}



// 
// PRINT MATRICES
//
void print_matrix(double *m, int rows, int cols) {



	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {

		
			printf("%.2f\t", *m);
			m++;
			
		}
		printf("\n");

	}

}


//
// CAMERA TRANSFORM
//
Vector camera_transform(Vector cam_pos, Vector cam_theta, Vector point) {
	
	// use transformation matrices to calculate distance from camera to 3D point
	double tx[9] = 			{1,					0,					0,
					   		 0,					cos(cam_theta.x),	sin(cam_theta.x),
					   		 0,					-sin(cam_theta.x),	cos(cam_theta.x)};

	double ty[9] = 			{cos(cam_theta.y),	0,					-sin(cam_theta.y),
					   		 0,					1,					0,
					   		 sin(cam_theta.y),	0,					cos(cam_theta.y)};
	
	double tz[9] = 			{cos(cam_theta.z),	sin(cam_theta.z),	0,
					   		 -sin(cam_theta.z),	cos(cam_theta.z),	0,
					   		 0,					0,					1};

	double temp[9];
	double transform[9];

	mult_matrix(tx, ty, temp, 3, 3, 3);
	mult_matrix(temp, tz, transform, 3, 3, 3);

	double dist[3];
	dist[0] = point.x - cam_pos.x;
	dist[1] = point.y - cam_pos.y;
	dist[2] = point.z - cam_pos.z;

	double final_dist[3];

	mult_matrix(transform, dist, final_dist, 3, 3, 1);

	Vector ret;
	ret.x = final_dist[0];
	ret.y = final_dist[1];
	ret.z = final_dist[2];

	return ret;

}


// 
// GET TRANSFORMED POINT
//
Projection proj_point(Vector disp, Vector plane) {

	Projection proj;

	// use distance from 3D point to camera with other parameters to project shape
	// onto 2D plane
	proj.x = (plane.z/disp.z * disp.x) + plane.x;
	proj.y = (plane.z/disp.z * disp.y) + plane.y;

	return proj;

}

void draw_edges(Projection *pts, int pts_len, int *edges, int edges_len, int type) {


	// change color based on cube type
	
	int r, g, b;
	
	// floor block or block with coin
	if (type == 1 || type == 6) {
		r = 200;
		g = 100;
		b = 0;

	// starting block
	} else if (type == 2) {
		r = 0;
		g = 100;
		b = 200;

	// goal block
	} else if (type == 3) {
		r = 0;
		g = 200;
		b = 100;

	// player
	} else if (type == 4) {
		r = 255;
		g = 255;
		b = 255;
	
	// coin
	} else if (type == 5) {
		r = 255;
		g = 255;
		b = 0;
	} else {
		printf("ERR: unknown cube type\n");
	}


	// iterate through all the pairs in the edges[] list; these pairs
	// represent the indices of points in the cubes[] list that will
	// be connected
	Projection p1, p2;
	
	gfx_color(r/3, g/3, b/3);
	for (int i = 0; i < edges_len/2; i++) {
		p1 = pts[edges[2*i]];
		p2 = pts[edges[(2*i)+1]];

		if (i >= (edges_len/2)-4) gfx_color(r, g, b);

		gfx_line(p1.x, p1.y, p2.x, p2.y);
	}


}

// 
// DRAW CUBE
//
void draw_cube(int cx, int cy, int z, int r, int type, Vector cam_pos, Vector cam_theta, Vector disp_surface) {


	// generate cube coordinates
	Vector cube[8];

	// four corners of the cube
	//					x		y		z
	float points[24] = {cx-r,	cy-r,	z-r,
						cx-r,	cy,		z-r,
						cx,		cy,		z-r,
						cx,		cy-r,	z-r,
						cx-r,	cy-r,	z,
						cx-r,	cy,		z,
						cx,		cy,		z,
						cx,		cy-r,	z};


	// represents indices of vertex pairs that should be connected by a line	
	int edges[24] = {0, 1,
					 0, 3,
					 0, 4,
					 1, 5,
					 2, 1,
					 2, 3,
					 2, 6,
					 3, 7,
					 4, 5,
					 4, 7,
					 6, 5,
					 6, 7};
						
	// cube - list of 8 Vectors representing each corner of cube
	for (int i = 0; i < 8; i++) {
		cube[i].x = points[i*3 + 0];
		cube[i].y = points[i*3 + 1];
		cube[i].z = points[i*3 + 2];
	}

	
	Vector d_vect[8];
	Projection points2D[8];

	// create list of corresponding 2D points for each 3D point
	for (int i = 0; i < 8; i++) {
		d_vect[i] = camera_transform(cam_pos, cam_theta, cube[i]);
		points2D[i] = proj_point(d_vect[i], disp_surface);
	}

	// draw edges and print to screen
	draw_edges(points2D, 8, edges, 24, type);

	gfx_flush();

}
	
	
// 
// DRAW FLOOR (DRAWS LEVEL)
//
int draw_floor(int floor[10][10], int z, int r, Vector cam_pos, Vector cam_theta, Vector disp_surface, Vector player, int *coins) {

	double sx = 25, sy = 25, x, y;
	char msg[40];
	int fell = 0;
	float dist;

	// iterate through 2D floor[][] array and place cubes corresponding to 
	// specified block type
	for (int dy = 0; dy < 10; dy++) {
		for (int dx = 0; dx < 10; dx++) {
			
			// x,y location of cube's center
			x = sx + (dx*r);
			y = sy + (dy*r);


			// place cube
			if (floor[dy][dx] != 0) draw_cube(x, y, z, r, floor[dy][dx], cam_pos, cam_theta, disp_surface);
			
			// CHECK COLLISIONS
			// check if player is out of bounds
			if (floor[dy][dx] == 0 && is_within(player, x, y, 8)) {
				fell = 1;

				gfx_color(255,0,0);
				gfx_text(250,200,"YOU FELL OFF! Press any key to try again.");

				player.x = 60;
				player.y = 200;

			// check if player got a coin
			} else if (floor[dy][dx] == 6 && is_within(player, x, y, 5)) {
				floor[dy][dx] = 1;
				(*coins)++;

			// check if player is in win square
			} else if (floor[dy][dx] == 3 && is_within(player, x, y, 5)) {
				return 1;
			}

		}
	}


	// draw coins (drawn last so they render on top of the other cubes)
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {

			if (floor[i][j] == 6) {
				x = 20 + (j*r);
				y = 20 + (i*r);
				draw_cube(x, y, z+10, 10, 5, cam_pos, cam_theta, disp_surface);

			}

		}
	}

	if (fell) return 2;
	return 0;

}


//
// DRAW PLAYER
//
void draw_player(Vector player, Vector cam_pos, Vector cam_theta, Vector disp_surface) {

	int sx = 25, sy = 25, r = 10;

	draw_cube(player.x, player.y, player.z+r, r, 4, cam_pos, cam_theta, disp_surface);

}


// 
// CHECK IF WITHIN A BOUNDARY
//
int is_within(Vector player, int x, int y, int r) {

	// if the player cube is within r units of point (x, y)
	if(player.x + 5 <= x + r && player.x + 5 >= x - r) {
		if (player.y + 5 <= y + r && player.y + 5 >= y - r) {
			return 1;
		}
	}

	// check if player is out of bounds of floor[][] array
	if (player.x >= 215 || player.x < 5 || player.y >= 215 || player.y < 5) {
		return 1;
	}

	// return 0 if not within specified point
	return 0;

}
