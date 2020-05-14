#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "SOIL/SOIL.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "model.h"
#include "draw.h"
#include "camera.h"
#include <sys/time.h>


/**
	Vadon Eniko
	DW9YG6
	2020.05.08.
**/


struct Camera camera;

GLfloat light_position[] = {0, 0, 0, 0};
GLfloat light_ambient[] = { 0.8, 0.8, 0.8, 0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0, 0 };
GLfloat light_specular[] = { 1, 1, 1, 0 };
GLfloat spot_direction[] = { 300, -100, 200};
float fogDensity = 0.00065;




static float front_mat_shininess[] = {30.0};
static float front_mat_specular[] = {1.0, 1.0, 1.0, 1.0};
static float front_mat_diffuse[] = {0.0, 1.0, 0.0, 1.0};
static float back_mat_shininess[] = {50.0};
static float back_mat_specular[] = {0.0, 0.0, 1.0, 1.0};
static float back_mat_diffuse[] = {1.0, 0.0, 0.0, 1.0};


typedef GLubyte Pixel;

struct Game game;
struct Game
{
	int move_forward;
	int move_backward;
	int step_left;
	int step_right;
	int move_up;
	int move_down;
	int rotate_boat_l;
	int rotate_boat_r;
	int move_boat_f;
	int move_boat_b;
	int increase_light;
	int decrease_light;
	int increase_fog;
	int decrease_fog;
	int enable_free_cam;
	int collision;
	int increase_speed;
	int decrease_speed;
	int increase_Mspeed;
	int decrease_Mspeed;
};

void initialize();

World world;
Rotate rotate;
Move move;

int checkpoint;

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int mouse_x, mouse_y;
int previous_time;
int help, help_on = 1;
float speed = 60;



double calc_elapsed_time()
{
	int current_time;
	double elapsed_time;

	current_time = glutGet(GLUT_ELAPSED_TIME);
	elapsed_time = (double)(current_time - previous_time) / 1000.0;
	previous_time=current_time;

	return elapsed_time;
}

//Update the environment
void update_environment(struct Camera* camera, double elapsed_time)
{
	double distance;

	distance = elapsed_time * camera->MOVE_SPEED *speed;

	if (game.move_forward == TRUE) {
		move_camera_forward(camera, distance);
	}

	if (game.move_backward == TRUE) {
		move_camera_backward(camera, distance);
	}

	if (game.step_left == TRUE) {
		step_camera_left(camera, distance);
	}

	if (game.step_right == TRUE) {
		step_camera_right(camera, distance);
	}

	if (game.move_up == TRUE) {
		move_camera_up(camera, distance);
	}

	if (game.move_down == TRUE) {
		move_camera_down(camera, distance);
	}

	if (game.increase_light == TRUE) {
		if (light_ambient[0] < 1)
			light_ambient[0] = light_ambient[1] = light_ambient[2] += 0.05;
	}

	if (game.decrease_light == TRUE) {
		if (light_ambient[0] > -1)
			light_ambient[0] = light_ambient[1] = light_ambient[2] -= 0.05;
	}
	
	if (game.increase_fog== TRUE) {
		fogDensity+=0.0005;
	}
	
	if (game.decrease_fog== TRUE) {
		fogDensity-=0.0005;
	}
	
	if (game.enable_free_cam == TRUE) {

		move.free=1;
		set_view_point(camera, &move, &rotate);
	}
	
	if (game.enable_free_cam == FALSE) {

		move.free=0;
		set_view_point(camera, &move, &rotate);
	}

	if (game.increase_speed) {
		if (camera->CAMERA_SPEED>1)
		camera->CAMERA_SPEED -=1;
	}
	if (game.decrease_speed) {
		if (camera->CAMERA_SPEED<10)
		camera->CAMERA_SPEED +=1;
	}

	
	stay_in (camera, &move, &rotate);

}

update_light( Move* move) {
	
	GLfloat light_position2[] = {-move->boat.x, -move->boat.y, -move->boat.z, 0.5};
	GLfloat light_diffuse2[] = { 1, 0.8, 0.8, 0.5 };
	
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
   //glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
	
}

//Move the boat at the basic speed of the camera.
void rowing (Move* move, Rotate* rotate, double elapsed_time, struct Camera* camera){
	
	double distance;
	distance = elapsed_time * camera->MOVE_SPEED *speed;
	if (game.increase_Mspeed == TRUE) distance = distance*2;
	double angle;
	angle = degree_to_radian(rotate->boat_rotation);

if (game.move_boat_f == TRUE && game.collision==FALSE)
    {
		move->boat.x -=distance * sin(angle);
		move->boat.y =0;
		move->boat.z -=distance * cos(angle);
		
	}
	else if (game.move_boat_f == TRUE && game.collision==TRUE) {
		move->boat.x -=distance * sin(angle);
		move->boat.y =0;
		move->boat.z = move->boat.z;
	}


if (game.move_boat_b == TRUE)
	  {move->boat.x +=distance * sin(angle);
		move->boat.y =0;
		move->boat.z +=distance * cos(angle) ;
	}

}

//Checks if the player has found the island
void collision (Move* move) {
	int D = (0.55+checkpoint*0.09)*skybox_size;
	
	if(move->boat.x >= D && move->boat.z >=D){
		checkpoint+=1;
	initialize();
	}

}

//Rotate the boat with the Q and E keys.
void rotate_boat (Rotate* rotate) {
	
	if (move.free==0) {
		if (game.rotate_boat_l==TRUE) rotate->boat_rotation +=1;
			else if (game.rotate_boat_r==TRUE) rotate->boat_rotation -=1;
	}
	
}

//Rotate the boat with the mouse
void rotate_boat_m(Rotate* rotate, double horizontal, struct Camera* camera)
{
	rotate->boat_rotation += horizontal/camera->CAMERA_SPEED;

	if (rotate->boat_rotation < 0) {
		rotate->boat_rotation += 360.0;
	}

	if (rotate->boat_rotation> 360.0) {
		rotate->boat_rotation -= 360.0;
	}


}

//Handler for special key(s)
void specialFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		if (help_on) {
			help_on = 0;

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
		}else {
			help_on = 1;
			GLfloat ones[] = { 1, 1, 1, 1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ones);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ones);
		}
	}
}

//Reshapes the screen when the help is called. 
void reshape(GLsizei width, GLsizei height) {
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!help_on)
		gluPerspective(50.0, (GLdouble)width / (GLdouble)height, 0.1, 20000.0);
	else
		gluOrtho2D(0, width, height, 0);
}

//Funciton to draw the help. if the final checkpoint has been reached, sleep and exit.
void draw_help() {

	GLfloat ones[] = { 1, 1, 1, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ones);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ones);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, help);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(WINDOW_WIDTH, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, WINDOW_HEIGHT, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutSwapBuffers();
	
	if (checkpoint==3)
	{
		sleep(5);
		exit(0);
	}
}

//Display function
void display() {
	if (!help_on) {
		double elapsed_time;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		elapsed_time = calc_elapsed_time();
		update_environment(&camera,  elapsed_time);
		update_light(&move);
		rotate_boat(&rotate);
		rowing(&move, &rotate, elapsed_time, &camera);
		set_view_point(&camera, &move, &rotate);
		collision(&move);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	

	glEnable(GL_FOG);
	GLfloat fogColor[4]= {0.6f, 0.6f, 0.6f, 1.0f};
	glFogi(GL_FOG_MODE, GL_EXP);       
	glFogfv(GL_FOG_COLOR, fogColor);         
	glFogf(GL_FOG_DENSITY, fogDensity);  
	glHint(GL_FOG_HINT, GL_FASTEST);  
	glFogf(GL_FOG_START, 0); 
	glFogf(GL_FOG_END, 1); 

		
		draw_environment(world, &rotate, move);
		reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutSwapBuffers();

	}

	else {
		draw_help();
	}
	
}

//Mouse handler
void mouse_handler(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;

}


void motion_handler(int x, int y)
{
	double horizontal, vertical;

	horizontal = mouse_x - x;
	vertical = mouse_y - y;
	
	rotate_camera(&camera, horizontal, vertical, &move);
	if(move.free ==0) rotate_boat_m(&rotate, horizontal, &camera);

	mouse_x = x;
	mouse_y = y;

	glutPostRedisplay();

}

//Basic key handler
void key_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w': 
		if	(move.free==1)game.move_forward = TRUE;	
		if	(move.free==0){
			if (game.move_boat_f == FALSE)
				game.move_boat_f = TRUE;
			else game.move_boat_f = FALSE;
		}
		break;
	case 's':
		if (move.free ==1) game.move_backward = TRUE;
		if(move.free==0){
			if (game.move_boat_b == FALSE)
			game.move_boat_b = TRUE;
			else game.move_boat_b = FALSE;
		}
		break;
	case 'a':
		game.step_left = TRUE;
		break;
	case 'd':
		game.step_right = TRUE;
		break;
	case 'c':
		game.move_down = TRUE;
		break;
	case 32:
		game.move_up = TRUE;
		break;
	case 'q':	
		if (game.rotate_boat_l == FALSE)
			game.rotate_boat_l = TRUE;
		else game.rotate_boat_l= FALSE;
		break;
	case 'e':
		if (game.rotate_boat_r == FALSE)
			game.rotate_boat_r = TRUE;
		else game.rotate_boat_r= FALSE;
		break;
	case '+':
		game.increase_light = TRUE;
		break;
	case '-':
		game.decrease_light = TRUE;
		break;
	case 'o':
	if (checkpoint >0){
		game.increase_fog = TRUE;
	}
		break;
	case 'p':
		if (checkpoint >0){
		game.decrease_fog = TRUE;
		}
		break;
	case 'n':
		game.increase_speed = TRUE;
		break;
	case 'm':
		game.decrease_speed = TRUE;
		break;
	case 'r' :
		if (game.increase_Mspeed==TRUE)
		game.increase_Mspeed=FALSE;
		else game.increase_Mspeed = TRUE;
		break;
	case 'f':
	if (checkpoint >1){
		if (game.enable_free_cam == TRUE)
			game.enable_free_cam=FALSE;
		else if (game.enable_free_cam==FALSE)
			game.enable_free_cam=TRUE;
	}
	break;
	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

//Key up handler
void key_up_handler(unsigned char key, int x, int y)
{
	int modifier = glutGetModifiers();
	switch (key) {
	case 'w':
		game.decrease_Mspeed=TRUE;
		game.move_forward = FALSE;
		game.move_boat_f = FALSE;
		break;
	case 's':
		game.decrease_Mspeed=TRUE;
		game.move_backward = FALSE;
		game.move_boat_b = FALSE;
		break;
	case 'a':
		game.step_left = FALSE;
		break;
	case 'd':
		game.step_right = FALSE;
		break;
	case 'c':
		game.move_down = FALSE;
		break;
	case 32:
		game.move_up = FALSE;
		break;
	case 'q':
		game.rotate_boat_l = FALSE;
		break;
	case 'e':
		game.rotate_boat_r = FALSE;
		break;
	case '+':
		game.increase_light = FALSE;
		break;
	case '-':
		game.decrease_light = FALSE;
		break;
	case 'o':
		game.increase_fog = FALSE;
		break;
	case 'p':
		game.decrease_fog = FALSE;
		break;
	case 'n':
		game.increase_speed = FALSE;
		break;
	case 'm':
		game.decrease_speed = FALSE;
		break;
	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}

GLuint load_texture(const char* filename) {
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	GLuint texture_name;
	Pixel* image;
	glGenTextures(1, &texture_name);

	int width;
	int height;

	image = (Pixel*)SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA); 

	glBindTexture(GL_TEXTURE_2D, texture_name);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (Pixel*)image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	return texture_name;
}

void set_callbacks() {
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_handler);
	glutKeyboardUpFunc(key_up_handler);
	glutMouseFunc(mouse_handler);
	glutMotionFunc(motion_handler);
	glutIdleFunc(idle);
	glutSpecialFunc(specialFunc);
	glutFullScreen();
}

void initialize()
{
	time_t t;
	srand((unsigned) time(&t));
	if (checkpoint ==0){
	help = load_texture("textures//help2.jpg");
	skybox_size=3000;
	rotate.boat_rotation=rand() % 360;
	help_on=TRUE;
	}
	else if (checkpoint ==1){
	help = load_texture("textures//help1.png");
	skybox_size=5000;
	rotate.boat_rotation=rand() % 360;
	move.boat.x=move.boat.z=0;
	help_on=TRUE;
	}
	else if (checkpoint ==2) {help = load_texture("textures//help.png");
	skybox_size= 7000;
	fogDensity = 0.00065;
	rotate.boat_rotation=rand() % 360;
	move.boat.x=move.boat.z=0;
	help_on=TRUE;
	}
	else if (checkpoint==3) {
	help = load_texture("textures//finish.png");	
	skybox_size=3000;
	help_on=TRUE;
	};

	
	set_callbacks();
	init_camera(&camera, &move, &rotate);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	
	light_ambient[0] = light_ambient[1]= light_ambient[2] =0.8;
	

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glClearColor(0.1, 0.1, 0.1, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearDepth(1.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
	//glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);	
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	
    init_entities(&world);
	
    glEnable(GL_TEXTURE_2D);

}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int wnd = glutCreateWindow("Dive deep");
	glutSetWindow(wnd);
	initialize();

    game.move_forward = FALSE;
    game.move_backward = FALSE;
    game.step_left = FALSE;
    game.step_right = FALSE;
    game.rotate_boat_l = FALSE;
	game.rotate_boat_r = FALSE;
	game.move_boat_f = FALSE;
	game.move_boat_b = FALSE;
	game.enable_free_cam = FALSE;
	game.increase_Mspeed=FALSE;
	game.collision=FALSE;
	move.free=0;
	checkpoint=0;

	glutMainLoop();
	return 0;
}
