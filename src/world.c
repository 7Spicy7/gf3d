#include "simple_logger.h"
#include "simple_json.h"
#include "gfc_config.h" //for the helper json functions just as get_color_from_object

#include "gfc_primitives.h" // for collission,, not sure if this is already included in smth else but oh well

#include "gf3d_obj_load.h" //for the Object Data

#include "gf3d_mesh.h"
#include "entity.h" //for entitiy draw
#include "world.h"
// #include "space.h"

extern int __DEBUG;
static World* theWorld;


void world_free(World* world) {
	if (!world) return;
	gf3d_mesh_free(world->terrain);

	if (world->texture)	gf3d_texture_free(world->texture);

	if (world->entity_list) gfc_list_delete(world->entity_list);

	free(world);
}

World* world_new() {
	World* world;
	world = gfc_allocate_array(sizeof(World), 1);
	if (!world) {
		slog("World could not be allocated");
		return NULL;
	}

	//Do all the initialization stuff
	return world;
}

//From point A to point B,  I wanna find out if this line interesects the world.  AND at what point it does.
 //This works :D  yay
Uint8 world_edge_test(World* world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact) {
	if (!world) return NULL;
	int i, j, c, d;		//d = face count
	int result = -1;
	GFC_Edge3D e;
	GFC_Triangle3D t;
	MeshPrimitive* prim;
	GFC_Matrix4 identity_offset;
	gfc_matrix4_identity(identity_offset);

	//algorithm is pretty simple.. it's gonna be Brute force and nasty

	//Basically.  I want to see if this edge makes contact with ANY VERTEX.  on my terrain,,,,,
	e = gfc_edge3d_from_vectors(start, end);

	c = gfc_list_count(world->terrain->primitives);
	//for every primitive:
	for (i = 0; i < c; i++) {

		// SO .  When I call gfc_triangle_edge_test(Edge e, Triangle t)..  The TRIANGLE I am testing against is going to be my Face -- made of my 3 Verteces.
		//which means I literally have to grab the primitive
		prim = gfc_list_get_nth(world->terrain->primitives, i);
		if ((!prim) || (!prim->objData) ) continue;
		
		result = gf3d_obj_edge_test(prim->objData, identity_offset, e, contact);
		if (result == 1) {
			//slog("My edge is\n1st vector: x: %f,  y: %f,  z: %f\n2nd vector: x: %f,  y: %f,  z: %f", e.a.x, e.a.y, e.a.z, e.b.x, e.b.y, e.b.z);
			slog("Colliding. At a contact point with x: %f,  y: %f,  z: %f", contact->x, contact->y, contact->z);
			return result;
		}
		
		/*
		//and CONSTRUCT a triangle  for each and every individual "pixel" -- Face made of 3 verticies (vertices = points in space).
		//set t
		d = prim->objData->face_count;
		//for every Face
		for (j = 0; j < d; j++) {

			//Where faceVertices[i]  I believe is the i'th Face.   and .verts[i] ...

			//Armen and Franklin explained it to me. one of them is a list of INDEXES that are used to index into the array (stored elsewhere) where the Verteces are found.
			t.a = prim->objData->faceVertices[j].vertex;


			if ( gfc_triangle_edge_test(e, t, contact) ) {
				slog("Colliding");
			}
		}*/
		
		// see gf3d_obj_edge_test()   in gf3d_obj_load.c;

		//holy..
	

	}

	return 0; //this is just so that the compiler doesn't yell at me while I test my skybox
}


World* world_load(const char* filename) {
	World* world;
//	Space* space;
	MeshPrimitive* prim;
	GFC_Vector3D vec = { 0 };
	SJson* json, *config;
	//For config:
	const char* str;

	//For entity list:
	int i, entCount;
	SJson* entList, * enty;
	const char* ent_name = NULL;
	Entity* ent = NULL;
	Entity* player = NULL;
	GFC_Vector2D temp;
	GFC_Vector3D default_pos = { 0 };

	slog_sync();
	json = sj_load(filename); //name of the def file :) ofc

	if (!json) {
		slog("JSon was no able to be loaded");
		return NULL;
	}
	world = world_new();
	if (!world) {
		slog("Could not create new world. Not loading");
		sj_free(json);	//BE SURE TO FREE THE JSON !  
		
		return NULL;
	}
	
	config = sj_object_get_value(json, "world");  //to ensure that the Json file I'm parsing IS the world object,, as I titled it in the terrain.def file
	if (!config) {
		slog("Failed to find 'world' object in the given def file. Not parsing");
		sj_free(json);
		world_free(world);
		return NULL;
	}

	//Now parse it :)
	str = sj_object_get_string(config, "terrainMesh");
	
	world->terrain = gf3d_mesh_load_obj(str);
	if (!world->terrain) {
		slog("World terrain was NOT PROPERLY LOADED");
	}
	prim = gfc_list_get_nth(world->terrain->primitives, 0);
	//slog("The object data's face_vert count is: %i", prim->objData->face_vert_count);

	world->texture = NULL;
	//world->texture = gf3d_texture_load( sj_object_get_string(config, "terrainTexture"));

	sj_object_get_vector3d(config, "worldPosition", &vec);
	world->position = vec;
	/*if (__DEBUG) */slog("World position:\t x: %f, y: %f, z: %f", vec.x, vec.y, vec.z);





	world->lightColor = GFC_COLOR_WHITE;
	
	sj_object_get_color_value( config, "lightColor", &world->lightColor);
	//if(__DEBUG) slog("The alpha of my lightColor should be: %f", world->lightColor.a);

	if (__DEBUG) slog("World with obj file name: %s should have been loaded", str);
	sj_free(json);  //Free the json once everything's loaded


	// Copied from 2D:   make sure it's good

	// ==================   ENTITY LIST
	world->entity_list = gfc_list_new();
	entList = sj_object_get_value(json, "entity_List"); //This is now a list/array of N objects
	entCount = sj_array_get_count(entList);
	//slog("The number of entities that belong to this World's list is: %i",entCount);	//Jlog

	/*for (i = 0; i < entCount; i++) {	//! Also what I want to do is I want to be able to use this list of entities to spawn them in.  So call spawn as well
		enty = sj_array_get_nth(entList, i); //enty is now the JSon object that contains all the info about the ent.   That being said !!  The only info my WORLD needs to know about it   is it's name
		//SO THAT:  I can call  spawn_entity("player", default_pos); :   meaning I want a name and its position OVERRIDE  if there is one..

		if (enty) {
			ent_name = sj_object_get_string(enty, "name");    //Get the name of the entity/JSon object
			sj_object_get_vector2d(enty, "position", &temp); //if the position argument does not exist, it doesn't change the values of default_pos at all
			default_pos.x = temp.x;
			default_pos.y = temp.y;
			default_pos.z = 0;
			//Spawn the entity using its name in the Spawn list & its position in the World def file

			//hmmmm  maybe I SHOULD make a list.  bit a List of 3D vectors.  so that I can also save the value AT that tile.  So I can make
				//5 = Dragon spawn points.   6  = item spawn points.    7 =  Cave spawn points.  8 = NPCs. Saving those values as the 3d vector's .z value

			//if (spawnTilePosition && spawnTilePosition->x > 0 && spawnTilePosition->z != 5) {
			//	default_pos.x = spawnTilePosition->x;
			//	default_pos.y = spawnTilePosition->y;
			//	default_pos.z = 1;
				//Jlog   is default_pos is being overriden by any spawn tile Value
				//slog("Spawning entity at the position indicated BY the Map def file's tileSet, x position: %f",spawnTilePosition->x);
			//}
			//Spawn in the entity.  THIS adds them into the Entity Manager
			//ent = spawn_entity(ent_name, default_pos, spawn_coords);
			if (ent) {
				//slog("Spawning and appending entity %s to the list. With position x at: %f", ent->name,default_pos.x); //Jlog
				gfc_list_append(world->entity_list, ent); //yeah,  I nearly appended just the names, but I suppose it's easier to appent the Entity object itself
			}
			/*lse { slog("ohhhh we didn't spawn the entity... Name retrieved by the SJon object is: %s", ent_name); }/
		}

		//else { slog("Enty index #%i not found. Not spawning", i); }
	}*/

	// =============
	// Configuring Objects and whatnot
	/*
	space = space_new();
	if (space) {
		space_set_iterations(space, 5);
		gfc_list_append(space->StaticMeshes, world->terrain);
			

		//For all the entities:
		space_add_body(Space *space, Body *body);

		//For any static entities:

		
	}
	*/


	theWorld = world;

	return world;
}

World* get_world() {
	return theWorld;
}


void set_world(World *worldToSet) {
	theWorld = worldToSet;
}


void world_draw(World* world) {
	GFC_Matrix4 modelMat;
	//GFC_Vector3D translation = gfc_vector3d(0,0,-5);
	if (!world) return;
	gfc_matrix4_identity(modelMat);

	/*gfc_matrix4_make_translation(translation, gfc_vector3d(0,0,0)); //I was trying to make translation another MATRIX lol,,, idk how that works
	gfc_matrix4_multiply(modelMat, modelMat, translation);*/  //oh.  wait. Nono, this works !  I just multiplied the matrices wrong :)  whoopsies!
	//gfc_matrix4_translate(modelMat, modelMat, world->position);

	//I don't WANT to change the position I draw it :| because the Obj data does not match up for the sake of collissions

	// .. wait.  I AM DRAWING IT  at a different position !  This line above me works!  BUT THE OBJECT DATA of the mesh itself is not yet updated
			//  :|  no that makes no sense.  I use modelMat as a parameter to COPY it INTO the ubo's data?? what the FUCK


	// hrm .  Need the MESH to be drawn somewhere else .  which means I need to translate the MODEL matrix
	gf3d_mesh_draw(world->terrain, modelMat, GFC_COLOR_WHITE, world->texture, world->lightPos, world->lightColor);

	//Draw all entities here

}