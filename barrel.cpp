#include "barrel.h"

BarrelContainer::BarrelContainer( Player* p, SplineMesh* s, int Barrels ) {
    m_spline = s;    
    m_player = p;
    m_nBarrels = Barrels;

    m_playermindist = 100.0f;

    // ccgl 
    float scaling     = 3.0;
    ObjLoader loader("barrel", "../assets/barrel.obj");    
    loader.TranslateToOrigin();
    vec3f scale = { scaling, scaling, scaling };
    loader.ScaleVertexData( scale );
    loader.pos_and_norm_shader    = find_shader("pos+norm");
    loader.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader.GenerateNonsharingMeshesAndDrawElements( mesh );

    setBack( s, Barrels );
}

void BarrelContainer::setBack(SplineMesh* s, int Barrels) {

    m_spline = s;
    m_nBarrels = Barrels;
    m_playermindist = 100.0f;

    int   xdivergence = 20;
    int   ydivergence = 5;

    int nSplineverts = m_spline->spline_strip.size();
    int ratio        = ( int )( ( float )nSplineverts / ( float )m_nBarrels );

    m_Barrels.clear();
    // create m_nBarrels and spread them over the spline
    for( int i = 0; i < m_nBarrels; ++i ) {
        // get idx over selected spline
        int idx = i * ratio + rand()%20;

        // spread Barrels randomly 
        float dy = rand() % ydivergence + 2;
        if( dy < 1.0 ) dy = 1.0;

        // get Barrel position and add random vector
        float mx = m_spline->spline_strip[idx].x;
        float my = m_spline->spline_strip[idx].y + dy;

        // create Barrel and save in our container
        Barrel *c = new Barrel( m_player, mesh, mx, my );
        m_Barrels.push_back( c );
    }

}

void BarrelContainer::draw() {
    for( vector<Barrel*>::iterator it = m_Barrels.begin(); it != m_Barrels.end(); ++it ) {
        Barrel *b = *it;
        if( b->isEaten() ) {
            m_Barrels.erase( it );
        } else {
            // draw only if in screen
            if( b->getDistanceToPlayer() < m_playermindist ) {
                b->draw();            
            }
        }
    }
}

void BarrelContainer::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    for( vector<Barrel*>::iterator it = m_Barrels.begin(); it != m_Barrels.end(); ++it ) {
        Barrel *b = *it;
        if( b->isEaten() ) {
            m_Barrels.erase( it );
        } else {
            // draw only if in screen
            if( b->getDistanceToPlayer() < m_playermindist ) {
                b->draw(depthtexture, tmatrix);            
            }
        }
    }
}

BarrelContainer::~BarrelContainer() {
    for( vector<Barrel*>::iterator it = m_Barrels.begin(); it != m_Barrels.end(); ++it ) {
        delete *it;
    }
}

Barrel::Barrel( Player* p, vector<drawelement*> mesh, float x, float y ) {
    m_player = p;

    // set mesh
    barrel = mesh;

    // set rotation speed
    m_rotationSpeed = 2.0 * ( float )( rand() % 100 ) / 100.0;

    // initial position
    make_unit_matrix4x4f( &trafo );
    trafo.row_col( 0, 3 ) = x;
    trafo.row_col( 1, 3 ) = y;
    trafo.row_col( 2, 3 ) = 0;

    // we are not eaten by the player yet...
    m_BarrelIsEeaten = false;
}

Barrel::~Barrel() {
    
}

float Barrel::getDistanceToPlayer() {
    return m_player->getDistanceTo( trafo.row_col( 0, 3 ), trafo.row_col( 1, 3 ) );
}

void Barrel::update() {
    // rotation
    matrix4x4f rot;
    vec3f axis = { 0.2, 1, 0 };
    make_rotation_matrix4x4f( &rot, &axis, M_PI / 180 * m_rotationSpeed );

    // move trafo to origin
    vec3f org = {trafo.row_col( 0, 3 ),trafo.row_col( 1, 3 ),trafo.row_col( 2, 3 )};
    trafo.row_col( 0, 3 ) = 0;
    trafo.row_col( 1, 3 ) = 0;
    trafo.row_col( 2, 3 ) = 0;

    // rotate
    multiply_matrices4x4f( &trafo, &rot, &trafo );

    // move back
    trafo.row_col( 0, 3 ) = org.x;
    trafo.row_col( 1, 3 ) = org.y;
    trafo.row_col( 2, 3 ) = org.z;

    // player gets Barrel 
    if( getDistanceToPlayer() < constant::DST_PLAYER_BAR ) {
        vec3f dirToPlayer = { m_player->getX() - trafo.row_col( 0, 3 ), 
                              m_player->getY() - trafo.row_col( 1, 3 ), };
        normalize_vec3f( &dirToPlayer );

        // move
        trafo.row_col( 0, 3 ) += dirToPlayer.x * constant::BARREL_SPEED;
        trafo.row_col( 1, 3 ) += dirToPlayer.y * constant::BARREL_SPEED;
    }

    // check if player got Barrel
    if( getDistanceToPlayer() < constant::EAT_DISTANCE_BAR && !m_player->isOutOfGas()) {
        m_player->addBarrel();
        m_BarrelIsEeaten = true;
    }
}

void Barrel::draw() {
    // rotate
    update();

    // draw
    for (vector<drawelement*>::iterator it = barrel.begin(); it != barrel.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 1, 1, 1 };
        setup_light( de->Shader(), &dir, &col, 1.0f );
        
        de->draw_em();
        de->unbind();
    } 
}

void Barrel::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    // draw
    for (vector<drawelement*>::iterator it = barrel.begin(); it != barrel.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 1, 1, 1 };
        setup_light( de->Shader(), &dir, &col, 1.0f );

        int loc = de->Shader()->location_of_uniform( "tmatrix" );
        glUniformMatrix4fv(loc, 1, GL_FALSE, tmatrix->col_major);
        
        loc = de->Shader()->location_of_uniform( "shadow_flag" );
        glUniform1f(loc, 1.0);

        loc = de->Shader()->location_of_uniform( "depthtexture" );
        bind_texture(depthtexture, 5);
        glUniform1i(loc, 5);
        
        de->draw_em();
        de->unbind();
    } 
}