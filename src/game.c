#include <SDL.h>    //Precompiled        

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"

#include "gf3d_mesh.h"
#include "entity.h"
#include "monster.h"
#include "player.h"
#include "camera_entity.h"
#include "ground.h"

#include "gf3d_camera.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}


int main(int argc,char *argv[])
{
    //local variables
    Sprite *bg;
    Mesh* mesh;
    Texture* texture;
    Entity* monster;
    Entity* player;
    Entity* ground;
    float theta = 0;
    GFC_Vector3D lightPos = { 5,5,20 };
    Entity* cam;
    GFC_Matrix4 id, dinoM;
    const Uint8* keys;
    GFC_Matrix4 modelMat;
    //initializtion    

    parse_arguments(argc,argv);
    init_logger("gf3d.log",0); //1 wont delete log file at end
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);

    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    entity_system_init(8000);
    //game init
    srand(SDL_GetTicks());
    slog_sync();
    bg = gf2d_sprite_load_image("images/bg_flat.png");
    gf2d_mouse_load("actors/mouse.actor");

    // main game loop    

    gfc_matrix4_identity(modelMat);
    gfc_matrix4_identity(id);
    //gf3d_camera_look_at(gfc_vector3d(0, 0, 0), &cam);
    mesh = gf3d_mesh_load_obj("models/sky/sky.obj");
    texture = gf3d_texture_load("models/sky/sky.png");
    monster = monster_spawn(gfc_vector3d(-10, 0, 0), GFC_COLOR_WHITE);
    player = player_spawn(gfc_vector3d(0, 0, 0), GFC_COLOR_WHITE);
    ground = ground_spawn(gfc_vector3d(0, 0, 0), GFC_COLOR_WHITE);
    cam = camera_entity_spawn(&player);
    slog("cam position %i, %i, %i", cam->position.x, cam->position.y, cam->position.z);
    while(!_done)
    {
        keys = SDL_GetKeyboardState(NULL);
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        theta += .1;
        gfc_matrix4_rotate_z(dinoM, id, theta);
        entity_system_think_all();
        entity_system_update_all();
        camera_entity_update2(cam, player);
        entity_system_move_all();
        //camera updates
        gf3d_vgraphics_render_start();
                //3D draws
                gf3d_mesh_sky_draw(mesh, modelMat, GFC_COLOR_WHITE, texture);
                entity_system_draw_all(lightPos, GFC_COLOR_RED); //Change id to dinoM
                //2D draws
               // gf2d_sprite_draw_image(bg,gfc_vector2d(0,0));
                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                if ((player->velocity.x != 0) || (player->velocity.y != 0)) {
                    gf2d_font_draw_line_tag("Shmovement :)", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 40));
                }
                else {
                    gf2d_font_draw_line_tag("No Shmovement :(", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 40));
                }
                if (((player->position.x >= -12) && (player->position.x <= -8)) && ((player->position.y >= -2) && (player->position.y <= 2))) {
                    gf2d_font_draw_line_tag("Contact", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 70));
                }
                gf2d_mouse_draw();
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
