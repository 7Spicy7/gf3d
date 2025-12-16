#include "simple_logger.h"
#include "gfc_vector.h"

#include "player.h"

#include "gfc_input.h"

Entity* player_spawn(GFC_Vector3D position, GFC_Color color)
{
	Entity* self;
	self = entity_new();
	if (!self) return NULL;
	gfc_line_cpy(self->name, "notAgumon");
	self->mesh = gf3d_mesh_load_obj("models/carts/babytank.obj");
	self->texture = gf3d_texture_load("models/carts/babytank.png");
	self->color = color;
	self->position = position;
	self->think = player_think;
	self->update = player_update;
	self->rotation.y = 1.57;
	self->direction = 0;
	self->velocity.x = 0;
	self->turn = 0.1;
	self->maxturn = 1.57;
	self->topspeed = 0.6;
	self->facing = gfc_vector3d(0,1,0);
	//self->velocity.z = gfc_crandom();
	return self;
}

void player_think(Entity* self)
{
	if (!self) return;
	player_move(self);

}

void player_move(Entity* self)
{
	if (!self) return;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	//Movement Up/Down
	if (gfc_input_command_down("walkforward"))
	{
		/**
		if (self->position.y - (0.1*self->facing.y) <= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.y = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else if (self->position.y + (0.1 * self->facing.y) >= 100)
		{
			slog("Pressed w, can't go further");
			self->velocity.y = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		if (self->position.x - (0.1 * self->facing.x) <= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.x = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else if (self->position.x + (0.1 * self->facing.x) >= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.x = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else {
		**/
			//slog("Pressed w");
			self->velocity.y = -(self->topspeed * self->facing.y);
			self->velocity.x = -(self->topspeed * self->facing.x);
		//}
		
	}
	else if (gfc_input_command_down("walkback"))
	{
		/**
		if (self->position.y - (0.1 * self->facing.y) <= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.y = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else if (self->position.y + (0.1 * self->facing.y) >= 100)
		{
			slog("Pressed w, can't go further");
			self->velocity.y = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else if (self->position.x - (0.1 * self->facing.x) <= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.x = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else if (self->position.x + (0.1 * self->facing.x) >= -100)
		{
			slog("Pressed w, can't go further");
			self->velocity.x = 0;
			self->rotation.z = 0;
			self->direction = 0;
		}
		else {
		**/
			//slog("Pressed s");
			self->velocity.y = (self->topspeed * self->facing.y);
			self->velocity.x = (self->topspeed * self->facing.x);
		//}
	}
	else {
		self->velocity.x = 0;
		self->velocity.y = 0;
	}
	//Movement left/right
	if (gfc_input_command_down("walkleft"))
	{
		if (self->position.x + 0.1 >= 100)
		{
			//slog("Pressed a, can't go further");
			//self->velocity.x = 0;
			self->rotation.z += 0.1;
			self->direction += 0.1;
		}
		else {
			//slog("Pressed a");
			//self->velocity.x = 0.1;
			self->rotation.z += 0.1;
			self->direction += 0.1;
			gfc_vector3d_rotate_about_z(&self->facing, self->turn);
			gfc_vector3d_magnitude(self->facing);
		}
	}
	else if (gfc_input_command_down("walkright"))
	{
		if (self->position.x - 0.1 <= -100)
		{
			//slog("Pressed d, can't go further");
			//self->velocity.x = 0;
			self->rotation.z -= 0.1;
			self->direction -= 0.1;
		}
		else {
			//slog("Pressed d");
			//self->velocity.x = -0.1;
			self->rotation.z -= 0.1;
			self->direction -= 0.1;
			gfc_vector3d_rotate_about_z(&self->facing, -(self->turn));
			gfc_vector3d_magnitude(self->facing);
		}
	}
	
	//Move Camera/Model
	if (gfc_input_command_down("rotateright"))
	{
		//slog("Pressed r");
		self->rotation.z = self->rotation.z + 0.1;
		self->direction = self->direction + 0.1;
	}
	else if (gfc_input_command_down("rotateleft"))
	{
		//slog("Pressed e");
		self->rotation.z = self->rotation.z - 0.1;
		self->direction = self->direction - 0.1;
	}

	if (gfc_input_command_down("1"))
	{
		self->topspeed = 0.6;
		self->mesh = gf3d_mesh_load_obj("models/carts/babytank.obj");
		self->texture = gf3d_texture_load("models/carts/babytank.png");
	}
	if (gfc_input_command_down("2"))
	{
		self->topspeed = 1.2;
		self->mesh = gf3d_mesh_load_obj("models/carts/draggy.obj");
		self->texture = gf3d_texture_load("models/carts/draggy.png");
	}
	if (gfc_input_command_down("3"))
	{
		self->topspeed = 0.8;
		self->mesh = gf3d_mesh_load_obj("models/carts/patchy.obj");
		self->texture = gf3d_texture_load("models/carts/patchy.png");
	}
	if (gfc_input_command_down("4"))
	{
		self->topspeed = 1;
		self->mesh = gf3d_mesh_load_obj("models/carts/uro.obj");
		self->texture = gf3d_texture_load("models/carts/uro.png");
	}
	if (gfc_input_command_down("5"))
	{
		self->topspeed = 0.4;
		self->mesh = gf3d_mesh_load_obj("models/carts/topspin.obj");
		self->texture = gf3d_texture_load("models/carts/topspin.png");
	}
}

void player_update(Entity *self) {
	if (!self) return;
	self->position.x = self->position.x + self->velocity.x;
	self->position.y = self->position.y + self->velocity.y;
}