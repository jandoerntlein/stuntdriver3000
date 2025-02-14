#include "hud.h"

Hud::Hud() {
	// load shader and texture
    m_shader = new shader( find_shader( "hud" ) );

    // create drawelement
    m_element = new drawelement( "Hud" );

    m_scale = 0.2;

    // define quads
    m_pos = { 10, 720 };
    m_pos.x /= 1024.0;
    m_pos.y /= 768.0;
    m_pos.x *= 2.0;
    m_pos.y *= 2.0;
    m_pos.x -= 1.0;
    m_pos.y -= 1.0;

    vec3f quad1[] = {
        { m_pos.x,              m_pos.y,          0 },
        { 6*m_scale+m_pos.x,    m_pos.y,          0 },
        { m_pos.x,              0.5*m_scale+m_pos.y,    0 },
        { 6*m_scale+m_pos.x,    0.5*m_scale+m_pos.y,    0 }
    };

    unsigned int quad_idx[] = {
        0, 1, 2, 2, 1, 3
    };

    // fill data structures
    mesh* mesh_1 = new mesh( "quad", stringlist( "in_pos" ) );
    mesh_1->bind();
    mesh_1->add_vertex_data( "in_pos", 4, quad1 );
    mesh_1->add_index_data( 6, quad_idx );
    mesh_1->unbind();

    m_element->add_mesh( mesh_1 );

    // setup position etc
    m_element->use_shader( shader_ptr( m_shader ) );

    // matrix
    make_unit_matrix4x4f( &trafo );

}

Hud::~Hud() {
	
}

void Hud::draw( float amount ) {
	// draw
    m_element->bind();

    int loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "amount");
    glUniform1f( loc, amount * 0.5 );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "left");
    glUniform1f( loc, m_pos.x );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "right");
    glUniform1f( loc, m_pos.x+6*m_scale );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "bottom");
    glUniform1f( loc, m_pos.y );

    loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "top");
    glUniform1f( loc, m_pos.y+m_scale );

    m_element->apply_default_matrix_uniforms();
    m_element->Modelmatrix(&trafo);
    m_element->apply_default_tex_uniforms_and_bind_textures();
    
    vec3f dir = { 0, 1, -1 };
    vec3f col = { 0.1, 0.1, 0.1 };
    setup_light( m_element->Shader(), &dir, &col, 1.0f );
    
    m_element->draw_em();
    m_element->unbind();
}