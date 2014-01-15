// Author: Bob Rubbens - Knights of the Compiler
// Creation date: di 07-01-2014
// Contact: http://plusminos.nl - @broervanlisa - gmail (bobrubbens)

#ifndef ANIMATEDSPRITE_HPP
#define ANIMATEDSPRITE_HPP

// Includes I didn't make myself
#include <queue>
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <tinyxml2.h>

 // Includes I did make myself

namespace nb {
	struct Animation {
		std::string name;
		// The collection of frames stored as 2d coordinates
		std::vector<sf::Vector2i> frames;
		// The time between the frames
		int dt;
		// The next animation that should be played
		std::string next;
	} ;

	class AnimatedSprite : public sf::Transformable, public sf::Drawable {
	public:
		// Loads an animated sprite using an external configuration file
		// Sets initial frame to the first frame
		AnimatedSprite(std::string config);
		~AnimatedSprite();

		// Sets the spritesheet to be used for rendering. Removes the previous spritesheet
		void setSpriteSheet(sf::Texture& inputSpritesheet);

		// Performs the logic of the animated sprite (mostly just updating the animation/frame)
		void logic(int dt);
		// Draws the animated sprite
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		// Starts a certain animation
		void play(std::string animation);
		// Resumes the animation if it was paused. Also resets the timer
		void resume();
		// Queues another animation. Once the current animation ends, this one is started
		void queue(std::string animation);
		// Pauses the current animation
		void pause();
		// Sets the sprite to a certain frame and pauses it
		void set(int frame);

		// Returns the name of the current animation
		std::string getAnimation();

	private:
		// To parse the XML file
		void handleXML(tinyxml2::XMLElement* e);
		// To parse an animation element
		std::vector<int> handleAnimation(tinyxml2::XMLElement* e);

		// To make update the view on the spritesheet
		void updateVertexes();

		// Contains the individual animations
		std::unordered_map<std::string, Animation> animations;
		// Contains the animations to be played
		std::queue<std::string> animationQueue;

		// The spritesheet 
		sf::Texture* spritesheet;
		bool deleteSpritesheetTexture;
		// The spritesheet location
		std::string spritesheetSource;
		// The size of the sprite
		sf::Vector2f spriteSize;
		// The amount of frames over the width & height of the spritesheet (The amount that fits in along the x & y axis)
		sf::Vector2f gridSize;

		// The current animation
		Animation currentAnimation;
		// The current frame
		int currentFrame;

		// Whether an animation is playing right now or not
		bool playing;
		// How much time has elapsed since the last frame switch. Milliseconds!
		int elapsedTime;

		// The part of the spritesheet which is rendered to the screen
		// sf::VertexArray part;
	};

}

#endif
