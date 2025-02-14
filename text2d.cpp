#include "text2d.h"

Text2D::Text2D() {
	m_quadelement   = new drawelement( "meshelement" );
	m_fonttexture 	= new texture("font", "colorfont2.png", texture::RGBA );
	//m_quad			= new mesh( "quad", stringlist( "in_pos", "in_tc" ) );
	m_shader        = shader_ptr( new shader( find_shader( "font" ) ) );

	m_width  = m_fonttexture->width();
	m_height = m_fonttexture->height();

	make_unit_matrix4x4f( &trafo );

	vec3f quad[] = {
		{ 0, 0, 0 },
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 1, 1, 0 }
	};

	unsigned int quad_idx[] = {
		0, 1, 2, 2, 1, 3
	};

	// m_quad->bind();
	// m_quad->add_vertex_data( "in_pos", 4, quad );
	// m_quad->add_index_data( 6, quad_idx );
	// m_quad->unbind();

	//m_quadelement->add_mesh( m_quad );
	m_quadelement->use_shader( m_shader );
	m_quadelement->add_texture( "font", m_fonttexture );
}

Text2D::~Text2D() {
	
}

void Text2D::init( int x, int y, string text, int textsize ) {
	vector<vec3f> verts;
	vector<vec2f> uvs;
	vector<unsigned int> idx;

	int j = 0;


	for( int i = 0; i < text.size(); ++i ) {
		
		// -0.05 bis 0.05
		float ran = (float)(rand() % constant::TEXT_SHAKE_HORI - constant::TEXT_SHAKE_HORI/2) / 100 * textsize;

		// -0.1 bis 0.1
		float ran2 = (float)(rand() % constant::TEXT_SHAKE_VERT - constant::TEXT_SHAKE_VERT/2) / 100 * textsize;

		vec3f left_bottom  = { x+i*textsize*0.7+ran, y+ran2, 0 };
		vec3f right_bottom = { x+i*textsize*0.7+textsize*0.7, y+ran2, 0 };
		vec3f left_top     = { x+i*textsize*0.7, y+textsize+ran2, 0 };
		vec3f right_top    = { x+i*textsize*0.7+textsize*0.7-ran, y+textsize+ran2, 0 };

		left_bottom.x  = ( left_bottom.x - ( 0.5 * m_width ) ) / ( 0.5 * m_width );
		right_bottom.x = ( right_bottom.x - ( 0.5 * m_width ) ) / ( 0.5 * m_width );
		left_top.x     = ( left_top.x - ( 0.5 * m_width ) ) / ( 0.5 * m_width );
		right_top.x    = ( right_top.x - ( 0.5 * m_width ) ) / ( 0.5 * m_width );

		left_bottom.y  = ( left_bottom.y - ( 0.5 * m_height ) ) / ( 0.5 * m_height );
		right_bottom.y = ( right_bottom.y - ( 0.5 * m_height ) ) / ( 0.5 * m_height );
		left_top.y     = ( left_top.y - ( 0.5 * m_height ) ) / ( 0.5 * m_height );
		right_top.y    = ( right_top.y - ( 0.5 * m_height ) ) / ( 0.5 * m_height );

		char character = text[i];
		float uv_x     = ( character % 16 ) / 16.0;
		float uv_y     = ( character / 16 ) / 16.0 + 0.0069;

		vec2f uv_left_bottom 	= { uv_x + 0.15f/16, 1.0f - ( uv_y + 1.0f / 16.0f ) + 0.027/16 };
		vec2f uv_right_bottom 	= { uv_x + 0.8f / 16.0f, 1.0f - ( uv_y + 1.0f  /16.0f ) + 0.027/16 };
		vec2f uv_left_top 		= { uv_x + 0.15f/16, 1.0f - uv_y -0.02/16};
		vec2f uv_right_top 		= { uv_x + 0.8f / 16.0f, 1.0f - uv_y -0.02/16};

		verts.push_back( left_bottom );
		verts.push_back( right_bottom );
		verts.push_back( left_top );
		verts.push_back( left_top );
		verts.push_back( right_bottom );
		verts.push_back( right_top );

		uvs.push_back( uv_left_bottom );
		uvs.push_back( uv_right_bottom );
		uvs.push_back( uv_left_top );
		uvs.push_back( uv_left_top );
		uvs.push_back( uv_right_bottom );
		uvs.push_back( uv_right_top );

		for( int k = 0; k < 6; ++k ) {
			idx.push_back( j++ );
		}
	}

	m_quad = new mesh( "quad", stringlist( "in_pos", "in_tc" ) );
	m_quad->bind();
	m_quad->add_vertex_data( "in_pos", verts.size(), &verts[0] );
	m_quad->add_vertex_data( "in_tc", uvs.size(), &uvs[0] );
	m_quad->add_index_data( idx.size(), &idx[0] );
	m_quad->unbind();

	m_quadelement->meshes.clear();
	m_quadelement->add_mesh( m_quad );
}

void Text2D::draw( int x, int y, string text, int textsize ) {
    glEnable( GL_BLEND );  
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	init( x, y, text, textsize );

    m_quadelement->bind();
    m_quadelement->apply_default_matrix_uniforms();
    m_quadelement->Modelmatrix(&trafo);
    m_quadelement->apply_default_tex_uniforms_and_bind_textures();
    m_quadelement->draw_em();
    m_quadelement->unbind();

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
}
