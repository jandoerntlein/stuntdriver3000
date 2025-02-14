#include "player.h"

Player::Player( cpSpace* space, SplineMesh *splineMesh) {
	
	outOfGas      = false;
	loopings      = 0;
	grounded      = false; 
	flying        = true;
	loopAngle     = 0;
	clockwise     = true;
	loopFlag      = false;
	explosionFlag = false;
	barrelFlag    = false;
	coins         = 0;

	overdrive = 0;
	overdrives = 0;
	overdriveFlag = false;
	pulldownFlag  = false;
	pulldownFlagRst = false;

	landingCounter = 0;
	landingGrade = 0;

	this->splineMesh = splineMesh;
	
	popup_decision = 0;

	makePhysics(space);
	

	// ccgl 
	ObjLoader loader("buggy", "../assets/buggy.obj");
	loader.TranslateToOrigin();
    loader.pos_and_norm_shader    = find_shader("pos+norm");
    loader.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
    loader.GenerateNonsharingMeshesAndDrawElements( buggy );

	
	// popups
	popup_bam   = new Popup( 0.5, "popup-bam.png" );
	popup_bang  = new Popup( 0.5, "popup-bang.png" );
	popup_pow   = new Popup( 0.5, "popup-pow.png" );
	popup_zap   = new Popup( 0.5, "popup-zap.png" );    

    // initial position
    make_unit_matrix4x4f( &trafo );

    // rotate 180 degrees
    vec3f axis = { 0, 1, 0 };
    matrix4x4f rot;
    make_rotation_matrix4x4f( &rot, &axis, M_PI );

    // apply
    multiply_matrices4x4f( &trafo, &rot, &trafo );

}

Player::~Player() {
	cpBodyFree( m_body );
}

void Player::setBack(cpSpace* space) {

	makePhysics(space);

	outOfGas      = false;
	loopings      = 0;
	grounded      = false; 
	flying        = true;
	loopAngle     = 0;
	clockwise     = true;
	loopFlag      = false;
	explosionFlag = false;
	barrelFlag    = false;
	coins         = 0;

	overdrive = 0;
	overdrives = 0;
	overdriveFlag = false;
	pulldownFlag  = false;
	pulldownFlagRst = false;

	landingCounter = 0;
	landingGrade = 0;

	// initial position
    make_unit_matrix4x4f( &trafo );

    // rotate 180 degrees
    vec3f axis = { 0, 1, 0 };
    matrix4x4f rot;
    make_rotation_matrix4x4f( &rot, &axis, M_PI );
    
    // apply
    multiply_matrices4x4f( &trafo, &rot, &trafo );

}

void Player::makePhysics(cpSpace* space) {

	this->space = space;

	m_moment    = cpMomentForBox( constant::MASS_OF_CAR, constant::WIDTH_PHYSICAL_CAR, constant::HEIGHT_PHYSICAL_CAR );
	m_body      = cpSpaceAddBody( this->space, cpBodyNew( constant::MASS_OF_CAR, m_moment ) );

	// Bounding Box
	cpVect chassisShape[5];
	chassisShape[0] = cpv(-1.4, -0);
	chassisShape[1] = cpv(-1.5, 0.8);
	chassisShape[2] = cpv(0.55, 0.8);
	chassisShape[3] = cpv(1.3, 0.4);
	chassisShape[4] = cpv(1.3, -0);

	cpVect move = cpv(0, 0.5);
	
	cpShape *chassis = cpPolyShapeNew(m_body, 5, chassisShape, move);
	chassis->group = CHASSIS;
	
	cpVect frontOffset  = cpv(1.58, -0.1 - 0.2);
	cpVect rearOffset   = cpv(-1.35, -0.1 - 0.2);
	
	frontWheel = cpCircleShapeNew(m_body, 0.55, frontOffset);
	rearWheel  = cpCircleShapeNew(m_body, 0.55, rearOffset);

	frontWheel->group = WHEEL;
	rearWheel->group = WHEEL;
	
	chassis->e = 0;
	frontWheel->e = 0.01;
	rearWheel->e = 0.01;

	cpSpaceAddShape(this->space, frontWheel);
	cpSpaceAddShape(this->space, rearWheel);
	cpSpaceAddShape(this->space, chassis);

	m_body->p.y = 10;

}

void Player::update() {
	
	// slow down when in the air
	if (flying && cpBodyGetVel(m_body).x > constant::MIN_HORIZONTAL_SPEED && overdrive == 0) {

		cpVect vel = cpv(cpBodyGetVel(m_body).x - constant::SLOWDOWN_RATE_IN_AIR, cpBodyGetVel(m_body).y);
		cpBodySetVel(m_body, vel);

	}

	// short impulse if we get too slow
	if( cpvlength(cpBodyGetVel(m_body)) < constant::MIN_TOTAL_SPEED && !outOfGas) {
		cpBodyApplyImpulse( m_body, cpv( 500, 0 ), cpvzero );
	}
	if (cpvlength(cpBodyGetVel(m_body)) < constant::MIN_OVERDRIVE_SPEED && overdrive > 0) {
    	cpBodyApplyImpulse( m_body, cpv( 500, 0 ), cpvzero );
    }
	
	// read current position
	cpVect  pos = cpBodyGetPos( m_body );
	
	trafo.row_col( 0, 3 ) = pos.x;
	trafo.row_col( 1, 3 ) = pos.y;   

    if (overdrive > 0) {
    	int wobble = (int)(sin((float)overdrive / 10.f) * 10);
    	text2d->draw(110 - 2*wobble,150 + wobble/2, "OVERDRIVE!", 50 + wobble);
    	overdrive--;
    } else if(overdrive == 0) {
    	overdriveFlag = false;
    }

    if (landingCounter > 0) {
    	// reset pulldown flag
	    if( pulldownFlag ) {
    		switch( popup_decision ) {
    			case 0: popup_bam->draw(); break;
    			case 1: popup_bang->draw(); break;
    			case 2: popup_zap->draw(); break;
    			case 3: popup_pow->draw(); break;
    		}  
    		pulldownFlagRst = true;  	
	    }

    	switch(landingGrade) {
    		case 5:
    			// Awesome Landing
    			text2d->draw(150,450, "AWESOME!", 40);
    			break;
    		case 4:
    			// Perfect Landing
    			text2d->draw(150,450, "PERFECT LANDING!", 30);
    			break;
    		case 3:
    			// Good Landing
    			text2d->draw(160,450, "GOOD LANDING!", 30);
    			break;
    		case 2:
    			// Ok Landing
    			text2d->draw(170,450, "OK LANDING!", 30);
    			break;
    		case 1:
    			// Bad Landing
    			text2d->draw(165,450, "BAD LANDING!", 30);
    			break;
    		default:
    			// no landing
    			break;
    	}
    	landingCounter--;
    } else {
    	// reset pulldown flag
    	if( pulldownFlagRst ) {
    		pulldownFlagRst = false;
    		pulldownFlag = false;
    	}
    }

    if (flying && loopings > 0) {
    	ostringstream out;
    	out << "Looping x" << loopings;
    	text2d->draw(350,350, out.str(), 20);
    }

    if (chassisTouchesGround()) {
    	
    	// EXPLOSION -> GAMEOVER
    	explosionFlag = true;
    	loopings = 0;

    	// fly out of window
    	cpBodyApplyImpulse( m_body, cpv( -5e3, 8e3 ), cpvzero );

    }
    else if (wheelTouchesGround()) {
    	
    	// Auto auf dem Boden
    	
    	if (!grounded) {
    		
    		// Winkel zwischen Auto und Boden
   			if (loopings > 0) {
   				
   				// Winkel von Auto zwischen -3.14 und 3.14
   				float carAngle = cpBodyGetAngle(m_body);
				while (carAngle > 0) carAngle -= (2*M_PI);
				while (carAngle < 0) carAngle += (2*M_PI);
				if (carAngle > M_PI) carAngle -= (2*M_PI);

				// Winkel von Boden zwischen -3.14 und 3.14
				vec3f tan = splineMesh->getUsedSpline().getTangetAtX(getX());
	    		float groundAngle = atan(tan.y / tan.x);

	    		// Winkel zwischen Auto und Boden in Grad
	    		float difAngle = abs(groundAngle - carAngle) * 180 / M_PI;

	    		landingCounter = constant::LANDING_FRAMES;
	    		if (difAngle < constant::PERFECT_ANGLE && loopings > 2 && pulldownFlag) {
	    			landingGrade = 5;
	    		} else if (difAngle < constant::PERFECT_ANGLE) {
	    			landingGrade = 4;
	    		} else if (difAngle < constant::GOOD_ANGLE) {
	    			landingGrade = 3;
	    		} else if (difAngle < constant::OK_ANGLE) {
	    			landingGrade = 2;
	    		} else {
	    			landingGrade = 1;
	    		}

   			}

    		grounded = true;
    		flying = false;
   			loopFlag = true;
   			if (loopings >= constant::LOOPINGS_NEEDED_FOR_OVERDRIVE && overdrive == 0 && landingGrade > 1) {
   				overdrive = constant::OVERDRIVE_FRAMES;
   				overdriveFlag = true;
   				overdrives++;
   			}

    	} else if (loopFlag) {
    		
    		loopFlag = false;
    		loopings = 0;

    	}

    	

    } else if (!flying) {
    	
    	// Auto hebt ab

    	// Auto hat nur fuer ein Frame den Boden beruehrt
    	if (loopFlag) {
    		
    		loopFlag = false;
    		loopings = 0;

    	}

    	grounded = false;
    	flying = true;
    	loopAngle = cpBodyGetAngle(m_body);

    } else {
    	
    	// Auto fliegt
    	
    	float actAngle = cpBodyGetAngle(m_body);
    	float angVel = cpBodyGetAngVel(m_body);

    	// Drehrichtung aendert sich
    	if ((angVel <= 0 && !clockwise) || (angVel > 0 && clockwise)) {
    		clockwise = !clockwise;
    		loopAngle = actAngle;
    	}

    	// Eine Umdrehung komplett
    	if (actAngle - loopAngle >= 2*M_PI || actAngle - loopAngle <= -2*M_PI) {
    		loopings++;
    		loopAngle = actAngle;
    	}

    }

}

void Player::draw( texture_ref depthtexture, matrix4x4f* tmatrix ) {
	
	cpFloat angle = cpBodyGetAngle( m_body );
	matrix4x4f rot;
	vec3f axis = { 0, 0, 1 };
    make_rotation_matrix4x4f( &rot, &axis, -angle );

	// draw
	for (vector<drawelement*>::iterator it = buggy.begin(); it != buggy.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms(trafo);
        de->Modelmatrix(&rot);
        de->apply_default_tex_uniforms_and_bind_textures();
    
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 0.9, 0.9, 0.6 };
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

    // popup: switch which texture is displayed
    // change this while we are flying because the popups only 
    // appear while we are grounded
    if( flying && landingCounter == 0 ) {
    	popup_decision = rand() % 4;
    }

}

void Player::draw() {
	
	cpFloat angle = cpBodyGetAngle( m_body );
	matrix4x4f rot;
	vec3f axis = { 0, 0, 1 };
    make_rotation_matrix4x4f( &rot, &axis, -angle );

	// draw
	for (vector<drawelement*>::iterator it = buggy.begin(); it != buggy.end(); ++it) {
        drawelement *de = *it;
        de->bind();
        de->apply_default_matrix_uniforms(trafo);
        de->Modelmatrix(&rot);
        de->apply_default_tex_uniforms_and_bind_textures();
    
        vec3f dir = { 0, 1, -1 };
        vec3f col = { 0.9, 0.9, 0.6 };
        setup_light( de->Shader(), &dir, &col, 1.0f );

        de->draw_em();
        de->unbind();
    }

}

void Player::rotateLeft() {

	float rotSpeed = cpBodyGetAngVel(m_body);
	if (rotSpeed < constant::MAX_ROTATING_SPEED) {
		cpBodySetAngVel(m_body, rotSpeed + constant::ADDED_ROTATING_VELOCITY);
	}

}

void Player::rotateRight() {

	float rotSpeed = cpBodyGetAngVel(m_body);
	if (cpBodyGetAngVel(m_body) > -constant::MAX_ROTATING_SPEED) {
		cpBodySetAngVel(m_body, rotSpeed - constant::ADDED_ROTATING_VELOCITY);
	}

}

void Player::setOutOfGas() {
	outOfGas = true;
}

bool Player::isOutOfGas() {
	return outOfGas;
}

float Player::getDistanceToGround() {
	return getY() - splineMesh->getUsedSpline().getY(getX());
}

void Player::pullDown() {

	if (m_body->v.y >= -constant::MAX_PULLDOWN_SPEED && getDistanceToGround() > constant::MIN_DIFF_FOR_PULLDOWN) {
		m_body->v.y -= constant::ADDED_PULLDOWN_VELOCITY;
		pulldownFlag = true;
	}

}

void Player::chassisToSpline(cpBody *body, cpArbiter *arb, bool *touch) {
	
	CP_ARBITER_GET_SHAPES(arb, car, spline);
	if (car->group == CHASSIS) *touch = true;

}

void Player::wheelToSpline(cpBody *body, cpArbiter *arb, bool *touch) {
	
	CP_ARBITER_GET_SHAPES(arb, car, spline);
	if (car->group == WHEEL) *touch = true;

}

bool Player::chassisTouchesGround() {

	bool touch = false;
	cpBodyEachArbiter(m_body, (cpBodyArbiterIteratorFunc)chassisToSpline, &touch);
	return touch;

}

bool Player::wheelTouchesGround() {

	bool touch = false;
	cpBodyEachArbiter(m_body, (cpBodyArbiterIteratorFunc)wheelToSpline, &touch);
	return touch;

}

bool Player::loopFlagSet() {
	return loopFlag;
}

bool Player::explosionFlagSet() {
	return explosionFlag;
}

bool Player::barrelFlagSet() {
	
	if (barrelFlag) {
		barrelFlag = !barrelFlag;
		return true;
	}
	return false;

}

int Player::getLoopings() {
	return loopings;
}

float Player::getDistanceTo(float x, float y) {
	return sqrt(pow(getX() - x, 2) + pow(getY() - y, 2));
}

int Player::getLandingGrade() {
	return landingGrade;
}