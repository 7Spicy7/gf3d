#ifndef __CAMERA_ENTITY_H__
#define __CAMERA_ENTITY_H__

#include "entity.h"

/**
 * @brief Spawn a moveable camera entity
 * @param position - the position at which to spawn the camera
 * @param target - the DIRECTION in which the camera should look.  The value of this parameter is often normalized within the class's methods
 * @note I may also want to make target an Entity, considering my Camera's Perspective is directly behind the player entity.
 */
// Where target is the *direction* I'm looking at.  Because I normalize it every time I work with it
//Entity* camera_entity_spawn(GFC_Vector3D position, GFC_Vector3D target);

/**
 * @brief Spawn a camera entity that follows the player
 * @param target - the entity I want to follow
 * @note I may also want to make target an Entity, considering my Camera's Perspective is directly behind the player entity.
 */
Entity* camera_entity_spawn(Entity* target);

void camera_entity_update(Entity* self);

#endif