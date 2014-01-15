// Author: Bob Rubbens - Knights of the Compiler
// Creation date: 4-01-2014
// Contact: http://plusminos.nl - @broervanlisa - gmail (bobrubbens)

// Includes I didn't make myself
#include <SFML/Graphics.hpp>

// Includes I did make myself
#include "AnimatedSprite.hpp"

int main() {
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

	nb::AnimatedSprite as("AnimatedSpriteInfo.nbas");
	as.setPosition(200, 200);
	as.play("spinningswitching");

	while (window.isOpen()) { 
		// Handle events
		sf::Event e;
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}

		as.logic(1000/60);

		window.clear();

		window.draw(as);

		window.display();

		sf::sleep(sf::milliseconds(1000/60));
	}

	return 0;
}

