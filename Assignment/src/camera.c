#include "camera.h"
#include "draw.h"
#include <GL/glut.h>
#include <math.h>
#define M_PI 3.14159265358979323846


double degree_to_radian(double degree)
{
	return degree * M_PI / 180.0;
}



void init_camera(struct Camera* camera, Move* move, Rotate* rotate)
{
	
	double angle = degree_to_radian(rotate->boat_rotation);
	
	camera->position.x = move->boat.x;
	camera->position.y = 0;
	camera->position.z = move->boat.y;

	camera->pose.x = 0;
	camera->pose.y = angle;
	camera->pose.z = 0;
	
	camera->CAMERA_SPEED =2;
	camera->MOVE_SPEED = 5;

}



void stay_in(struct Camera* camera,  Move* move, Rotate* rotate){

	if (camera->position.x<-skybox_size|| camera->position.x>skybox_size ||
		camera->position.y<-5|| camera->position.y>2*skybox_size ||
		camera->position.z<-skybox_size || camera->position.z>skybox_size)
		init_camera (camera, move, rotate);

}


void set_view_point(const struct Camera* camera, Move* move, Rotate* rotate)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	double angle = degree_to_radian(rotate->boat_rotation);

	
 if (move->free==1) {
	if (camera->pose.y==0)
			glRotatef((-(rotate->boat_rotation)), 0, 1, 0); 
		else glRotatef(-(camera->pose.y), 0, 1, 0);
	glTranslatef(-move->boat.x, -move->boat.y, -move->boat.z);
	glTranslatef(-sin(angle)*1, -2, -cos(angle)*1);
	glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
	}
	else {	
	glRotatef((-(rotate->boat_rotation)), 0, 1, 0); 
	glTranslatef(-move->boat.x, -move->boat.y, -move->boat.z);
	glTranslatef(-sin(angle)*1, -2, -cos(angle)*1);	
	}
	

}



void rotate_camera(struct Camera* camera, double horizontal, double vertical, Move* move)
{

	camera->pose.y += horizontal/camera->CAMERA_SPEED;

	if (move->free ==1) {
			camera->pose.x += vertical/camera->CAMERA_SPEED;
	
		if (camera->pose.x < 0) {
			camera->pose.x += 360.0;
		}

		if (camera->pose.x > 360.0) {
			camera->pose.x -= 360.0;
		}
	}

	if (camera->pose.y < 0) {
		camera->pose.y += 360.0;
	}

	if (camera->pose.y > 360.0) {
		camera->pose.y -= 360.0;
	}

}


void move_camera_up(struct Camera* camera, double distance)
{

	camera->prev_position = camera->position;
	camera->position.y += distance;

}



void move_camera_down(struct Camera* camera, double distance)
{
	camera->prev_position = camera->position;
	camera->position.y -= distance;
	
}



void move_camera_backward(struct Camera* camera, double distance)
{
	
	camera->prev_position = camera->position;
	double angle = degree_to_radian(camera->pose.y);


	camera->position.x += sin(angle) * distance;
	camera->position.z += cos(angle) * distance;
	

}


void move_camera_forward(struct Camera* camera, double distance)
{
	
	camera->prev_position = camera->position;
	double angle = degree_to_radian(camera->pose.y);

	camera->position.x -= sin(angle) * distance;
	camera->position.z -= cos(angle) * distance;
	


}


void step_camera_right(struct Camera* camera, double distance)
{
		camera->prev_position = camera->position;
	double angle = degree_to_radian(camera->pose.y - 90.0);


	camera->position.x -= sin(angle) * distance;
	camera->position.z -= cos(angle) * distance;
	


}


void step_camera_left(struct Camera* camera, double distance)
{
	camera->prev_position = camera->position;
	double angle = degree_to_radian(camera->pose.y + 90.0);

	camera->position.x -= sin(angle) * distance;
	camera->position.z -= cos(angle) * distance;


}
