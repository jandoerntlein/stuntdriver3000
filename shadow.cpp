#include "shadow.h"

Shadow::Shadow( Player* p, texture_ref depth ) {
	m_player = p;

	m_main_cam = current_camera();

    m_matT = new matrix4x4f();
    make_unit_matrix4x4f( m_matT );

	vec3f m_pos = {0,50,0};
    vec3f m_dir = {0,-1,0}; 
    vec3f m_up  = {0,0,-1};
	m_cam     = make_orthographic_cam( "shadowcam", &m_pos, &m_dir, &m_up, 50, -50, 50, -50, 1, 1000);

    // framebuffer stuff
 //    tex_params_t params = default_fbo_tex_params();
 //    params.wrap_s = GL_CLAMP_TO_EDGE;
 //    params.wrap_t = GL_CLAMP_TO_EDGE;

 //    float size = constant::SHADOW_RES;

	// m_fbo   = make_framebuffer( "depthbuffer", size, size );

	// if( !valid_framebuffer_ref( m_fbo ) ) {
 //        std::cout << "-- Framebuffer error occured" << std::endl;
 //        return;
 //    }
 //    bind_framebuffer( m_fbo );
 //    attach_depth_buffer( m_fbo );

	// m_depth = make_empty_texture("depthtexture", size, size, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, GL_FLOAT, GL_DEPTH_COMPONENT, &params );

	// bind_texture( m_depth, 0 );
 //    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
 //    unbind_texture( m_depth );

 //    attach_texture_as_depthbuffer( m_fbo, "depth", m_depth );
 //    unbind_framebuffer( m_fbo );
    m_depth = depth;
}

Shadow::~Shadow() {
	
}

matrix4x4f* Shadow::T() {
    // column major!
    matrix4x4f S   = { 0.5, 0,   0,   0,
                       0,   0.5, 0,   0,
                       0,   0,   0.5, 0,
                       0.5, 0.5, 0.5, 1 };
    matrix4x4f PL  = *projection_matrix_of_cam( m_cam );
    matrix4x4f VL  = *gl_view_matrix_of_cam( m_cam );
    matrix4x4f VC  = *gl_view_matrix_of_cam( m_main_cam );
    
    matrix4x4f _VC;   
    invert_matrix4x4f( &_VC, &VC );

    matrix4x4f tmp, tmp2;
    multiply_matrices4x4f( &tmp, &VL, &_VC );
    multiply_matrices4x4f( &tmp2, &PL, &tmp );

    multiply_matrices4x4f( m_matT, &S, &tmp2 );
    return m_matT;
}

void Shadow::updatePos() {
    vec3f cam_pos;
    matrix4x4f *lookat = lookat_matrix_of_cam( m_cam );
    extract_pos_vec3f_of_matrix( &cam_pos, lookat );

    cam_pos.x = m_player->getX();
    cam_pos.y = m_player->getY() + 15;
    cam_pos.z = 0; 
    m_pos = cam_pos;

    m_dir = {-0.5,-1,-0.5}; 
    m_up  = {0,0,-1};

    // cout << m_pos.x << ", " << m_pos.y << ", " << m_pos.z << ", " << endl;
    // cout << m_dir.x << ", " << m_dir.y << ", " << m_dir.z << ", " << endl;
    // cout << m_up.x << ", " << m_up.y << ", " << m_up.z << ", " << endl;

    make_lookat_matrixf( lookat, &m_pos, &m_dir, &m_up );
    recompute_gl_matrices_of_cam( m_cam );

    use_camera( m_cam );
}

void Shadow::begin(framebuffer_ref fbo) {
    updatePos();

	// switch to the light's view: neccessary for the framebuffer
    use_camera( m_cam );

    // draw to framebuffer
    bind_framebuffer( fbo );
    // clear old stuff
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // erase noise
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 0.5, 0.5 );
    // no color
    glColorMask( 0, 0, 0, 0 );
}

void Shadow::end(framebuffer_ref fbo) {
    // unbind and clear up
    unbind_framebuffer( fbo );
    glColorMask( 1, 1, 1, 1 );
    //glDrawBuffer( GL_FRONT );
    glDisable( GL_POLYGON_OFFSET_FILL );

    // debug
#ifdef DEBUG_MODE
    save_texture_as_png( m_depth, "test.png" );
#endif

    // switch back to original camera
    use_camera( m_main_cam );

#ifdef DEBUG_MODE
    std::cout << "-- Screenshot finished" << std::endl;
    exit( 0 );
#endif
}

texture_ref Shadow::depthtexture() {
	return m_depth;
}