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

void camera_entity_think(Entity* self) {
	float look = 1;
	float turn = 0.02;		//The rate at which I turn

	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
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
}

// Actually follow the player (UPDATE my values) based on the values we set in think.
void camera_entity_update(Entity* self) {
	//GFC_Vector3D offset = { 0 };
	//CameraEntityData* data;
	//if ((!self) || (!self->data)) return;
	//data = self->data;
	//offset = gfc_vector3d(0, 1, 0); //initialize to be in the +y direction (will not matter. Because we're rotating it all about the xy plane anyway)

	//gfc_vector3d_rotate_about_z(&offset, data->angle);
	//gfc_vector3d_scale(offset, offset, data->followDistance); //Scale the offset so it's followDistance AWAY from my entity
	//offset.z = data->followHeight;
	//gfc_vector3d_add(self->position, offset, data->target->position);

	//gf3d_camera_look_at(data->target->position, &self->position); //nearly forgot this line when I moved the offset code from Think into Update lol
	//gf3d_camera_update_view();
	//slog("offset is %i, %i, %i", offset.x, offset.y, offset.z);
	//("distance is %i", data->followDistance);
}

void camera_entity_update2(Entity* self, Entity* target) {
	GFC_Vector3D offset = { 0 };
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	offset = gfc_vector3d(0, 1, 0); //initialize to be in the +y direction (will not matter. Because we're rotating it all about the xy plane anyway)

	gfc_vector3d_rotate_about_z(&offset, data->angle);
	gfc_vector3d_scale(offset, offset, data->followDistance); //Scale the offset so it's followDistance AWAY from my entity
	offset.z = data->followHeight;
	gfc_vector3d_add(self->position, offset, data->target->position);

	self->position.x = target->position.x;
	self->position.y = target->position.y + 50;

	gf3d_camera_look_at(target->position, &self->position); //nearly forgot this line when I moved the offset code from Think into Update lol
	gf3d_camera_update_view();
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
	gfc_line_cpy(self->name, "camera");
	//Set all the data values
	data->target = target;
	data->angle = 0;
	data->followDistance = 50;
	data->followHeight = 15;
	//set MY data  to be the CameraEntityData pointer
	self->targetPos = &target->position;
	self->data = data;
	self->position.y = self->targetPos->y + data->followDistance;
	self->position.z = data->followHeight;
	//slog("My Camera Entity's Y position is: %f\nWhile my target, %s, Y position is: %f", self->position.y, target->name, data->target->position.y);


	//Look in that direction:
	gf3d_camera_look_at(target->position, &self->position);
	gf3d_camera_update_view();

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
