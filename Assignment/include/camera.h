#ifndef CAMERA_H
#define CAMERA_H
#include "model.h"
#include "draw.h"


struct Camera
{
	struct Vertex position;
	struct Vertex pose;
	struct Vertex prev_position;
	int CAMERA_SPEED; 
	int MOVE_SPEED;
};


//Converts degree to radian.
double degree_to_radian(double degree);


//If the camera is about to leave the skybox, re-initialize it. 
void stay_in (struct Camera* camera,  Move* move, Rotate* rotate);


//Initialize the camera position and direction.
void init_camera(struct Camera* camera, Move* move, Rotate* rotate);


//Transform the models into the view point of the camera.
void set_view_point(const struct Camera* camera, Move* move, Rotate* rotate);


//Rotate the camera horizontally and vertically.
void rotate_camera(struct Camera* camera, double horizontal, double vertical, Move* move);


//Move the camera forward.
void move_camera_forward(struct Camera* camera, double distance);


//Move the camera backward.
void move_camera_backward(struct Camera* camera, double distance);


//Step the camera left.
void step_camera_left(struct Camera* camera, double distance);


//Step the camera right.
void step_camera_right(struct Camera* camera, double distance);


//Move the camera up.
void move_camera_up(struct Camera* camera, double distance);


//Move the camera down.
void move_camera_down(struct Camera* camera, double distance);


#endif
