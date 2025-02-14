#ifndef GLOBALCONSTANT_H
#define GLOBALCONSTANT_H

// FEEL FREE TO CHANGE CONSTANTS TO WHATEVER YOU LIKE

namespace constant {
	
	/*
	*	
	*	SCREEN
	*
	*/ 
	const bool FULLSCREEN 					= true;

	// Auflösung der Schattentextur, 2048 ist ein besserer Wert!!!
	const int  SHADOW_RES					= 2048;

	const unsigned int SCREEN_WIDTH			= 1920;
	const unsigned int SCREEN_HEIGHT		= 1080;

	/*
	*
	*	PHYSIK
	*
	*/

	// Gravitation (Standard: -30)
	const float GRAVITY						= -30;

	/*
	*	Auto
	*/

	// Die Breite des Physikobjekts des Autos (Standard: 3)
	const float WIDTH_PHYSICAL_CAR			= 3;

	// Die Hoehe des Physikobjekts des Autos (Standard: 1.5)
	const float HEIGHT_PHYSICAL_CAR			= 1.5;

	// Die Masse des Autos (Standard: 100)
	const float MASS_OF_CAR					= 100;

	/*
	*	Drehung
	*/
	
	// Maximale Drehgeschwindigkeit des Autos (Standard: 9.5)
	const float MAX_ROTATING_SPEED				= 9.5;

	// Drehgeschwindigkeit, die man bei der Drehung pro Frame dazu oder abgezogen bekommt (Standard: 2.5)
	const float ADDED_ROTATING_VELOCITY			= 2.5;

	
	/*
	*	Gravitationserhoehung
	*/

	// Minimaler Abstand, den man vom Boden haben muss, um Gravitation zu erhoehen (Standard: 7.5)
	const float MIN_DIFF_FOR_PULLDOWN			= 7.5;

	// Maximale Gravitationserhoehung (Standard: 50)
	const float MAX_PULLDOWN_SPEED				= 50.0;

	// Vertikale Geschwindigkeit, die man bei der Gravitationserhoehung pro Frame dazubekommt (Standard: 2)
	const float ADDED_PULLDOWN_VELOCITY			= 2;

	
	/*
	*	Geschwindigkeiten
	*/

	// Minimale Gesamtgeschwindigkeit des Autos (Standard: 50)
	const float MIN_TOTAL_SPEED					= 50;

	// Minimale Horizontale Geschwindigkeit des Autos (Standard: 20)
	const float MIN_HORIZONTAL_SPEED			= 20;

	// Rate an horizontalem Geschwindigkeitsverlust in der Luft (Standard: 0.5)
	const float SLOWDOWN_RATE_IN_AIR			= 0.5;

	// Minimale Geschwindigkeit des Autos im Overdrive (Standard: 80)
	const float MIN_OVERDRIVE_SPEED				= 80;

	
	/*
	*	Einsammeln
	*/

	// Maximale Entfernung zwischen Auto und Muenze fuer Magnetwirkung (Standard: 5)
	const float DST_PLAYER_COIN					= 5.0;

	// Geschwindigkeit der Muenze zum Spieler waehrend der Magnetwirkung (Standard: 0.8)
	const float COIN_SPEED						= 0.8f;

	// Maximale Entfernung zwischen Auto und Muenze um diese aufzunehmen (Standard: 0.5)
	const float EAT_DISTANCE_COI				= 0.5f;

	// Maximale Entfernung zwischen Auto und Benzinfass fuer Magnetwirkung (Standard: 5)
	const float DST_PLAYER_BAR					= 5.0;

	// Geschwindigkeit des Benzinfasses zum Spieler waehrend der Magnetwirkung (Standard: 0.8)
	const float BARREL_SPEED					= 0.8f;

	// Maximale Entfernung zwischen Auto und Benzinfass um dieses aufzunehmen (Standard: 0.5)
	const float EAT_DISTANCE_BAR				= 0.5f;

	


	/*
	*	
	*	STRECKE
	*
	*/

	/*
	*	Streckeneigenschaften
	*/

	// Breite des Fahrstreifens (Standard: 16)
	const float GROUND_WIDTH					= 16.0;

	// Laenge der ebenen Startstrecke (Standard: 50)
	const float START_STRIP_LENGTH				= 50.0;

	// Glätte der Landschaft (Standard: 10) (Je kleiner, desto mehr Huegel)
	const float LANDSCAPE_SMOOTHNESS			= 10;

	
	/*
	*	Darstellung
	*/

	// Anzahl der Vertices der Strecke (Standard: 10000) (Je weniger, desto eckiger)
	const int VERTICES							= 10000;

	// Startkoordinaten der Strecke (Standard: 0)
	const float START_COORD						= 0.0;

	// Endkoordinaten der Strecke (Standard: 10000)
	const float END_COORD						= 10000.0;

	// Anzahl der Splines im Vordergrund (Standard: 5)
	const int FOREGROUND_SPLINES				= 5;

	// Anzahl der Splines im Hintergrund (Standard: 4)
	const int BACKGROUND_SPLINES				= 4;

	
	/*
	*	Splineberechnung
	*/

	// Anzahl Kontrollpunkte fuer die Splineerstellung (DO NOT CHANGE!)
	const int NUM_CONTROL_POINTS				= (int)((END_COORD - START_COORD) / LANDSCAPE_SMOOTHNESS) + 3;

	// Maximale Vertikale nach oben zwischen zwei Kontrollpunkten (Standard: 5)
	const float MAX_UP							= 5;

	// Maximale Vertikale nach unten zwischen zwei Kontrollpunkten (Standard: -10)
	const float MAX_DOWN 						= -10;

	


	/*
	*
	*	GAMEPLAY
	*
	*/

	/*
	*	Overdrive
	*/

	// Benoetigte Loopings, um in den Overdrive zu kommen (Standard: 4)
	const int LOOPINGS_NEEDED_FOR_OVERDRIVE		= 4;
	
	// Dauer eines Overdrives in Frames (Standard: 500)
	const int OVERDRIVE_FRAMES					= 500;

	
	/*
	*	Benzin
	*/

	// Menge an Benzin, mit der man startet (Standard : 100) (Anzeige geht bis 200)
	const float START_AMOUNT_GAS				= 100;

	// Benzinverbrauch (Standard: 0.2)
	const float GAS_CONSUMPTION					= 0.2;

	// Maximale Menge an Benzin, die man haben kann (Standard: 200) (Anzeige geht bis 200)
	const float MAX_AMOUNT_GAS					= 400;

	
	/*
	*	Pickups
	*/

	// Anzahl der Muenzen auf der Strecke (Standard: 300)
	const int NUMBER_OF_COINS					= 300;

	// Anzahl der Benzinfaesser auf der Strecke (Standard: 30)
	const int NUMBER_OF_BARRELS					= 30;

	// Anzahl der Bäume auf der Strecke (Standard: 350)
	const int NUMBER_OF_TREES					= 500;

	// Menge an Benzingewinn pro Looping (Standard: 4) (Erhoeht sich je nach Landung bis zum Fuenffachen)
	const int GAS_PER_LOOPING					= 4;

	// Menge an Benzingewinn pro eingesammelten Benzinfass (Standard: 50)
	const int GAS_PER_BARREL					= 50;

	
	/*
	*	Punkte
	*/

	// Punkte pro Looping (Standard: 8)
	const int POINTS_PER_LOOPING				= 8;

	// Punkte pro Muenze (Standard: 15)
	const int POINTS_PER_COIN					= 15;

	// Punkte pro Overdrive (Standard: 100)
	const int POINTS_PER_OVERDRIVE				= 100;

	/*
	*	Landung
	*/

	// Maximaler Winkel fuer eine perfekte Landung (Standard: 5)
	const float PERFECT_ANGLE					= 5;

	// Maximaler Winkel fuer eine gute Landung (Standard: 10)
	const float GOOD_ANGLE						= 10;

	// Maximaler Winkel fuer eine ausreichende Landung (Standard: 30)
	const float OK_ANGLE						= 30;
	
	


	/*
	*
	*	SONSTIGES
	*
	*/

	/*
	*	Anzeigen
	*/

	// Dauer, der Landeanzeige in Frames (Standard: 50)
	const int LANDING_FRAMES					= 50;

	// Vertikaler Schriftshakefaktor (Standard: 10) (Muss gerade sein)
	const int TEXT_SHAKE_HORI					= 6;

	// Horizontaler Schriftshakefaktor (Standard: 2) (Muss gerade sein)
	const int TEXT_SHAKE_VERT					= 8;

	
	/*
	*	Spielgeschwindigkeit
	*/

	// Adaptive Spielgeschwindigkeit, fuer langsamere PCs (Standard: true)
	const bool  ADAPTIVE_UPDATE					= true;

	// Updategeschwindigkeit der Physikengine (Standard: 0.5) (Achtung: Spielgeschwindigkeit aendert sich)
	const float UPDATE_INTERVALL				= 0.5;

	
	/*
	*	Kamera
	*/
	
	// Y-Position der Kamera (Standard: 5)
	const float CAMERA_POS_Y					= 5;

	// Z-Position der Kamera (Standard: 25)
	const float CAMERA_POS_Z					= 25;

	// Y-Kamerarichtung (Standard: -0.2)
	const float CAMERA_DIR_Y					= -0.2;

	
	/*
	*	Hintergrund
	*/

	// Rotanteil der Hintergrundfarbe (Standard: 61)
	const float BG_RED							= 61;
	
	// Gruenanteil der Hintergrundfarbe (Standard: 112)
	const float BG_GRN							= 112;
	
	// Blauanteil der Hintergrundfarbe (Standard: 172)
	const float BG_BLU							= 172;
	

	/*
	* 	Wolken
	*/

	// Wieviele Wolken werden erstellt?
	const int 	CLD_NUM							= 15;

	// Größe der verschiedenen Wolken
	const float CLD_SIZE_MIN					= 0.05;

	// Geschwindigkeit
	const float CLD_SPEED						= 0.0005;

	// Updateintervall für die Richtung (+ max. 500 weitere Frames)
	const int 	CLD_FRAMES						= 1000;
	const int 	CLD_FRAMES_DISTRIBUTION			= 500;

	// Untere Bildschirmbegrenzung für die Wolken
	const float CLD_SCREEN_BORDER				= 0.5;

	// Verteilung in X - Richtung: höher ist weniger verteilt
	const float	CLD_DIST_X						= 50;
	
	// Verteilung in Y - Richtung: niedriger ist stärker verteilt
	const float	CLD_DIST_Y						= 150;




}

#endif // GLOBALCONSTANT_H
