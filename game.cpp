#include "game.h"

Game::Game() {
	
	highscore = 0;
	highscorePoints = 0;
	frames = 0;
	fps  = 0;
	this->particles = 0;
	this->carstripes= 0;
	this->player    = 0;
	this->coin      = 0;
	this->barrel    = 0;
	wood 			= 0;
	this->splinemesh= 0;
	g_Space         = 0;
	gasoline 		= constant::START_AMOUNT_GAS;

	initFBO();
	newGame( true );
	

}

Game::~Game() {

}

void Game::newGame( bool newSpline ) {
	
	if (g_Space) cpSpaceFree(g_Space);

	// physics
    g_Space           = cpSpaceNew();
    cpVect cp_gravity = cpv( 0, constant::GRAVITY );                // gravity
	deltaTime         = constant::UPDATE_INTERVALL / 60.0;                   // update interval
    cpSpaceSetGravity( g_Space, cp_gravity );

    // create new ground if you wish
    if( newSpline ) { 
    	this->splinemesh = new SplineMesh( g_Space );    	
	} 
	// otherwise make sure that our new space object is assigned to the old ground
	else {
		this->splinemesh->addCollider( g_Space );
	}

	this->timer      = new Timer();
	countdown        = new Timer();
	
	if (this->player != 0) {
		this->player->setBack(g_Space);
	} else {
		this->player     = new Player(g_Space, this->splinemesh);
	}
	
	if (this->coin != 0) {
		this->coin->setBack(splinemesh, constant::NUMBER_OF_COINS);
	} else {
		this->coin       = new CoinContainer(this->player, splinemesh, constant::NUMBER_OF_COINS);
	}
	
	if (this->barrel != 0) {
		this->barrel->setBack(splinemesh, constant::NUMBER_OF_BARRELS);
	} else {
		this->barrel     = new BarrelContainer(this->player, splinemesh, constant::NUMBER_OF_BARRELS);
	}

	if (this->wood != 0) {
		this->wood->setBack(splinemesh, constant::NUMBER_OF_TREES );
	} else {
		this->wood       = new WoodContainer(this->player, splinemesh, constant::NUMBER_OF_TREES);
	}
	
	this->background = new Background(this->player);
	this->hud		 = new Hud();
	
	vec3f p_pos = { 0, 0, 0 };
    vec3f p_dir = { -1, .1, 0 };
    vec3f p_col = { 0.3, 0.3, 0.3 };
    this->particles = new Particles( p_pos, p_dir, p_col, 2000, 3.0, 0.4, 1.0, 1500, "explosion.png" );
    
    vec3f c_pos = { 0, 0, 0 };
    vec3f c_dir = { -1, .1, 0 };
    vec3f c_col = { 0.3, 0.3, 0.3 };
    this->carstripes  = new CarStripes( player, c_pos, c_dir, c_col, 100, 1.0, 0.4, 0.001, 1500, "carstripes.png" );
    this->overdrive   = new Fire( player, c_pos, c_dir, c_col, 100, 1.0, 0.4, 0.001, 1500, "explosion.png" );

    // make shadow
	player_shadow = new Shadow( player, m_depth );

	distance = 0.0;
	coins    = 0;
	loopings = 0;
	oldX     = 0.0;
	frame    = 0;
	distTime        = 0;
	distTimeTime = 0;
	distTimeDist = 0;

    if (highscore < getScore()) highscore = getScore();
    
    timer->start();
    countdown->start();

    gasoline = constant::START_AMOUNT_GAS;

}

void Game::initFBO() {

	m_fbo   = make_framebuffer( "screenbuffer", constant::SCREEN_WIDTH, constant::SCREEN_HEIGHT );

	if( !valid_framebuffer_ref( m_fbo ) ) {
        std::cout << "-- Framebuffer error occured" << std::endl;
        return;
    }
    bind_framebuffer( m_fbo );
    attach_depth_buffer( m_fbo );

    tex_params_t params = default_fbo_tex_params();
    params.wrap_s = GL_CLAMP_TO_EDGE;
    params.wrap_t = GL_CLAMP_TO_EDGE;

	m_depth = make_empty_texture("depthtexture", constant::SCREEN_WIDTH, constant::SCREEN_HEIGHT, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, GL_FLOAT, GL_DEPTH_COMPONENT, &params );
	m_splitLeft = make_empty_texture("splitLefttexture", constant::SCREEN_WIDTH, constant::SCREEN_HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, GL_FLOAT, GL_RGBA, &params );
	m_splitRight = make_empty_texture("splitRighttexture", constant::SCREEN_WIDTH, constant::SCREEN_HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, GL_FLOAT, GL_RGBA, &params );

	bind_texture( m_depth, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    unbind_texture( m_depth );
	bind_texture( m_splitLeft, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    unbind_texture( m_splitLeft );
    bind_texture( m_splitRight, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    unbind_texture( m_splitRight );

    attach_texture_as_depthbuffer( m_fbo, "depth", m_depth );
    attach_texture_as_colorbuffer( m_fbo, "left", m_splitLeft );
    attach_texture_as_colorbuffer( m_fbo, "right", m_splitRight );

    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, DrawBuffers);

    unbind_framebuffer( m_fbo );

}

void Game::addGas(float amount) {

	gasoline += amount;
	if(gasoline > constant::MAX_AMOUNT_GAS) gasoline = constant::MAX_AMOUNT_GAS;

}

void Game::subGas() {

	float distanceTmp = player->getX() - oldX;
	oldX = player->getX();
	gasoline -= constant::GAS_CONSUMPTION * distanceTmp;

}

void Game::render() {
		
	// render to shadow texture
	player_shadow->begin(m_fbo);
		if( !player->explosionFlagSet() && gasoline > 0 ) player->draw();
		coin->draw();
		barrel->draw();
		wood->draw();
	player_shadow->end(m_fbo);
	
	bind_framebuffer( m_fbo );
	background->draw();
	wood->draw(player_shadow->depthtexture(), player_shadow->T());
	barrel->draw( player_shadow->depthtexture(), player_shadow->T() );
	coin->draw( player_shadow->depthtexture(), player_shadow->T() );
	splinemesh->draw();
	hud->draw( gasoline / 100.0f );
	unbind_framebuffer( m_fbo );

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_FRONT_AND_BACK);
	glBlitFramebuffer(0,0,1920,1080,0,0,1920,1080,GL_COLOR_BUFFER_BIT,GL_LINEAR);

}

void Game::update() {
	
	frames++;
    if( timer->look() > 1000 ) {
        timer->start();
        fps = frames;
        frames = 0;
    }

	render();

	if (!player->explosionFlagSet()) {
		
		// explosion position
        vec3f car = { player->getX(), player->getY(), 0 };
        particles->updatePos( car );

        // draw only after start
		if( countdown->look() > 3000 && !player->overdriveFlagSet() ) {	
	        bind_framebuffer( m_fbo );
	        carstripes->render();
	        unbind_framebuffer( m_fbo );
    	}

    	if( player->overdriveFlagSet() ) {
    		bind_framebuffer( m_fbo );
    		overdrive->render();
    		unbind_framebuffer( m_fbo );
    	} 

		bind_framebuffer( m_fbo );
		player->update();
		unbind_framebuffer( m_fbo );

	}
	

	if (player->loopFlagSet() && !player->isOutOfGas()) {
		loopings += player->getLoopings();
		addGas(player->getLoopings() * constant::GAS_PER_LOOPING * player->getLandingGrade());
	}

	if (player->barrelFlagSet() && !player->isOutOfGas()) {
		addGas(constant::GAS_PER_BARREL);
	}

	if (player->explosionFlagSet()) {
		bind_framebuffer( m_fbo );
		text2d->draw(170,350, "BOOOOM", 60);
		unbind_framebuffer( m_fbo );
		gasoline = -1;
		particles->setExplosionMode();
		bind_framebuffer( m_fbo );
        particles->draw(); 
        unbind_framebuffer( m_fbo );
	} else {
		// render to screen
		bind_framebuffer( m_fbo );
		player->draw( player_shadow->depthtexture(), player_shadow->T() );
		unbind_framebuffer( m_fbo );
	}

	if (!player->overdriveFlagSet() && !player->isOutOfGas()) {
		subGas();
	} else if (!player->isOutOfGas()) {
		oldX = player->getX();
	}
	
	if (gasoline < 0) {

		//GAMEOVER
		player->setOutOfGas();
		ostringstream out,out2;
		showScore();

		if (calculateHighscorePoints() > this->highscorePoints) this->highscorePoints = calculateHighscorePoints();

		out2 << "Highscore: " << this->highscorePoints << " Points";
		
		bind_framebuffer( m_fbo );
		text2d->draw(100,180, out2.str(), 30);
		unbind_framebuffer( m_fbo );

	}

	// if we get lower fps, we increase the update speed of the physics
	if( constant::ADAPTIVE_UPDATE && fps > 25 ) {
		deltaTime = constant::UPDATE_INTERVALL / fps;		
	}
		
	// initial countdown
	if( countdown->look() < 1000 ) {
		bind_framebuffer( m_fbo );
    	text2d->draw(150,300, "READY...", 60);
    	unbind_framebuffer( m_fbo );
    }
    else if( countdown->look() < 2000 ) {
    	bind_framebuffer( m_fbo );
    	text2d->draw(150,300, " SET...", 60);    
    	unbind_framebuffer( m_fbo );
    }
    else if( countdown->look() < 3000 ) {
    	bind_framebuffer( m_fbo );
        text2d->draw(150,300, "  GO!", 60);
        unbind_framebuffer( m_fbo );
    } 
    else { 
		// update physics
	    cpSpaceStep( g_Space, deltaTime );
	}

	// game instructions
	if ( countdown->look() < 3000 ) {
		bind_framebuffer( m_fbo );
		text2d->draw(80,120, "Use A and D to tilt", 30);
		unbind_framebuffer( m_fbo );
		bind_framebuffer( m_fbo );
		text2d->draw(85,80, "Use S to slam down", 30);
		unbind_framebuffer( m_fbo );
	}

	showDistAndPoints();

	if ((int)player->getX() % 500 == 0 && (int)player->getX() > 1) {
		
		distTime = 500;
		distTimeDist = (int)(player->getX() / 500) * 500;
		distTimeTime = (int)((countdown->look() - 3000) / 1000);

		int sek = distTimeTime % 60;
		int min = (distTimeTime - sek) / 60;
		distTimeOut.str("");
		distTimeOut << "Time for " << distTimeDist << " meters: " << min << ":" << sek << " min!";
		
	}

	if (distTime > 0) {
		bind_framebuffer( m_fbo );
		text2d->draw(2*distTime - 400,200,distTimeOut.str(),20);
		unbind_framebuffer( m_fbo );
		distTime--;
	}
	
}

int Game::getScore() {
	return this->highscore;
}

CoinContainer* Game::getCoins() {
    return this->coin;
}

int Game::calculateHighscorePoints() {

	coins = player->getCoins();
	overdrives = player->getOverdrives();
	return loopings * constant::POINTS_PER_LOOPING + coins * constant::POINTS_PER_COIN + (int)oldX + overdrives * constant::POINTS_PER_OVERDRIVE;

}

void Game::showScore() {

	ostringstream out1, out2, out3, out4, out5;

	if (loopings < 1000 && loopings > 99) {
		out1 << "Loopings     " << loopings << endl;
	} else if (loopings < 100 && loopings > 9) {
		out1 << "Loopings      " << loopings << endl;
	} else if (loopings < 10) {
		out1 << "Loopings       " << loopings << endl;
	} else {
		out1 << "Loopings    " << loopings << endl;
	}

	if (coins < 1000 && coins > 99) {
		out2 << "Coins        " << coins << endl;
	} else if (coins < 100 && coins > 9) {
		out2 << "Coins         " << coins << endl;
	} else if (coins < 10) {
		out2 << "Coins          " << coins << endl;
	} else {
		out2 << "Coins       " << coins << endl;
	}

	if (overdrives < 1000 && overdrives > 99) {
		out3 << "Overdrives   " << overdrives << endl;
	} else if (overdrives < 100 && overdrives > 9) {
		out3 << "Overdrives    " << overdrives << endl;
	} else if (overdrives < 10) {
		out3 << "Overdrives     " << overdrives << endl;
	} else {
		out3 << "Overdrives  " << overdrives << endl;
	}

	int dist = (int)oldX;
	if (dist < 1000 && dist > 99) {
		out4 << "Distance     " << dist << endl;
	} else if (dist < 100 && dist > 9) {
		out4 << "Distance      " << dist << endl;
	} else if (dist < 10) {
		out4 << "Distance       " << dist << endl;
	} else {
		out4 << "Distance    " << dist << endl;
	}

	int score = calculateHighscorePoints();
	if (score < 1000 && score > 99) {
		out5 << "Score        " << score << endl;
	} else if (score < 100 && score > 9) {
		out5 << "Score         " << score << endl;
	} else if (score < 10) {
		out5 << "Score          " << score << endl;
	} else {
		out5 << "Score       " << score << endl;
	}
	
	bind_framebuffer( m_fbo );
	text2d->draw(150,300,out1.str(), 30);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(150,280,out2.str(), 30);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(150,260,out3.str(), 30);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(150,240,out4.str(), 30);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(150,220,out5.str(), 30);
	unbind_framebuffer( m_fbo );
	
	bind_framebuffer( m_fbo );
	text2d->draw(40,100,"Press N to restart", 30);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(40,80,"Press M for a new track", 30);
	unbind_framebuffer( m_fbo );
	
}

void Game::showDistAndPoints() {

	ostringstream out1,out2;

	int distance = (int)player->getX();
	if (distance < 10000 && distance > 999) {
		out1 << "Distance   " << distance << endl;
	} else if (distance < 1000 && distance > 99) {
		out1 << "Distance    " << distance << endl;
	} else if (distance < 100 && distance > 9) {
		out1 << "Distance     " << distance << endl;
	} else if (distance < 10) {
		out1 << "Distance      " << distance << endl;
	} else {
		out1 << "Distance  " << distance << endl;
	}

	int coins = player->getCoins();
	if (coins < 10000 && coins > 999) {
		out2 << "Coins      " << coins << endl;
	} else if (coins < 1000 && coins > 99) {
		out2 << "Coins       " << coins << endl;
	} else if (coins < 100 && coins > 9) {
		out2 << "Coins        " << coins << endl;
	} else  if (coins < 10) {
		out2 << "Coins         " << coins << endl;
	} else {
		out2 << "Coins     " << coins << endl;
	}

	bind_framebuffer( m_fbo );
	text2d->draw(440,580, out1.str(), 15);
	unbind_framebuffer( m_fbo );
	bind_framebuffer( m_fbo );
	text2d->draw(440,560, out2.str(), 15);
	unbind_framebuffer( m_fbo );

}