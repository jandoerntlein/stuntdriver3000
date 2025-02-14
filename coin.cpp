#include "coin.h"

CoinContainer::CoinContainer(  Player* p, SplineMesh* s, int coins ) {
    m_spline = s;    
    m_player = p;
    m_nCoins = coins;

    m_playermindist = 100.0f;

    // ccgl 
    float scaling     = 0.5;
    ObjLoader loader("coin", "../assets/coin.obj");
    loader.TranslateToOrigin();
    vec3f scale = { scaling, scaling, scaling };
    loader.ScaleVertexData( scale );
    loader.pos_and_norm_shader    = find_shader("pos+norm");
    loader.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader.GenerateNonsharingMeshesAndDrawElements( mesh );

    setBack( s, coins );
}

void CoinContainer::setBack(SplineMesh* s, int coins) {

    m_spline = s;
    m_nCoins = coins;
    m_playermindist = 100.0f;

    int   xdivergence = 20;
    int   ydivergence = 5;

    int nSplineverts = m_spline->spline_strip.size();
    int ratio        = ( int )( ( float )nSplineverts / ( float )m_nCoins );

    m_coins.clear();
    // create m_nCoins and spread them over the spline
    for( int i = 0; i < m_nCoins; ++i ) {
        // get idx over selected spline
        int idx = i * ratio + rand()%20;

        // spread coins randomly 
        float dy = rand() % ydivergence + 2;
        if( dy < 1.0 ) dy = 1.0;

        // get coin position and add random vector
        float mx = m_spline->spline_strip[idx].x;
        float my = m_spline->spline_strip[idx].y + dy;

        // create coin and save in our container
        Coin *c = new Coin( m_player, mesh, mx, my );
        m_coins.push_back( c );
    }

}

void CoinContainer::draw() {
    for( vector<Coin*>::iterator it = m_coins.begin(); it != m_coins.end(); ++it ) {
        Coin* c = *it;
        if( c->isEaten() ) {
            m_coins.erase( it );
        } else {
            // draw only if in screen
            if( c->getDistanceToPlayer() < m_playermindist ) {
                c->draw();            
            }
        }
    }
}

void CoinContainer::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    for( vector<Coin*>::iterator it = m_coins.begin(); it != m_coins.end(); ++it ) {
        Coin* c = *it;
        if( c->isEaten() ) {
            m_coins.erase( it );
        } else {
            // draw only if in screen
            if( c->getDistanceToPlayer() < m_playermindist ) {
                c->draw( depthtexture, tmatrix );            
            }
        }
    }
}

CoinContainer::~CoinContainer() {
    for( vector<Coin*>::iterator it = m_coins.begin(); it != m_coins.end(); ++it ) {
        delete *it;
    }
}

Coin::Coin( Player* p, vector<drawelement*> mesh, float x, float y ) {
    m_player = p;

    // set mesh
    coin = mesh;

    // set rotation speed
    m_rotationSpeed = 2.0 * ( float )( rand() % 100 ) / 100.0;

    // initial position
    make_unit_matrix4x4f( &trafo );
    trafo.row_col( 0, 3 ) = x;
    trafo.row_col( 1, 3 ) = y;
    trafo.row_col( 2, 3 ) = 0;

    // we are not eaten by the player yet...
    m_coinIsEeaten = false;
}

Coin::~Coin() {
	
}

float Coin::getDistanceToPlayer() {
    return m_player->getDistanceTo( trafo.row_col( 0, 3 ), trafo.row_col( 1, 3 ) );
}

void Coin::update() {
    // rotation
    matrix4x4f rot;
    vec3f axis = { 0, 1, 0 };
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

    // player gets coin 
    if( getDistanceToPlayer() < constant::DST_PLAYER_COIN ) {
        vec3f dirToPlayer = { m_player->getX() - trafo.row_col( 0, 3 ), 
                              m_player->getY() - trafo.row_col( 1, 3 ), };
        normalize_vec3f( &dirToPlayer );

        // move
        trafo.row_col( 0, 3 ) += dirToPlayer.x * constant::COIN_SPEED;
        trafo.row_col( 1, 3 ) += dirToPlayer.y * constant::COIN_SPEED;
    }

    if( getDistanceToPlayer() < constant::EAT_DISTANCE_COI && !m_player->isOutOfGas()) {
        m_player->addCoin();
        m_coinIsEeaten = true;
    }
}

void Coin::draw() {
    // rotate
    update();

    // draw
    for (vector<drawelement*>::iterator it = coin.begin(); it != coin.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 0.7, 0.7, 0.1 };
        setup_light( de->Shader(), &dir, &col, 1.0f );
        
        de->draw_em();
        de->unbind();
    } 
}


void Coin::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    // draw
    for (vector<drawelement*>::iterator it = coin.begin(); it != coin.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 0.7, 0.7, 0.1 };
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