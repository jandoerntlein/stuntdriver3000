#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <lib3dmath/matrix.h>
#include <libcgl/cgl.h>
#include <libcgl/camera.h>
#include <libguile.h>
#include <chipmunk/chipmunk.h>
using namespace std;

#include "view.h"
#include "text2d.h"
#include "game.h"
#include "globalConstants.h"

// global objects
Text2D*     text2d;
View *main_view;
View *race_view;
Game* game;

static void drawFPS() {
    ostringstream out;
    out << "FPS: " << game->getFPS();
    text2d->draw( 5, 5, out.str(), 12 );
}

void keyhandler(unsigned char key, int x, int y) {
    
    if (key == 'a')      game->player->rotateLeft();
    else if (key == 'd') game->player->rotateRight();
    else if (key == 's') game->player->pullDown();
    else if (key == 'n' && (game->player->explosionFlagSet() || game->player->isOutOfGas())) {
        game->newGame( false );
    }
    else if (key == 'm' && (game->player->explosionFlagSet() || game->player->isOutOfGas())) {
        game->newGame( true );
    } 
    else if( key == char( 27 ) ) exit( 0 ); // escape to exit

}

void dummy( int a, int b, int c, int d ) {}

void display() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render player and ground
    game->update();

    // camera
    vec3f pos = { game->player->getX(), game->player->getY() + constant::CAMERA_POS_Y, constant::CAMERA_POS_Z };
    main_view->moveCameraToPos( pos );

    //drawFPS();

    check_for_gl_errors("display");
    swap_buffers();

}

void initRaceCam() {

    vec3f pos = {0,0,0};
    vec3f dir = {1,0,0}; 
    vec3f up = {0,5,0};
    race_view = new View(pos, dir, up, 35, 1, 1, 500);
    race_view->moveCamera(up);

}


void actual_main() {
    
    append_image_path( "../assets" );
    load_configfile("stuntdriver.scm");
    
    register_display_function(display);
    register_idle_function(display);
    register_keyboard_function(keyhandler);
    register_mouse_function(dummy);

    vec3f pos = {0, 0, constant::CAMERA_POS_Z};
    vec3f dir = {0, constant::CAMERA_DIR_Y, -1};
    vec3f up  = {0, constant::CAMERA_POS_Y, 0};
    main_view = new View(pos, dir, up, 35, 1, 1, 10000);
    main_view->moveCamera(up);

    if( constant::FULLSCREEN )
        glutFullScreen();

    use_camera(main_view->cam);

    glClearColor( constant::BG_RED/255.0, constant::BG_GRN/255.0, constant::BG_BLU/255.0, 1.0 );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

    // run forest run
    game = new Game();

    // Text
    text2d = new Text2D();

    // launch
    enter_glut_main_loop();

}

int main(int argc, char **argv)
{
    // int guile_mode = with_guile;
    int guile_mode = guile_cfg_only;
    startup_cgl("stuntdriver", 3, 3, argc, argv, constant::SCREEN_WIDTH, constant::SCREEN_HEIGHT, actual_main, guile_mode, true, 0);
    return 0;
}
