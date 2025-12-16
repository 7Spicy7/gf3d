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
    int startgame;
    int pausetimer;
    int checkpoint;
    int echeckpoint;
    int lap;
    int elap;
    Sprite *bg;
    Mesh* mesh;
    Texture* texture;
    Entity* monster;
    Entity* player;
    Entity* ground;
    float theta = 0;
    GFC_Vector3D lightPos = { 5,5,20 };
    GFC_Vector3D pointto = { 1000,-1000,100 };
    Entity* cam;
    GFC_Matrix4 id, dinoM;
    const Uint8* keys;
    GFC_Matrix4 modelMat;
    GFC_Sound *bgm;
    GFC_Sound* bgma;
    GFC_Vector2D monsterxy;
    //initializtion    

    parse_arguments(argc,argv);
    init_logger("gf3d.log",0); //1 wont delete log file at end
    //gfc init
    gfc_input_init("config/input.cfg");
    SDL_Init(SDL_INIT_AUDIO);
    gfc_audio_init(128, true, true);
    gfc_sound_init_config("config/audio.cfg");
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
    monster = monster_spawn(gfc_vector3d(69, 0, 0), GFC_COLOR_WHITE);
    monsterxy = gfc_vector2d(monster->position.x, monster->position.y);
    player = player_spawn(gfc_vector3d(65, 0, 0), GFC_COLOR_WHITE);
    ground = ground_spawn(gfc_vector3d(0, 0, 0), GFC_COLOR_WHITE);
    cam = camera_entity_spawn(&player);
    bgm = gfc_sound_load("music/KARcitytrialcity.mp3", 1, 1);
    bgma = bgm;
    checkpoint = 0, echeckpoint = 0, lap = 0, elap = 0, startgame = 0, pausetimer = 0;
    Mix_PlayChannel(1, bgma, 0);
    gfc_sound_play_to_group(bgma, -1, 0.5, "world");
    slog(Mix_GetError);
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
            if (startgame == 1) {
                entity_system_update_all();
                camera_entity_update2(cam, player);
                entity_system_move_all();
            }
            if (startgame == 0) {
                camera_entity_update2(cam, &pointto);
            }
            //camera updates
            gf3d_vgraphics_render_start();
            //3D draws
            gf3d_mesh_sky_draw(mesh, modelMat, GFC_COLOR_WHITE, texture);
            entity_system_draw_all(lightPos, GFC_COLOR_RED); //Change id to dinoM
            //2D draws
           // gf2d_sprite_draw_image(bg,gfc_vector2d(0,0));
            if (startgame == 1) {
                gf2d_font_draw_line_tag("P to pause", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 10));
                    if ((player->velocity.x != 0) || (player->velocity.y != 0)) {
                        gf2d_font_draw_line_tag("Shmovement :)", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 40));
                    }
                    else {
                        gf2d_font_draw_line_tag("No Shmovement :(", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 40));
                    }
                    if (gfc_input_command_down("pause")) {
                        if (pausetimer == 0) 
                        { 
                        startgame = 0;
                        pausetimer = 30;
                        }
                    }
            }
            if (startgame == 0) {
                gf2d_font_draw_line_tag("P to start game", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 10));
                gf2d_font_draw_line_tag("ALT+F4 to exit", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 40));
                if (gfc_input_command_down("exit"))_done = 1; // exit condition
                if (gfc_input_command_down("pause")) {
                    if (pausetimer == 0)
                    {
                        startgame = 1;
                        pausetimer = 30;
                    }
                }
            }
            if (((player->position.x >= -12) && (player->position.x <= -8)) && ((player->position.y >= -2) && (player->position.y <= 2))) {
                gf2d_font_draw_line_tag("Contact", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 70));
            }
            //checkpoints
            if (((player->position.x >= 60) && (player->position.x <= 70)) && ((player->position.y <= -80) && (player->position.y >= -90))) {
                if (checkpoint == 0) {
                    checkpoint = 1;
                    slog("checkpoint 1!");
                }
            }
            if (((player->position.x >= -45) && (player->position.x <= -35)) && ((player->position.y <= -80) && (player->position.y >= -90))) {
                if (checkpoint == 1) {
                    checkpoint = 2;
                    slog("checkpoint 2!");
                }
            }
            if (((player->position.x >= -40) && (player->position.x <= -30)) && ((player->position.y >= 65) && (player->position.y <= 75))) {

                if (checkpoint == 2) {
                    checkpoint = 3;
                    slog("checkpoint 3!");
                }
            }
            if ((player->position.x >= 60) && (player->position.y >= 65)) {
                if (checkpoint == 3) {
                    checkpoint = 4;
                    slog("checkpoint 4!");
                }
            }
            if (((player->position.x >= 60) && (player->position.x <= 70)) && ((player->position.y <= 10) && (player->position.y >= -10))) {
                if (checkpoint == 4) {
                    slog("lap!");
                    checkpoint = 0;
                    lap += 1;
                    slog("laps total: %i", lap);
                }
            }
            if (((player->position.x >= -12) && (player->position.x <= -8)) && ((player->position.y >= -2) && (player->position.y <= 2))) {
                gf2d_font_draw_line_tag("Contact", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 70));
            }
            //enemy checkpoints
            if (((monster->position.x >= 60) && (monster->position.x <= 70)) && ((monster->position.y <= -80) && (monster->position.y >= -90))) {
                if (echeckpoint == 0) {
                    echeckpoint = 1;
                    slog("enemy checkpoint 1!");
                }
            }
            if (((monster->position.x >= -45) && (monster->position.x <= -35)) && ((monster->position.y <= -80) && (monster->position.y >= -90))) {
                if (echeckpoint == 1) {
                    echeckpoint = 2;
                    slog("enemy checkpoint 2!");
                }
            }
            if (((monster->position.x >= -40) && (monster->position.x <= -30)) && ((monster->position.y >= 65) && (monster->position.y <= 75))) {

                if (echeckpoint == 2) {
                    echeckpoint = 3;
                    slog("enemy checkpoint 3!");
                }
            }
            if ((monster->position.x >= 60) && (monster->position.y >= 65)) {
                if (echeckpoint == 3) {
                    echeckpoint = 4;
                    slog("enemy checkpoint 4!");
                }
            }
            if (((monster->position.x >= 60) && (monster->position.x <= 70)) && ((monster->position.y <= 10) && (monster->position.y >= -10))) {
                if (echeckpoint == 4) {
                    slog("enemy lap!");
                    echeckpoint = 0;
                    elap += 1;
                    slog("enemy laps total: %i", elap);
                }
            }
            if (startgame == 1) {
                if (echeckpoint == 0) {
                    gfc_vector2d_move_towards(&monsterxy, gfc_vector2d(monster->position.x, monster->position.y), gfc_vector2d(65, -85), 0.3);
                    monster->position.x = monsterxy.x;
                    monster->position.y = monsterxy.y;
                }
                if (echeckpoint == 1) {
                    gfc_vector2d_move_towards(&monsterxy, gfc_vector2d(monster->position.x, monster->position.y), gfc_vector2d(-40, -85), 0.3);
                    monster->position.x = monsterxy.x;
                    monster->position.y = monsterxy.y;
                }
                if (echeckpoint == 2) {
                    gfc_vector2d_move_towards(&monsterxy, gfc_vector2d(monster->position.x, monster->position.y), gfc_vector2d(-40, 70), 0.3);
                    monster->position.x = monsterxy.x;
                    monster->position.y = monsterxy.y;
                }
                if (echeckpoint == 3) {
                    gfc_vector2d_move_towards(&monsterxy, gfc_vector2d(monster->position.x, monster->position.y), gfc_vector2d(65, 70), 0.3);
                    monster->position.x = monsterxy.x;
                    monster->position.y = monsterxy.y;
                }
                if (echeckpoint == 4) {
                    gfc_vector2d_move_towards(&monsterxy, gfc_vector2d(monster->position.x, monster->position.y), gfc_vector2d(65, 0), 0.3);
                    monster->position.x = monsterxy.x;
                    monster->position.y = monsterxy.y;
                }
            }
            
            gf2d_mouse_draw();
            gf3d_vgraphics_render_end();
            if (pausetimer > 0) pausetimer--;

        
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
