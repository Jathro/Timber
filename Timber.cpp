#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

// function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPosition[NUM_BRANCHES];

const float TIME_TO_ADD = .15f;

int main() {

	VideoMode vm(1920, 1080);
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);
	Texture textureBackground;
	textureBackground.loadFromFile("graphics/background.png");
	Sprite spriteBackground;
	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);

	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	bool beeActive = false;
	float beeSpeed = 0.0f;

	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);

	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;

	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// time control
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 1000;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// track whether the game is running
	bool paused = true;

	//draw some text
	int score = 0;

	Text messageText;
	Text scoreText;

	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	// Make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	// prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	// Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		// set the sprite's origin to dead center
		// we can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}

	updateBranches(1);
	updateBranches(2);
	updateBranches(3);
	updateBranches(4);
	updateBranches(5);

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// Theplay starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 800);

	// Prepare the gravestone
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(730, 830);

	// Line up the axe with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1050;

	// Control the player input
	bool acceptInput = false;

	// prepare the sounds
	// the player chopping sound
	SoundBuffer chopbuffer;
	chopbuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopbuffer);

	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen()) {

		/*
		Handle the players input
		*/

		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for key presses again
				acceptInput = true;

				// hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				window.close();
			}

		// start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// reset the time and the score
			score = 0;
			timeRemaining = 6;

			// make all the branches disappear
			// starting in the second position
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPosition[i] = side::NONE;
			}

			// make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the player into position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;
		}

		// Another start game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and score
			score = 0;
			timeRemaining = 5;

			// make all the branches disappear
			for (int i = 0; i < NUM_BRANCHES; i++) {
				branchPosition[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the player into position
			spritePlayer.setPosition(675, 600);

			acceptInput = true;
		}

		//wrap the player controls to
		// make sure we are accepting input
		if (acceptInput) {
			//more code here
			// first handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				// Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + TIME_TO_ADD;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				// Update the branches
				updateBranches(score);

				// set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

			// handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				// Make sure the player is on the left
				playerSide = side::LEFT;
				score++;

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + TIME_TO_ADD;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				// Update the branches
				updateBranches(score);

				// set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;
				
				// Play a chop sound
				chop.play();
			}

		}

		/*
		Update the scene
		*/

		if (!paused) {

			// measure time
			Time dt = clock.restart();

			// subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {
				// pause the game
				paused = true;

				// change the message to the player
				messageText.setString("Out of time!!");

				// reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the out of time sund
				outOfTime.play();
			}

			// setup the bee
			if (!beeActive) {
				// how fast is the bee
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				// how high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else {
				spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

				// reset bee if it reaches edge of screen
				if (spriteBee.getPosition().x < -100) {
					// set it up ready to be a whole new bee next frams
					beeActive = false;
				}
			}

			// manage the clouds
			// cloud1

			if (!cloud1Active) {
				// how fast is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y);
				// has the cloud reach the edge of the screen
				if (spriteCloud1.getPosition().x > 1920) {
					// set it up ready to be a whole new cloud
					cloud1Active = false;
				}
			}

			// cloud2

			if (!cloud2Active) {
				// how fast is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				spriteCloud2.setPosition(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y);
				// has the cloud reach the edge of the screen
				if (spriteCloud2.getPosition().x > 1920) {
					// set it up ready to be a whole new cloud
					cloud2Active = false;
				}
			}
			// cloud3

			if (!cloud3Active) {
				// how fast is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y);
				// has the cloud reach the edge of the screen
				if (spriteCloud3.getPosition().x > 1920) {
					// set it up ready to be a whole new cloud
					cloud3Active = false;
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// update branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPosition[i] == side::LEFT) {
					// Move the sprite to the left
					branches[i].setPosition(610, height);

					// flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPosition[i] == side::RIGHT) {
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);

					// set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else {
					// hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// handle the flying log
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				//has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().y > 2000) {
					// Set it up ready to be a whole new log next framce
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPosition[5] == playerSide) {
				//death
				paused = true;
				acceptInput = false;

				//draw the gravestone
				spriteRIP.setPosition(525, 760);

				//hide the player
				spritePlayer.setPosition(2000, 660);

				// change the message text
				messageText.setString("Squished!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// play the death sound
				death.play();
			}

		} // end if (!paused)

		/*
		Draw the scene
		*/

		// clear everything from the last frame
		window.clear();

		//draw our game scene here
		window.draw(spriteBackground);

		// draw clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		// draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		// draw the tree
		window.draw(spriteTree);

		// draw the player
		window.draw(spritePlayer);

		// draw the axe
		window.draw(spriteAxe);

		// draw the flying log
		window.draw(spriteLog);

		// draw the gravestone
		window.draw(spriteRIP);

		// draw the bee
		window.draw(spriteBee);

		// draw the score
		window.draw(scoreText);

		// draw the timebar
		window.draw(timeBar);

		if (paused) {
			window.draw(messageText);
		}

		// show everything we just drew
		window.display();

	}

	return 0;
}

// Funciton definition
void updateBranches(int seed) {
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPosition[j] = branchPosition[j - 1];
	}

	// spawn a new branch at position 0
	// LEFT, RIGHT, NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		branchPosition[0] = side::LEFT;
		break;
	case 1:
		branchPosition[0] = side::RIGHT;
		break;
	case 2:
		branchPosition[0] = side::NONE;
		break;
	}
}