#ifndef DRAW_H
#define DRAW_H
#include "model.h"


typedef struct Position
{
	double x;
	double y;
	double z;
}Position;

typedef struct {
	double boat_rotation;
	double boat_rocking;
	int rock;
}Rotate;

typedef struct{
	Position boat;
	int free;
}Move;


//Draw the model.
void draw_model(const struct Model* model);


//Draw the triangles of the model.
void draw_triangles(const struct Model* model);


//Draw the quads of the model.
void draw_quads(const struct Model* model);


//Draw the normal vectors of the model.
void draw_normals(const struct Model* model, double length);


//Draw the skybox.
void draw_skybox(Complicated skybox);


//Draw the environment
void draw_environment(World world,Rotate* rotate, Move move);


#endif
