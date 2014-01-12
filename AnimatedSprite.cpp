// Author: Bob Rubbens - Knights of the Compiler 
// Creation date: di 07-01-2014 
// Contact: http://plusminos.nl - @broervanlisa - gmail (bobrubbens) 

// Includes I didn't make myself 

 // Includes I did make myself 
#include "AnimatedSprite.hpp" 

// Loads an animated sprite using an external configuration file
// Sets initial frame to the first frame
nb::AnimatedSprite::AnimatedSprite(std::string config) :
	part(sf::Quads, 4) {
	animationName = "";
	currentFrame = 0;
	playing = false;
	elapsedTime = 0;

	// Handle XML
	//TODO//

}

nb::AnimatedSprite::~AnimatedSprite() {

}

// Sets the spritesheet to be used for rendering. Removes the previous spritesheet
void nb::AnimatedSprite::setSpriteSheet(sf::Texture* inputSpritesheet) {
	spritesheet = inputSpritesheet;
}

// Performs the logic of the animated sprite (mostly just updating the animation/frame)
void nb::AnimatedSprite::logic(float dt) {
	if (playing) {
		elapsedTime += dt;

		if (elapsedTime > animation.dt) {
			currentFrame++; // Next frame!
			elapsedTime -= animation.dt; // Time has passed
			
			// Handle next animation, or looping!
			if (currentFrame >= currentAnimation.frames.size()) {
				// Current animation ended!
				if (animationQueue.size() > 0) { // Check for any queued animations
					// An animation is queued
					// Also pop the front of the stack?
					std::string nextAnimation = animationQueue.front();
					// Error handling?
					play(nextAnimation);
				} else if (currentAnimation.next != "") { // Check if the animation has a heir
					// Well what'd you expect, the current animation has a preference!
					play(currentAnimation.next);
				} else {
					// We will just loop again, ok?
					currentFrame = 0;
				}
			}

			sf::Vector2i& pos = currentAnimation.frames.at(currentFrame) * spriteSize;
			part[0].texCoords = pos;
			part[1].texCoords = pos + sf::Vector2f(spriteSize.x, 0);
			part[2].texCoords = pos + spriteSize;
			part[3].texCoords = pos + sf::Vector2f(0, spriteSize.x);
		}
	}
}

// Draws the animated sprite
void nb::AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Change drawstate and draw that S.O.B.
	states.transform *= getTransform();
	states.texture = spritesheet;

	target.draw(part, states);
}

// Starts a certain animation
void nb::AnimatedSprite::play(std::string animation) {
	currentAnimation = animations.at(animation);
	elapsedTime = 0;
	animationName = animation;
	// Errorchecking?
}

// Resumes the animation if it was paused. Also resets the timer
void nb::AnimatedSprite::resume() {
	playing = true;
	elapsedTime = 0;
}

// Queues another animation. Once the current animation ends, this one is started
void nb::AnimatedSprite::queue(std::string animation) {
	animationQueue.push(animation);
}

// Pauses the current animation
void nb::AnimatedSprite::pause() {
	playing = false;
}

// Sets the sprite to a certain frame and pauses it
void nb::AnimatedSprite::set(int frame) {
	currentFrame = frame;
	playing = false;
}

// Returns the name of the current animation
std::string nb::AnimatedSprite::getAnimation() {
	return animationName;
}
