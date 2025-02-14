#include "splinemesh.h"

Spline center;
Spline first;
Spline second;
Spline foreground[constant::FOREGROUND_SPLINES];
Spline background[4*constant::BACKGROUND_SPLINES];

SplineMesh::SplineMesh( cpSpace* space ) {
    
	splineShader = new shader(find_shader("pos+norm+tc"));
	m_tex    = new texture( "grass", "grass.png");

	center = Spline();
	vec3f move = {45,0,0};
	center.moveSpline(move);
	spline_strip = center.createLineStrip(constant::VERTICES);
	move = {0,0,-constant::GROUND_WIDTH / 2};
	first = Spline(center, move);
	move = {0,0,constant::GROUND_WIDTH / 2};
	second = Spline(center, move);
	for (int i = 0; i < constant::FOREGROUND_SPLINES; i++) {
		move = {0, -pow(2, i), pow(2, i) + 2};
		foreground[i] = Spline(second,move);
	}
	for (int i = 0; i < constant::BACKGROUND_SPLINES; i++) {
		move = {0, pow(1.5, i), -pow(2, i)};
		// cout << move.y << " " << move.z << endl;
		background[i] = Spline(first,move);
	}
	for (int i = constant::BACKGROUND_SPLINES; i < 4*constant::BACKGROUND_SPLINES; i++) {
		move = {0, pow(1.5, constant::BACKGROUND_SPLINES - 1)-pow(1.6, i - constant::BACKGROUND_SPLINES), -pow(1.4, i) - 6};
		// cout << move.y << " " << move.z << endl;
		background[i] = Spline(first,move);
	}

	make_unit_matrix4x4f(&trafo);

	makeDrawable();

    // we only need a static body
    m_body  = cpBodyNewStatic();

    // collision
    addCollider( space );
}

void SplineMesh::addCollider( cpSpace* space ) {
	// create line colliders for every line on our spline
	vec3f a = spline_strip.at( 0 );
	vec3f b = { 0, 0 };
    for ( int i = 1; i < spline_strip.size(); ++i ) {
	 	
	 	b = spline_strip.at( i );

	 	// debug output
 		// cout << "-- Adding segment: " << a.x << ", " << a.y << " -> " << b.x << ", " << b.y << endl;
 		m_shape = cpSegmentShapeNew( m_body, cpv( a.x, a.y ), cpv( b.x, b.y ), 0.01 );
 		m_shape->group = 0;
		    
		// cpShapeSetFriction( m_shape, 0 );
		// cpShapeSetElasticity( m_shape, 0 );
		cpSpaceAddShape( space, m_shape );

	 	a = b;

	}
}


SplineMesh::~SplineMesh() {

}

Spline SplineMesh::getUsedSpline() {

	return center;

}

void SplineMesh::draw() {
	
	drawableSpline->bind();
    drawableSpline->apply_default_matrix_uniforms();
    drawableSpline->Modelmatrix(&trafo);
    drawableSpline->apply_default_tex_uniforms_and_bind_textures();
   
	vec3f dir = { 0, -1, 0 };
	vec3f col = { 0.6, 1, 0.2 };
	setup_light( drawableSpline->Shader(), &dir, &col, 1.0f );
    
    drawableSpline->draw_em();
    drawableSpline->unbind();

}

mesh_ref SplineMesh::makeLineMesh(Spline s) {

	mesh_ref mesh = make_mesh("splineLine", 1);
	vec3f vertices[constant::NUM_CONTROL_POINTS];
	for (int i = 0; i < constant::NUM_CONTROL_POINTS; i++) {
		vertices[i] = s.getControlPoints()[i];
	}
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vertices", GL_FLOAT, constant::NUM_CONTROL_POINTS, 3, vertices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;

}

void SplineMesh::makeTrianglesMesh(Spline s, Spline second) {

	vector<vec3f> smooth1 = s.createLineStrip(constant::VERTICES);
	vector<vec3f> smooth2 = second.createLineStrip(constant::VERTICES);
	
	m_spline = new mesh( "splineTriangles", stringlist( "in_pos", "in_norm", "in_tc" ) );

	float texFactor = sqrt(pow(smooth2[0].z - smooth1[0].z, 2) + pow(smooth2[0].y - smooth1[0].y, 2)) / 16;
	
	vec3f vertices[2*constant::VERTICES + 2];
	vec3f normals[2*constant::VERTICES + 2];
	vec3f tc[2*constant::VERTICES + 2];
	unsigned int indices[6*constant::VERTICES];
	for (int i = 0; i < constant::VERTICES + 1; i++) {
		vertices[2*i]     = smooth1[i];
		vertices[2*i + 1] = smooth2[i];

		normals[2*i]   = getNormalAt( smooth1[i].x );
		normals[2*i+1] = getNormalAt( smooth1[i].x );

		
		tc[2*i]     = {i*0.04,0};
		tc[2*i + 1] = {i*0.04,texFactor};
		
		
	}
	for (int i = 0; i < constant::VERTICES; i++) {
		indices[6*i] = 2*i;
		indices[6*i + 1] = 2*i + 1;
		indices[6*i + 2] = 2*i + 3;
		indices[6*i + 3] = 2*i;
		indices[6*i + 4] = 2*i + 3;
		indices[6*i + 5] = 2*i + 2;
	}
	
	m_spline->bind();
	m_spline->add_vertex_data( "in_pos", 2*constant::VERTICES + 2, vertices );
	m_spline->add_vertex_data( "in_norm", 2*constant::VERTICES + 2, normals );
	m_spline->add_vertex_data( "in_tc", 2*constant::VERTICES + 2, tc );
	m_spline->add_index_data( 6*constant::VERTICES, indices );
	m_spline->unbind();

}

void SplineMesh::makeDrawable() {

	drawableSpline = new drawelement("splineDrawable");
	drawableSpline->add_texture( "grass", m_tex );
	
	makeTrianglesMesh(first, second);
	drawableSpline->add_mesh(m_spline);

	for (int i = 0; i < constant::FOREGROUND_SPLINES; i++) {
		if (i == 0) {
			makeTrianglesMesh(second, foreground[i]);
		} else {
			makeTrianglesMesh(foreground[i - 1], foreground[i]);
		}
		drawableSpline->add_mesh(m_spline);
	}
	for (int i = 0; i < 4*constant::BACKGROUND_SPLINES; i++) {
		if (i == 0) {
			makeTrianglesMesh(background[i], first);
		} else {
			makeTrianglesMesh(background[i], background[i - 1]);
		}
		drawableSpline->add_mesh(m_spline);
	}
	
	drawableSpline->use_shader(shader_ptr(splineShader));

}