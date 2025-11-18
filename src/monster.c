#include "simple_logger.h"

#include "entity.h"
#include "gf3d_mesh.h"
#include "gfc_vector.h"

Entity* monster_spawn(GFC_Vector3D position, GFC_Color color) {
	Entity* self;
	self = entity_new();
	if (!self) return NULL;
	gfc_line_cpy(self->name, "punchy");
	self->mesh = gf3d_mesh_load_obj("models/characters/punchy/punchy.obj");
	self->texture = gf3d_texture_load("models/characters/punchy/punchy.png");
	self->color = color;
	self->rotation.y = 90;
	self->position = position;
	//self->velocity.z = gfc_crandom();
	return self;
}

void monster_think(Entity* self) {

}

void monster_move(Entity* self) {

}