#include <windows.h>
#include "draw.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "camera.h"
#include <GL/gl.h>


void draw_triangles(const struct Model* model)
{
    int i, k;
    int vertex_index, texture_index, normal_index;
    double x, y, z, normal_x, normal_y, normal_z, u, v;

    glBegin(GL_TRIANGLES);

    for (i = 0; i < model->n_triangles; ++i) {
        for (k = 0; k < 3; ++k) {

            texture_index = model->triangles[i].points[k].texture_index;
            u = model->texture_vertices[texture_index].u;
            v = model->texture_vertices[texture_index].v;
            glTexCoord2f(u, 1-v);

            normal_index = model->triangles[i].points[k].normal_index;
            normal_x = model->normals[normal_index].x;
            normal_y = model->normals[normal_index].y;
            normal_z = model->normals[normal_index].z;
            glNormal3d(normal_x, normal_y, normal_z);

            vertex_index = model->triangles[i].points[k].vertex_index;
            x = model->vertices[vertex_index].x;
            y = model->vertices[vertex_index].y;
            z = model->vertices[vertex_index].z;
            glVertex3d(x, y, z);
        }
    }

    glEnd();
}


void draw_quads(const struct Model* model)
{
    int i, k;
    int vertex_index, texture_index, normal_index;
    double x, y, z, u, v;

    glBegin(GL_QUADS);

    for (i = 0; i < model->n_quads; ++i) {
        for (k = 0; k < 4; ++k) {

            texture_index = model->quads[i].points[k].texture_index;
            u = model->texture_vertices[texture_index].u;
            v = model->texture_vertices[texture_index].v;
            // NOTE: The 1-v is model file specific!
            glTexCoord2f(u, 1-v);

            vertex_index = model->quads[i].points[k].vertex_index;
            x = model->vertices[vertex_index].x;
            y = model->vertices[vertex_index].y;
            z = model->vertices[vertex_index].z;
            glVertex3d(x, y, z);
        }
    }

    glEnd();
}

void draw_normals(const struct Model* model, double length)
{
    int i;
    double x1, y1, z1, x2, y2, z2;

    glColor3f(0, 0, 1);

    glBegin(GL_LINES);

    for (i = 0; i < model->n_vertices; ++i) {
        x1 = model->vertices[i].x;
        y1 = model->vertices[i].y;
        z1 = model->vertices[i].z;
        x2 = x1 + model->normals[i].x * length;
        y2 = y1 + model->normals[i].y * length;
        z2 = z1 + model->normals[i].z * length;
        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y2, z2);
    }

    glEnd();
}



void draw_model(const struct Model* model)
{
	draw_triangles(model);
	draw_quads(model);
}



void draw_skybox(Complicated skybox)
{
	int D= skybox_size;
  glEnable(GL_TEXTURE_2D);
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, skybox.material_ambient);

  glBindTexture(GL_TEXTURE_2D,skybox.texture[1]);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(-D,-5,-D);
  glTexCoord2f(1,0); glVertex3f(+D,-5,-D);
  glTexCoord2f(1,1); glVertex3f(+D,+2*D,-D);
  glTexCoord2f(0,1); glVertex3f(-D,+2*D,-D);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,skybox.texture[2]);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(+D,-5,-D);
  glTexCoord2f(1,0); glVertex3f(+D,-5,+D);
  glTexCoord2f(1,1); glVertex3f(+D,+2*D,+D);
  glTexCoord2f(0,1); glVertex3f(+D,+2*D,-D);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,skybox.texture[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(+D,-5,+D);
  glTexCoord2f(1,0); glVertex3f(-D,-5,+D);
  glTexCoord2f(1,1); glVertex3f(-D,+2*D,+D);
  glTexCoord2f(0,1); glVertex3f(+D,+2*D,+D);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,skybox.texture[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(-D,-5,+D);
  glTexCoord2f(1,0); glVertex3f(-D,-5,-D);
  glTexCoord2f(1,1); glVertex3f(-D,+2*D,-D);
  glTexCoord2f(0,1); glVertex3f(-D,+2*D,+D);
  glEnd();

  glBindTexture(GL_TEXTURE_2D,skybox.texture[5]);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(-D,+2*D,-D);
  glTexCoord2f(1,0); glVertex3f(+D,+2*D,-D);
  glTexCoord2f(1,1); glVertex3f(+D,+2*D,+D);
  glTexCoord2f(0,1); glVertex3f(-D,+2*D,+D);
  glEnd();

  //floor
	glBindTexture(GL_TEXTURE_2D,skybox.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(1,1); glVertex3f(+D,-5,-D);
	glTexCoord2f(0,1); glVertex3f(-D,-5,-D);
	glTexCoord2f(0,0); glVertex3f(-D,-5,+D);
	glTexCoord2f(1,0); glVertex3f(+D,-5,+D);
	glEnd(); 

}




void draw_environment(World world, Rotate* rotate, Move move ){
	
	int D = skybox_size;
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	

	//Draw the  skybox.
	draw_skybox(world.skybox);
	

	
   //Draw the boaaaaaat
     glPushMatrix();

    		glTranslatef(move.boat.x, move.boat.y, move.boat.z);
			
    		glBindTexture(GL_TEXTURE_2D, world.boat.texture[0]);
			glBindTexture(GL_TEXTURE_2D, world.boat.texture[1]);
    		glScalef(1.0f, 1.0f, 1.0f);
			glRotatef(rotate->boat_rotation, 0, 1, 0);
			
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			
    		glMaterialfv(GL_FRONT, GL_AMBIENT, world.boat.material_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
			draw_model(&world.boat.model);

   glPopMatrix();
   
   
   

   //draw an island
   
     glPushMatrix();


    		glBindTexture(GL_TEXTURE_2D, world.island.texture[0]);
			glBindTexture(GL_TEXTURE_2D, world.island.texture[1]);
			glBindTexture(GL_TEXTURE_2D, world.island.texture[2]);
			glBindTexture(GL_TEXTURE_2D, world.island.texture[3]);
    		glScalef(1.0f, 1.0f, 1.0f);
			glTranslatef (D*0.8, -50, D*0.8);
			
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			
    		glMaterialfv(GL_FRONT, GL_AMBIENT, world.island.material_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    		draw_model(&world.island.model);

   glPopMatrix();
   
 //draw a treasure chest
   
      glPushMatrix();

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	    		//glMaterialfv(GL_FRONT, GL_AMBIENT, world.chest.material_ambient);
				glBindTexture(GL_TEXTURE_2D, world.chest.texture);	
				glTranslatef (D*0.8-100,100 , D*0.8-100);
			    glRotatef(180, 0, 1, 0);
				glScalef(20.0f, 20.0f, 20.0f);
				
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
			glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 120);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
			
    		draw_model(&world.chest.model);

   glPopMatrix();
   


}


