#include "simple_logger.h"

#include "player.h"

#include "gfc_input.h"

Entity* player_spawn(GFC_Vector3D position, GFC_Color color)
{
	Entity* self;
	self = entity_new();
	if (!self) return NULL;
	gfc_line_cpy(self->name, "notAugmon");
	self->mesh = gf3d_mesh_load_obj("models/weapons/amp2.obj");
	self->texture = gf3d_texture_load("models/weapons/amskin.png");
	self->color = color;
	self->position = position;
	self->think = player_think;
	self->rotation.y = 90;
	self->velocity.x = 0;
	//self->velocity.z = gfc_crandom();
	slog("Made it here");
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
		slog("Pressed w");
		self->velocity.x = 3;
	}
	else
	{
		self->velocity.x = 0;
	}
	if (gfc_input_command_down("walkforward"))
	{
		slog("Pressed w");
		self->velocity.y = -3;
	}
	else if (gfc_input_command_down("walkback"))
	{
		slog("Pressed s");
		self->velocity.y = 3;
	}
	else
	{
		self->velocity.y = 0;
	}
	//Movement left/right
	if (gfc_input_command_down("walkleft"))
	{
		slog("Pressed a");
		self->velocity.x = 3;
	}
	else if (gfc_input_command_down("walkright"))
	{
		slog("Pressed d");
		self->velocity.x = -3;
	}
	else
	{
		self->velocity.x = 0;
	}
	
	//Move Camera/Model
	if (gfc_input_command_down("rotateright"))
	{
		slog("Pressed r");
		self->rotation.z = self->rotation.z + 1;
	}
	else if (gfc_input_command_down("rotateleft"))
	{
		slog("Pressed e");
		self->rotation.z = self->rotation.z - 1;
	}
	else
	{
		//self->rotation.z = 0;
	}

}