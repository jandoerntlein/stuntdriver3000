#include "wood.h"

WoodContainer::WoodContainer( Player* p, SplineMesh* s, int Woods ) {
    m_spline = s;    
    m_player = p;
    m_nWoods = Woods;

    m_playermindist = 100.0f;

    // ccgl 
    float scaling     = 1.0;
    ObjLoader loader1("Wood", "../assets/tree1.obj");    
    loader1.TranslateToOrigin();
    vec3f scale = { scaling, scaling, scaling };
    loader1.ScaleVertexData( scale );
    loader1.pos_and_norm_shader    = find_shader("pos+norm");
    loader1.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader1.GenerateNonsharingMeshesAndDrawElements( mesh1 );

    scaling     = 1.0;
    ObjLoader loader2("Wood", "../assets/tree2.obj");    
    loader2.TranslateToOrigin();
    scale = { scaling, scaling, scaling };
    loader2.ScaleVertexData( scale );
    loader2.pos_and_norm_shader    = find_shader("pos+norm");
    loader2.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader2.GenerateNonsharingMeshesAndDrawElements( mesh2 );

    scaling     = 2.0;
    ObjLoader loader3("Wood", "../assets/tree3.obj");    
    loader3.TranslateToOrigin();
    scale = { scaling, scaling, scaling };
    loader3.ScaleVertexData( scale );
    loader3.pos_and_norm_shader    = find_shader("pos+norm");
    loader3.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader3.GenerateNonsharingMeshesAndDrawElements( mesh3 );

    setBack( s, Woods );
}

void WoodContainer::setBack(SplineMesh* s, int Woods) {

    m_spline = s;
    m_nWoods = Woods;
    m_playermindist = 100.0f;

    int   xdivergence = 20;
    int   ydivergence = 5;

    int nSplineverts = m_spline->spline_strip.size();
    int ratio        = ( int )( ( float )nSplineverts / ( float )m_nWoods );

    m_Woods.clear();
    // create m_nWoods and spread them over the spline
    for( int i = 0; i < m_nWoods; ++i ) {
        // get idx over selected spline
        int idx = i * ratio + rand()%20;
  
        // get Wood position and add random vector
        float mx = m_spline->spline_strip[idx].x;
        float my = m_spline->spline_strip[idx].y;

        // create Wood and save in our container
        Wood *c = 0;
        int decision = rand() % 3;
        if( decision == 0 )
            c = new Wood( m_player, 0, mesh1, mx, my );
        if( decision == 1 )
            c = new Wood( m_player, 1, mesh2, mx, my );
        if( decision == 2 )
            c = new Wood( m_player, 2, mesh3, mx, my );
        m_Woods.push_back( c );
    }

}

void WoodContainer::draw() {
    for( vector<Wood*>::iterator it = m_Woods.begin(); it != m_Woods.end(); ++it ) {
        Wood *b = *it;

        // draw only if in screen
        if( b->getDistanceToPlayer() < m_playermindist ) {
            b->draw();            
        }
        
    }
}

void WoodContainer::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    for( vector<Wood*>::iterator it = m_Woods.begin(); it != m_Woods.end(); ++it ) {
        Wood *b = *it;

        // draw only if in screen
        if( b->getDistanceToPlayer() < m_playermindist ) {
            b->draw(depthtexture, tmatrix);            
        }
        
    }
}

WoodContainer::~WoodContainer() {
    for( vector<Wood*>::iterator it = m_Woods.begin(); it != m_Woods.end(); ++it ) {
        delete *it;
    }
}

Wood::Wood( Player* p, int type, vector<drawelement*> mesh, float x, float y ) {
    m_player = p;
    m_type   = type;

    // set mesh
    wood = mesh;

    // set rotation speed
    m_rotationSpeed = 2.0 * ( float )( rand() % 100 ) / 100.0;

    // initial position
    make_unit_matrix4x4f( &trafo );
    trafo.row_col( 0, 3 ) = x;

    float zval = float( rand() % 8 + 7 );// between -8 and -16
    trafo.row_col( 2, 3 ) = -zval;

    // calculate y value
    double i  = log( zval ) / log( 2.0 );
    float  dy = pow( 1.5, i ) + y; 

    if( m_type == 0 )      // baum 1
        trafo.row_col( 1, 3 ) = dy + 0;
    else if( m_type == 1 ) // baum 2
        trafo.row_col( 1, 3 ) = dy + 0;
    else if( m_type == 2 ) // farn
        trafo.row_col( 1, 3 ) = dy - 1.8;
}

Wood::~Wood() {
    
}

float Wood::getDistanceToPlayer() {
    return m_player->getDistanceTo( trafo.row_col( 0, 3 ), trafo.row_col( 1, 3 ) );
}

void Wood::draw() {

    // draw
    for (vector<drawelement*>::iterator it = wood.begin(); it != wood.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 0.2, -1 };
        vec3f col = { .5, 1, .5 };
        setup_light( de->Shader(), &dir, &col, 1.0f );
        
        de->draw_em();
        de->unbind();
    } 
}

void Wood::draw(texture_ref depthtexture, matrix4x4f* tmatrix) {
    // draw
    for (vector<drawelement*>::iterator it = wood.begin(); it != wood.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms();
        de->Modelmatrix(&trafo);
        de->apply_default_tex_uniforms_and_bind_textures();
        
        vec3f dir = { 0, 0.2, -1 };
        vec3f col = { .5, 1, .5 };
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