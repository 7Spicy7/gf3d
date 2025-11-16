#include "simple_logger.h"

#include "gfc_input.h"
#include "entity.h"

#include "gf3d_camera.h"
#include "camera_entity.h"


void camera_entity_free(Entity* self);
void camera_entity_think(Entity* self);
void camera_entity_update(Entity* self);

typedef struct {
	Entity* target;

	float followHeight;
	float followDistance;
	float angle;

}CameraEntityData;

void camera_entity_free(Entity* self) {
	CameraEntityData* data;
	if ((!self) || (!self->data) ) {
		return;
	}

	data = self->data;

	//Clean up the data here.

	free(data);  //free the pointer itself bc we malloc'ed it
	self->data = NULL; //Set it to null incase we try to use it again. We don't want to accidentally access garbage data
}

//This one was when data->target was just a vector
/*void camera_entity_think(Entity* self) {
	float move = 5;
	float turn = 0.01;		//The rate at which I turn
	float pitch = 0;		//Pan up and down will be my Pitch
	float yaw = 0;			//Pan left and right will be my Yaw

	GFC_Vector3D movement = { 0 };
	GFC_Vector3D dir = { 0 };

	CameraEntityData *data;
	if ((!self) || (!self->data)) return;
	data = self->data;

	//Movies:
	if (gfc_input_command_down("walkforward")) {
		slog("FORWARD PRESSED");
		movement.y -= move;
	}
	if (gfc_input_command_down("walkbackward")) { //wait no this checks out actually. I just happen to be LOOKING in the negative y direction.
		//slog("BACK PRESSED");
		movement.y += move;
	}
	if (gfc_input_command_down("walkright")) {
		//slog("RIGHT PRESSED");
		movement.x -= move;
	}
	if (gfc_input_command_down("walkleft")) {
		//slog("LEFT PRESSED");
		movement.x += move;
	}

	if (gfc_input_command_down("jump")) {
		//slog("FORWARD PRESSED");
		movement.z += move;
	}
	if (gfc_input_command_down("crouch")) {
		//slog("FORWARD PRESSED");
		movement.z -= move;
	}

	/*
	//yeee SCALING was the problem.  Because we initialize direction to 0 !!  And so I'm scaling 0 :/
	gfc_vector3d_scale(dir, dir, movement.y); //because we wanna move in that direction teehee
	gfc_vector3d_add(self->position, self->position, dir);
	gfc_vector3d_add(data->target, data->target, dir);

	
	//This block made me LOOK in the direction I moved.  which wasn't quite what I needed
	//gfc_vector3d_add(self->position, self->position, movement);
	//gfc_vector3d_add(data->target, self->position, movement);

	gfc_vector3d_add(dir, dir, movement);
	gfc_vector3d_normalize(&dir);
		//gfc_vector3d_rotate_about_z(&dir, GFC_HALF_PI); //This rotates the XY axes by 90.  I don't want this :thonk: 
	//Update movement:  (AND the direction I'm looking: target)
	gfc_vector3d_add(self->position, self->position, dir);
	gfc_vector3d_add(data->target, data->target, dir);
	//Where my data->target is THE DIRECTION I'M LOOKING.essentially.   Hence why it's a unit vetor!

	//Lookies:
	if (gfc_input_command_down("panleft")) {
		yaw -= turn;
	}

	if (gfc_input_command_down("panright")) {
		yaw = turn;
	}
	if (gfc_input_command_down("panup")) {
		pitch += turn;
	}
	if (gfc_input_command_down("pandown")) {
		pitch -= turn;
	}

	if (pitch) {
		//I want my direction to go up
		data->target.z += pitch;
	}

	if (yaw)
	{
		gfc_vector3d_sub(dir, data->target, self->position);
		slog("dir pre: %f,%f,%f", data->target.x, data->target.y, data->target.z);
		gfc_vector3d_rotate_about_z(&dir, yaw);  //I think it's becaues this function rotates about an angle starting from +x axis... and my yaw isn't guaranteed to be that
		gfc_vector3d_add(data->target, self->position, dir);
		slog("dir pos: %f,%f,%f", data->target.x, data->target.y, data->target.z);
	}

	//if( /gfc_vector3d_compare(data->target, data->target)/ movement.y != 0 || movement.x != 0)
	gf3d_camera_look_at(data->target, &self->position);

	//data->position.z = data->target->position.z

}

Entity* camera_entity_spawn(GFC_Vector3D position, GFC_Vector3D target) {
	GFC_Vector3D dir = { 0 };
	CameraEntityData* data;
	Entity* self;
	self = entity_new();
	if (!self) return NULL;

	data = gfc_allocate_array(sizeof(CameraEntityData), 1);
	if (!data) {
		entity_free(self);
		slog("Cannot allocate Camera Entity Data. Not spawning");
		return NULL;
	}
	self->think = camera_entity_think;
	self->free = camera_entity_free;
	self->position = position;
	slog("My Camera Entity' Y position is: %f",position.y);

	gfc_vector3d_sub(dir, target, position);
	gfc_vector3d_normalize(&dir);
	gfc_vector3d_add(target, position, dir);
	//Look in that direction:
	gf3d_camera_look_at(target, &self->position);

	//self->position = position;
	data->target = target;
	data->angle = GFC_PI;
	data->followDistance = 10;
	data->followHeight = 5;
	self->data = data;

	return self;
}
*/

void camera_entity_think(Entity* self) {
	GFC_Vector3D dir = { 0 };
	GFC_Vector3D offset = { 0 };
	float look = 1;
	float turn = 0.02;		//The rate at which I turn

	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;

	/*
	* Moving this code to update
	* 
	offset = gfc_vector3d(0, 1, 0); //initialize to be in the +y direction (will not matter. Because we're rotating it all about the xy plane anyway)

	gfc_vector3d_rotate_about_z(&offset, data->angle);
	gfc_vector3d_scale(offset, offset, data->followDistance); //Scale the offset so it's followDistance AWAY from my entity
	offset.z = data->followHeight;
	gfc_vector3d_add(self->position, offset, data->target->position);
	*/
	//Lookies:
	if (gfc_input_command_down("panleft")) {
		data->angle -= turn;
	}

	if (gfc_input_command_down("panright")) {
		data->angle += turn;
	}
	//Up and down is inherently different; we don't wanna change our angle here.
	if (gfc_input_command_down("panup")) {
		data->followHeight += look;
	}
	if (gfc_input_command_down("pandown")) {
		data->followHeight -= look;
	}

	//gf3d_camera_look_at(data->target->position, &self->position);

}

// Actually follow the player (UPDATE my values) based on the values we set in think.
void camera_entity_update(Entity* self) {
	GFC_Vector3D offset = { 0 };
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;

	offset = gfc_vector3d(0, 1, 0); //initialize to be in the +y direction (will not matter. Because we're rotating it all about the xy plane anyway)

	gfc_vector3d_rotate_about_z(&offset, data->angle);
	gfc_vector3d_scale(offset, offset, data->followDistance); //Scale the offset so it's followDistance AWAY from my entity
	offset.z = data->followHeight;
	gfc_vector3d_add(self->position, offset, data->target->position);

	gf3d_camera_look_at(data->target->position, &self->position); //nearly forgot this line when I moved the offset code from Think into Update lol
}

Entity* camera_entity_spawn(Entity *target) {
	GFC_Vector3D dir = { 0 };
	CameraEntityData* data;
	Entity* self;
	self = entity_new();
	if (!self) return NULL;

	data = gfc_allocate_array(sizeof(CameraEntityData), 1);
	if (!data) {
		entity_free(self);
		slog("Cannot allocate Camera Entity Data. Not spawning");
		return NULL;
	}
	self->think = camera_entity_think;
	self->update = camera_entity_update;
	self->free = camera_entity_free;
	self->position = gfc_vector3d(0,50,50);	
	slog("cam position %i, %i, %i", self->position.x, self->position.y, self->position.z);
	//Set all the data values
	data->target = target;
	data->angle = 0;
	data->followDistance = 50;
	data->followHeight = 15;
	//set MY data  to be the CameraEntityData pointer
	self->data = data;
	self->position.y = data->target->position.y + data->followDistance;
	self->position.z = data->followHeight;
	//slog("My Camera Entity's Y position is: %f\nWhile my target, %s, Y position is: %f", self->position.y, target->name, data->target->position.y);

	//Look in that direction:
	gf3d_camera_look_at(target->position, &self->position);

	return self;
}

// Because I don't wanna FUCK UP my current spawn function or anything
void camera_ent_add_on_flying(Entity *self) {

}

void camera_entity_think_fly(Entity* self) {

}

void camera_entity_update_fly(Entity *self) {

}

//eof
