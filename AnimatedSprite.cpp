// Author: Bob Rubbens - Knights of the Compiler 
// Creation date: di 07-01-2014 
// Contact: http://plusminos.nl - @broervanlisa - gmail (bobrubbens) 

// Includes I didn't make myself 
#include <iostream>
#include <sstream>
#include <string>

 // Includes I did make myself 
#include "AnimatedSprite.hpp" 

namespace nb {
	void disp(std::string in) {
		std::cout << in << "\n" << "\n";
	}

	void disp(sf::Vector2f in) {
		std::stringstream ss;
		ss << "[vector2]" << "(" << in.x << ", " << in.y << ")\n";
		std::cout << ss.str() << "\n";
	}

	void disp(sf::Vector2i in) {
		disp(sf::Vector2f(in));
	}

	void disp(Animation in) {
		std::stringstream ss;
		ss << "[animation]\n";
		ss << "name: " << in.name << "\n";
		ss << "next: " << (in.next == "" ? "<EMPTY>" : in.next) << "\n";
		ss << "delay: " << in.dt << "\n";
		ss << "[frames]\n";
		std::cout << ss.str();
		for (auto i : in.frames) {
			disp(i);
		}
		std::cout << "\n";
	}

	// Loads an animated sprite using an external configuration file
	// Sets initial frame to the first frame
	AnimatedSprite::AnimatedSprite(std::string config) {
		currentFrame = 0;
		playing = false;
		elapsedTime = 0;
		spritesheetSource = "";
		deleteSpritesheetTexture = false;

		// Handle XML
		tinyxml2::XMLDocument doc;
		doc.LoadFile(config.c_str());
		// It iterates over all the XML elements using recursion
		handleXML(doc.RootElement());

		// If a source texture was given in the config, load it
		if (spritesheetSource != "") {
			deleteSpritesheetTexture = true;
			spritesheet = new sf::Texture();
			spritesheet->loadFromFile(spritesheetSource);
		}

		// List all XML properties
		// disp("spritesheetSource");
		// disp(spritesheetSource);

		// disp("spriteSize");
		// disp(spriteSize);
		
		// disp("origin");
		// disp(getOrigin());

		// disp("gridSize");
		// disp(gridSize);

		// disp("spinning2", animations.at("spinning2"));
		int p;
		for (auto& i : animations) {
			for (sf::Vector2i& v : i.second.frames) {
				p = v.x;
				v = sf::Vector2i(p % (int)gridSize.x, p / (int)gridSize.x);
			}

			// disp(i.second);
		}
	}

	void AnimatedSprite::handleXML(tinyxml2::XMLElement* e) {
		// Extract the element type
		const char* val = e->Value();
		std::string elemType(val);

		// Parse the info
		if (elemType == "spritesheet") { // The root element
			handleXML(e->FirstChildElement());
		} else if (elemType == "src") { // Source (spritesheet file) element
			tinyxml2::XMLText* textNode = e->FirstChild()->ToText();
			spritesheetSource = textNode->Value();
		} else if (elemType == "origin") { // Origin of the sprite
			int x, y;
			e->QueryIntAttribute("x", &x);
			e->QueryIntAttribute("y", &y);
			setOrigin(x, y);
		} else if (elemType == "size") { // Size of the sprite
			int x, y;
			e->QueryIntAttribute("x", &x);
			e->QueryIntAttribute("y", &y);
			spriteSize = sf::Vector2f(x, y);
		} else if (elemType == "gridsize") { // How big the grid with all the frames is
			int x, y;
			e->QueryIntAttribute("x", &x);
			e->QueryIntAttribute("y", &y);
			gridSize = sf::Vector2f(x, y);
		} else if (elemType == "animation") { 	// An animation element. An animation element consists of more frame elements, so with
												// another recursive function it recurses over all those elements and returns a vector
												// with the frame numbers. The frame numbers are linear (1...n instead of x,y...maxX,maxY)
												// and will be converted after the whole XML file is interpreted.
			std::vector<int> frames = handleAnimation(e->FirstChildElement());
			std::vector<sf::Vector2i> newFrames;
			for (uint32_t i = 0; i < frames.size(); ++i) {
				int p = frames[i];
				newFrames.push_back(sf::Vector2i(p, p));
			}

			// Extract the rest of the information about this particular animation and put it in the animation unordered map.
			Animation animation;
			animation.name = e->Attribute("name");
			e->QueryIntAttribute("delay", &animation.dt);
			animation.next = std::string(e->Attribute("nextanimation") == NULL ? "" : e->Attribute("nextanimation"));
			animation.frames = newFrames;

			animations.emplace(animation.name, animation);
		}

		// Continue parsing the next element (WHY DOES THIS WORK)
		if (e->NextSiblingElement()) {
			handleXML(e->NextSiblingElement());
		}
	}

	std::vector<int> AnimatedSprite::handleAnimation(tinyxml2::XMLElement* e) {
		// Vector to store the frame(s) in
		std::vector<int> frames;

		// Extract the element type
		const char* val = e->Value();
		std::string elemType(val);

		if (elemType == "frames") { // Sequence type (start...end)
			int start, end;
			e->QueryIntAttribute("start", &start);
			e->QueryIntAttribute("end", &end);

			// Iterate from start to end and add all index inbetween to the vector
			for (int i = start; i <= end; i++) {
				frames.push_back(i);
			}
		} else if (elemType == "f") { // Single frame (n)
			// Get the frame index and convert it to int
			// WHY DOESN'T MINGW HAVE STOI (C++11) ENABLED
			std::string contents(e->GetText());
			int val;
			std::istringstream ss(contents);
			ss >> val;
			frames.push_back(val);
			// WHAT THE HELL MINGW
		}

		// Concatenate the current frame vector with that of the next.
		// Not that pretty but it works (and it's recursive!)
		if (e->NextSiblingElement()) {
			std::vector<int> nextFrames = handleAnimation(e->NextSiblingElement());
			frames.insert(frames.end(), nextFrames.begin(), nextFrames.end());
		}

		return frames;
	}

	AnimatedSprite::~AnimatedSprite() {
		if (deleteSpritesheetTexture) {
			delete spritesheet;
		}
	}

	// Sets the spritesheet to be used for rendering. Works only if spritesheet is unset
	void AnimatedSprite::setSpriteSheet(sf::Texture& inputSpritesheet) {
		if (!deleteSpritesheetTexture) {
			spritesheet = &inputSpritesheet;
		}
	}

	// Performs the logic of the animated sprite (mostly just updating the animation/frame)
	void AnimatedSprite::logic(int dt) {
		if (playing) {
			elapsedTime += dt;

			if (elapsedTime > currentAnimation.dt) {
				currentFrame++; // Next frame!
				elapsedTime -= currentAnimation.dt; // Time has passed
				
				// Handle next animation, or looping!
				if ((uint32_t)currentFrame >= currentAnimation.frames.size()) {
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

				// updateVertexes();
			}
		}
	}

	// Draws the animated sprite
	void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::VertexArray part(sf::Quads, 4);
		sf::Vector2f pos = sf::Vector2f(currentAnimation.frames.at(currentFrame));
		pos.x *= spriteSize.x;
		pos.y *= spriteSize.y;

		// Set position on texture
		part[0].texCoords = pos;
		part[1].texCoords = pos + sf::Vector2f(spriteSize.x, 0);
		part[2].texCoords = pos + spriteSize;
		part[3].texCoords = pos + sf::Vector2f(0, spriteSize.y);

		// Set position on screen;
		disp(getPosition());
		part[0].position = sf::Vector2f(0, 0);
		part[1].position = sf::Vector2f(spriteSize.x, 0);
		part[2].position = spriteSize;
		part[3].position = sf::Vector2f(0, spriteSize.y);

		// Change drawstate and draw that S.O.B.
		states.transform *= getTransform();
		states.texture = spritesheet;
		target.draw(part, states);
	}

	// Starts a certain animation
	void AnimatedSprite::play(std::string animation) {
		currentAnimation = animations.at(animation);
		elapsedTime = 0;
		currentFrame = 0;
		playing = true;
		// updateVertexes();
		// Errorchecking?
	}

	// Resumes the animation if it was paused. Also resets the timer
	void AnimatedSprite::resume() {
		playing = true;
		elapsedTime = 0;
	}

	// Queues another animation. Once the current animation ends, this one is started
	void AnimatedSprite::queue(std::string animation) {
		animationQueue.push(animation);
	}

	// Pauses the current animation
	void AnimatedSprite::pause() {
		playing = false;
	}

	// Sets the sprite to a certain frame and pauses it
	void AnimatedSprite::set(int frame) {
		currentFrame = frame;
		playing = false;
		// updateVertexes();
	}

	// Returns the name of the current animation
	std::string AnimatedSprite::getAnimation() {
		return currentAnimation.name;
	}

	// To make update the view on the spritesheet
	void AnimatedSprite::updateVertexes() {
		// Deprecated
	}
}