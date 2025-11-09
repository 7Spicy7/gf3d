#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_text.h"
#include "gfc_matrix.h"

#include "gf3d_mesh.h"
#include "world.h"
//Include vector, maatrix, 



typedef struct Entity_S
{
	Uint8							_inuse;
	GFC_TextLine					name;
	Mesh*							mesh;
	Texture*						texture;
	GFC_Color						color;
	GFC_Matrix4						matrix;
	GFC_Vector3D					position;
	GFC_Vector3D					drawOffset;
	GFC_Vector3D					rotation;
	GFC_Vector3D					scale;
	GFC_Vector3D					velocity;
	GFC_Vector3D					acceleration;
	Uint8							drawShadow;


	//GFC_Primitive					collision; 	//Primitives are shapes in 3D; this is NOT the same as a mesh primitive
	GFC_Box							bounds;
	void							(*draw)(struct Entity_S *self);
	void							(*think)(struct Entity_S* self);
	void							(*update)(struct Entity_S* self);
	void							(*move)(struct Entity_S* self);
	void							(*free)(struct Entity_S* self);
	Uint8							doGenericUpdate;
	Uint8							canJump;
	Uint8							onGround;
	Uint8							justSpawned; //A check for first spawn to snap to ground
	void*							data;
}Entity;


/*
* @brief get a pointer to a new blank entity
* @return NULL on out of memory or other error, a pointer to a blank entity otherwise
*/
Entity* entity_new();

/*
* @brief free a previously new'd entity
* @param ent tje entioty to be freed
* @note the memory address should no longer be used
*/
void entity_free(Entity* ent);

/*
@brief Initilizes the enttiy subsystem
@param max_ents jpw ,amu tp sipport concurrently
*/
void entity_system_init(Uint32 max_ents);

/*
* @brief draw all entittes with the provided light
* @param lightPos where the loght is in world space
* @
*/
void entity_system_draw_all(GFC_Vector3D lightPos, GFC_Color lightColor);


void entity_system_think_all();

void entity_system_update_all();

void entity_think(Entity* ent);

void entity_update(Entity* ent);

void entity_system_move_all();

void entity_move();

Uint8 entity_get_floor_pos(Entity* ent, World* world, GFC_Vector3D* contact);

void entity_draw_shadow(Entity* ent);


#endif