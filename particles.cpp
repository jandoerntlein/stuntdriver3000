#include "particles.h"

static float random01() {
    return ( float )( rand() % 100 ) / 100.0;
}

Particles::Particles( vec3f& pos, vec3f& dir, vec3f& col, int nCount, float scale, float speed, float spread, int alive, string s_texture ) {
    m_pos = pos;
    m_dir = dir;
    m_col = col;

    m_nCount = nCount;
    m_alive  = alive;

    m_scale  = scale;
    m_speed  = speed;
    m_spread = spread;

    is_explosion_mode = false;
    is_explosion_end  = false;

    init( s_texture );
}

void Particles::init( string s_texture ) {
    // load shader and texture
    m_shader = new shader( find_shader( "particle" ) );
    m_tex    = new texture( "exp", s_texture.c_str() );

    // initial positions
    currentPosition = new vec3f[ m_nCount ];
    explosionDir    = new vec3f[ m_nCount ];
    m_startingTime  = new double[ m_nCount ];

    // create drawelement
    m_particles = new drawelement( "particles" );
    m_particles->add_texture( "splat", m_tex );

    // define quads
    vec3f quad[] = {
        { 0, 0, 0 },
        { m_scale, 0, 0 },
        { 0, m_scale, 0 },
        { m_scale, m_scale, 0 }
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
    m_quad = new mesh( "quad", stringlist( "in_pos", "in_tc" ) );
    m_quad->bind();
    m_quad->add_vertex_data( "in_pos", 4, quad );
    m_quad->add_vertex_data( "in_tc", 4, quadtc );
    m_quad->add_index_data( 6, quad_idx );
    m_quad->unbind();

    m_particles->add_mesh( m_quad );

    // setup position etc
    m_particles->use_shader( shader_ptr( m_shader ) );

    make_unit_matrix4x4f( &trafo );
    trafo.row_col( 0, 3 ) = m_pos.x;
    trafo.row_col( 1, 3 ) = m_pos.y;
    trafo.row_col( 2, 3 ) = m_pos.z;

    // start timer
    m_timer = new Timer();
    m_timer->start();
    double now = m_timer->look();

    vec3f zero = {0,0,0};
    for( int i = 0; i < m_nCount; ++i ) {
        m_startingTime[i]  = now - random01() * m_alive;
        currentPosition[i] = zero;
    }
}

Particles::~Particles() {
    delete[] currentPosition;
    delete[] explosionDir;
    delete[] m_startingTime;
    // delete[] m_quad;
    // delete   m_shader;
    // delete   m_tex;
    // delete   m_particles;
    delete   m_timer;
}

void Particles::updatePos( vec3f& pos ) {
    trafo.row_col( 0, 3 ) = pos.x;
    trafo.row_col( 1, 3 ) = pos.y;
    trafo.row_col( 2, 3 ) = pos.z;
}

void Particles::updateDir( vec3f& dir ) {
    m_dir = dir;
}

void Particles::reset() {
    for( int which = 0; which < m_nCount; ++which ) {
        vec3f zero = {0,0,0};
        currentPosition[which] = zero;
        m_startingTime[which]  = m_timer->look();
    }
}

void Particles::setExplosionMode() {
    if( is_explosion_mode ) return;
    is_explosion_mode = true;

    // give really spreaded directions
    vec3f dir   = { 1, 0, 0 };
    float angle = 360.0 / ( float )m_nCount;

    for( int which = 0; which < m_nCount; ++which ) {
        float rndangle = angle * random01();
        
        dir.x = dir.x * cos( rndangle ) - dir.y * sin( rndangle );
        dir.y = dir.x * sin( rndangle ) + dir.y * cos( rndangle );

        normalize_vec3f( &dir );
        explosionDir[which] = dir;
    }

    reset();
}

void Particles::draw() {
    if( is_explosion_mode && is_explosion_end ) return;
    glEnable( GL_BLEND );  
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    for( int which = 0; which < m_nCount; ++which ) {
        int loc;

        m_shader->activate();

        if( !is_explosion_mode ) {
            vec3f dir = { m_dir.x * random01(), m_dir.y * random01(), m_dir.z * random01() };
            normalize_vec3f( &dir );

            currentPosition[which].x += dir.x * m_speed * random01();
            currentPosition[which].y += dir.y * m_speed * random01();
        }
        else {
            float sign = 0.3;
            float decc = 1.0;
            if( random01() > sign ) {
                currentPosition[which].x += explosionDir[which].x * m_speed * decc * random01();
                currentPosition[which].y += explosionDir[which].y * m_speed * decc * random01();
            } 
            else {
                currentPosition[which].x -= explosionDir[which].x * m_speed * decc * random01();
                currentPosition[which].y -= explosionDir[which].y * m_speed * decc * random01();                
            }

            if( m_timer->look() > m_startingTime[m_nCount-1] + m_alive ) {
                is_explosion_end = true;
            }
        }

        if( m_timer->look() > m_startingTime[which] + m_alive ) {
            vec3f zero = {0,0,0};
            currentPosition[which] = zero;
            m_startingTime[which]  = m_timer->look() - random01() * m_alive;
            continue;
        }
    
        loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "curpos");
        glUniform3f(loc, currentPosition[which].x, currentPosition[which].y, currentPosition[which].z );

        loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "which");
        glUniform1f(loc, float( which ) );

        loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "color");
        glUniform3f(loc, m_col.x, m_col.y, m_col.z );

        loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "adjust");
        glUniform1f(loc, float( 0 ) );

        m_particles->bind();
        m_particles->apply_default_matrix_uniforms();
        m_particles->Modelmatrix(&trafo);
        m_particles->apply_default_tex_uniforms_and_bind_textures();

        vec3f l_dir = { 0.1, 0.1, -1 };
        vec3f l_col = { 1, 0.0, 0.0 };
        setup_light( m_particles->Shader(), &l_dir, &l_col, 1.0f );
    
        m_particles->draw_em();
        m_particles->unbind();

        m_shader->deactivate();
    }

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
}


CarStripes::CarStripes( Player *p, vec3f& pos, vec3f& dir, vec3f& col,
                        int nCount, float scale, float speed,
                        float spread, int alive, string s_texture )  
                        : Particles( pos, dir, col, nCount, scale, speed, spread, alive, s_texture ) {
    m_player = p;
    make_unit_matrix4x4f( &trafo ); // reset position
}

void CarStripes::render() {

    float angle  = m_player->getRotation();
    vec3f direct = { cos( angle ), sin( angle ), 0 };
    float dist   = 2.0;
    vec3f wheel  = { m_player->getX() - dist * direct.x, m_player->getY() - dist * direct.y, 0 };
    m_carpositions.push_back( wheel );

    dist   = -1.0;
    wheel  = { m_player->getX() - dist * direct.x, m_player->getY() - dist * direct.y, 0 };
    m_carpositions.push_back( wheel );


    glEnable( GL_BLEND );  
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    int idxoffset = 5;

    int which = 0;
    if( m_carpositions.size() > idxoffset ) { 
        for( vector<vec3f>::iterator it = m_carpositions.begin(); it != m_carpositions.end() - idxoffset; ++it ) {
            int loc;

            m_shader->activate();

            currentPosition[which].x = ( *it ).x;
            currentPosition[which].y = ( *it ).y;

            if( m_timer->look() > m_startingTime[which] + m_alive ) {
                
                vec3f zero = {0,0,0};
                currentPosition[which] = zero;
                m_startingTime[which]  = m_timer->look() - random01() * m_alive;

            } else {

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "curpos");
                glUniform3f(loc, currentPosition[which].x, currentPosition[which].y, currentPosition[which].z );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "which");
                glUniform1f(loc, float( which ) / float( m_nCount ) );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "color");
                glUniform3f(loc, m_col.x, m_col.y, m_col.z );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "adjust");
                glUniform1f(loc, float( 1 ) );

                m_particles->bind();
                m_particles->apply_default_matrix_uniforms();
                m_particles->Modelmatrix(&trafo);
                m_particles->apply_default_tex_uniforms_and_bind_textures();

                vec3f l_dir = { 0.1, 0.1, -1 };
                vec3f l_col = { 0.0, 0.0, 0.5 };
                setup_light( m_particles->Shader(), &l_dir, &l_col, 1.0f );
            
                m_particles->draw_em();
                m_particles->unbind();

            }

            m_shader->deactivate();

            ++which;
        }
    }

    // remove latest position
    while( m_carpositions.size() >= m_nCount + idxoffset ) {
        m_carpositions.erase( m_carpositions.begin() );
    }   

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
}

CarStripes::~CarStripes() {
    // base class clears particles itself
}

Fire::Fire( Player *p, vec3f& pos, vec3f& dir, vec3f& col,
                        int nCount, float scale, float speed,
                        float spread, int alive, string s_texture )  
                        : CarStripes( p, pos, dir, col, nCount, scale, speed, spread, alive, s_texture ) {
}

void Fire::render() {

    float angle  = m_player->getRotation();
    vec3f direct = { cos( angle ), sin( angle ), 0 };
    float dist   = 2.0;
    vec3f wheel  = { m_player->getX() - dist * direct.x, m_player->getY() - dist * direct.y, 0 };
    m_carpositions.push_back( wheel );

    // dist   = -1.0;
    // wheel  = { m_player->getX() - dist * direct.x, m_player->getY() - dist * direct.y, 0 };
    // m_carpositions.push_back( wheel );


    glEnable( GL_BLEND );  
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    int idxoffset = 3;

    int which = 0;
    if( m_carpositions.size() > idxoffset ) { 
        for( vector<vec3f>::iterator it = m_carpositions.begin(); it != m_carpositions.end() - idxoffset; ++it ) {
            for( int i = 0; i < 15; ++i ) {

            int loc;

            m_shader->activate();

            float distx = 1.0;
            float disty = 1.0;
            currentPosition[which].x = ( *it ).x + random01() * distx;
            currentPosition[which].y = ( *it ).y + random01() * disty - 1.0;

            if( m_timer->look() > m_startingTime[which] + m_alive ) {
                
                vec3f zero = {0,0,0};
                currentPosition[which] = zero;
                m_startingTime[which]  = m_timer->look() - random01() * m_alive;

            } else {

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "curpos");
                glUniform3f(loc, currentPosition[which].x, currentPosition[which].y, currentPosition[which].z );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "which");
                glUniform1f(loc, float( which ) / float( m_nCount ) );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "color");
                glUniform3f(loc, m_col.x, m_col.y, m_col.z );

                loc = glGetUniformLocation(gl_shader_object(m_shader->cgl_ref()), "adjust");
                glUniform1f(loc, float( 2 ) );

                m_particles->bind();
                m_particles->apply_default_matrix_uniforms();
                m_particles->Modelmatrix(&trafo);
                m_particles->apply_default_tex_uniforms_and_bind_textures();

                vec3f l_dir = { 0.1, 0.1, -1 };
                vec3f l_col = { 1, 0, 0 };
                setup_light( m_particles->Shader(), &l_dir, &l_col, 1.0f );
            
                m_particles->draw_em();
                m_particles->unbind();

            }
            }

            m_shader->deactivate();

            ++which;
        }
    }

    // remove latest position
    while( m_carpositions.size() >= m_nCount + idxoffset ) {
        m_carpositions.erase( m_carpositions.begin() );
    }   

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
}

Fire::~Fire() {

}