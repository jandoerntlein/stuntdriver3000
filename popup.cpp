#include "popup.h"

Popup::Popup( float scale, string s_texture ) {
	// load shader and texture
    m_shader = new shader( find_shader( "popup" ) );
    m_tex    = new texture( "popup", s_texture.c_str(), texture::RGBA );

    // create drawelement
    m_element = new drawelement( "popup" );
    m_element->add_texture( "popuptex", m_tex );

    m_scale = scale;

    // define quads
    vec3f quad[] = {
        { 0,        0,          -50 },
        { m_scale,  0,          -50 },
        { 0,        m_scale,    -50 },
        { m_scale,  m_scale,    -50 }
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
    mesh* mesh_pop = new mesh( "quad", stringlist( "in_pos", "in_tc" ) );
    mesh_pop->bind();
    mesh_pop->add_vertex_data( "in_pos", 4, quad );
    mesh_pop->add_vertex_data( "in_tc", 4, quadtc );
    mesh_pop->add_index_data( 6, quad_idx );
    mesh_pop->unbind();

    m_element->add_mesh( mesh_pop );

    // setup position etc
    m_element->use_shader( shader_ptr( m_shader ) );

    // matrix
    make_unit_matrix4x4f( &trafo );
    m_pos = { 0, 0 };

    // timer
    m_timer = new Timer();
    m_timer->start();
}

Popup::~Popup() {
	
}

void Popup::draw() {
    // reset position every 5 seconds
    if( m_timer->look() > 5000 ) {
        m_pos = { .5 * ( float ) ( rand() % 100 / 100.0 ),
                  .5 * ( float ) ( rand() % 100 / 100.0 ) };
        m_timer->start();
    }

	// draw
    m_element->bind();

    int loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "dpos");
    glUniform2f(loc, m_pos.x, m_pos.y );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "cloud");
    glUniform1f(loc, 0.0 );

    m_element->apply_default_matrix_uniforms();
    m_element->Modelmatrix(&trafo);
    m_element->apply_default_tex_uniforms_and_bind_textures();
    
    vec3f dir = { 0, 1, -1 };
    vec3f col = { 0.1, 0.1, 0.1 };
    setup_light( m_element->Shader(), &dir, &col, 1.0f );
    
    m_element->draw_em();
    m_element->unbind();
}

void Cloud::randomDir() {
    float dx = ( ( float )( rand() % 100 ) / 100.0 );
    float dy = ( ( float )( rand() % 100 ) / 100.0 );
    float xn = -1.0; //rand() % 100 > 50 ? 1.0 : -1.0;
    float yn = rand() % 100 > 50 ? 1.0 : -1.0;

    m_dir = { xn * dx, yn * dy };
}

void Cloud::render() {
    glEnable( GL_BLEND );  
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    // update direction 
    float everyXframes = constant::CLD_FRAMES + ( float )( rand() % constant::CLD_FRAMES_DISTRIBUTION );
    update_ticker++;
    if( update_ticker > everyXframes ) {
        randomDir();
        update_ticker = 0;
    }

    // update pos
    float speed = constant::CLD_SPEED;
    m_pos.x += 4.0 * speed * m_dir.x;
    m_pos.y += speed * m_dir.y;

    // check pos y
    if( m_pos.y < constant::CLD_SCREEN_BORDER ) m_pos.y = constant::CLD_SCREEN_BORDER;

    // reset pos x
    float xmin = 1.1;
    if( m_pos.x < -xmin ) m_pos.x = xmin;
    
    // draw
    m_element->bind();

    int loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "dpos");
    glUniform2f(loc, m_pos.x, m_pos.y );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "cloud");
    glUniform1f(loc, 1.0 );


    m_element->apply_default_matrix_uniforms();
    m_element->Modelmatrix(&trafo);
    m_element->apply_default_tex_uniforms_and_bind_textures();
    
    vec3f dir = { 0, 1, -1 };
    vec3f col = { 0.1, 0.1, 0.1 };
    setup_light( m_element->Shader(), &dir, &col, 1.0f );
    
    m_element->draw_em();
    m_element->unbind();

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
}