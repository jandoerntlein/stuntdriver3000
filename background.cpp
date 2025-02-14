#include "background.h"

Background::Background(Player* player) {

	this->player = player;

	backgroundShader = new shader(find_shader("pos+norm+tc"));
	m_tex1    = new texture( "sky", "sky.png");
    m_tex2    = new texture( "skyline", "skyline3.png", texture::RGBA );

	// create drawelement
    background1 = new drawelement( "sky" );
    background1->add_texture( "skytex", m_tex1 );

    background2 = new drawelement( "skyline" );
    background2->add_texture( "skytex", m_tex2 );

    m_scale1 = 2000;
    m_scale2 = 1000;

    // define quads
    vec3f quad1[] = {
        { 0,        0,          0 },
        { 2*m_scale1,  0,          0 },
        { 0,        m_scale1,    0 },
        { 2*m_scale1,  m_scale1,    0 }
    };

    vec3f quad2[] = {
        { 0,        0,          0 },
        { 2*m_scale2,  0,          0 },
        { 0,        2*m_scale2,    0 },
        { 2*m_scale2,  2*m_scale2,    0 }
    };

    vec3f norm[] = {
        { 0,        0,          1 },
        { 0,  0,          1 },
        { 0,        0,    1 },
        { 0,  0,    1 }
    };

    vec2f quadtc[] = {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    };

    unsigned int quad_idx[] = {
        0, 1, 2, 2, 1, 3
    };

    // fill data structures
    mesh* sky_mesh = new mesh( "quad", stringlist( "in_pos", "in_norm", "in_tc") );
    sky_mesh->bind();
    sky_mesh->add_vertex_data( "in_pos", 4, quad1 );
    sky_mesh->add_vertex_data( "in_norm", 4, norm );
    sky_mesh->add_vertex_data( "in_tc", 4, quadtc );
    sky_mesh->add_index_data( 6, quad_idx );
    sky_mesh->unbind();

    mesh* skyline_mesh = new mesh( "quad", stringlist( "in_pos", "in_norm", "in_tc") );
    skyline_mesh->bind();
    skyline_mesh->add_vertex_data( "in_pos", 4, quad2 );
    skyline_mesh->add_vertex_data( "in_norm", 4, norm );
    skyline_mesh->add_vertex_data( "in_tc", 4, quadtc );
    skyline_mesh->add_index_data( 6, quad_idx );
    skyline_mesh->unbind();

    // add
    background1->add_mesh( sky_mesh );
    background2->add_mesh( skyline_mesh );


    // setup position etc
    background1->use_shader( shader_ptr( backgroundShader ) );
    background2->use_shader( shader_ptr( backgroundShader ) );

    // matrix
    make_unit_matrix4x4f( &trafo2 );
    pos2 = { -400, -900 , -450};
    trafo2.row_col( 0, 3 ) = pos2.x;
    trafo2.row_col( 1, 3 ) = pos2.y;
    trafo2.row_col( 2, 3 ) = pos2.z;

    make_unit_matrix4x4f( &trafo1 );
    pos1 = { -600, -600 , -500};
    trafo1.row_col( 0, 3 ) = pos1.x;
	trafo1.row_col( 1, 3 ) = pos1.y;
	trafo1.row_col( 2, 3 ) = pos1.z;

    // clouds
    clouds = constant::CLD_NUM;
    m_clouds   = new Cloud*[clouds];

    for( int i = 0; i < clouds; ++i ) {
        float size = ( ( rand() % 100 ) / 1000.0 ) + constant::CLD_SIZE_MIN;

        float decision = rand() % 100 > 50 ? 1.0 : -1.0;
        if( decision == 1.0 ) {
            m_clouds[i] = new Cloud( size, "cloud_small.png" );        
        } else {
            m_clouds[i] = new Cloud( size, "cloud_big.png" );                    
        }

        m_clouds[i]->m_pos.x = ( ( rand() % 100 ) / constant::CLD_DIST_X ) - 1.0;
        m_clouds[i]->m_pos.y = ( ( rand() % 100 ) / constant::CLD_DIST_Y ) + constant::CLD_SCREEN_BORDER;
    }
}

Background::~Background() {
	
}

void Background::draw() {

    // reset position
    trafo1.row_col( 0, 3 ) = player->getX() * 0.5 + pos1.x;
    trafo2.row_col( 0, 3 ) = player->getX() * 0.5 + pos2.x;

	// draw
    background1->bind();

    background1->apply_default_matrix_uniforms(trafo1);
    background1->Modelmatrix(&trafo1);
    background1->apply_default_tex_uniforms_and_bind_textures();
    
    vec3f dir1 = { 0, 0, -1 };
    vec3f col1 = { 1, 1, 1 };
    setup_light( background1->Shader(), &dir1, &col1, 1.0f );
    
    background1->draw_em();
    background1->unbind();

    // draw 2
    background2->bind();

    background2->apply_default_matrix_uniforms(trafo2);
    background2->Modelmatrix(&trafo2);
    background2->apply_default_tex_uniforms_and_bind_textures();
    
    vec3f dir2 = { 0, 0, -1 };
    vec3f col2 = { 1, 1, 1 };
    setup_light( background2->Shader(), &dir2, &col2, 1.0f );
    
    background2->draw_em();
    background2->unbind();

    // clouds
    for( int i = 0; i < clouds; ++i ) {
        m_clouds[i]->render();
    }
}