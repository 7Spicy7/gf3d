#include "simple_logger.h"

#include "entity.h"
#include "gf3d_mesh.h"
#include "gfc_vector.h"

Entity* ground_spawn(GFC_Vector3D position, GFC_Color color) {
	Entity* self;
	self = entity_new();
	if (!self) return NULL;
	gfc_line_cpy(self->name, "ground");
	self->mesh = gf3d_mesh_load_obj("models/ground/ground.obj");
	self->texture = gf3d_texture_load("models/ground/groundtexture.png");
	self->color = color;
	self->position = position;
	return self;
}
