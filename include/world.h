#ifndef __WORLD_H__
#define __WORLD_H__

#include "gfc_vector.h"
#include "gfc_color.h"

#include "gf3d_mesh.h"

typedef struct {
	Mesh				*terrain;
	Texture				*texture;
	GFC_Vector3D		position;

	GFC_Color			lightColor;
	GFC_Vector3D		lightPos;
	//down the road:
	GFC_List			*entity_list; /**<List of entities to be contained in our world*/

}World;


World* world_new();


World* world_load(const char* filename);

void world_free(World* world);

World* get_world();

void set_world(World* worldToSet);


void world_draw(World* world);


Uint8 world_edge_test(World* world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D* contact);

#endif
