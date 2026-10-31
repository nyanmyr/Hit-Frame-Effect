#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include <SFML/Graphics.hpp>
#include "../../Engine/Core.hpp"
#include "Components.hpp"
#include <queue>

namespace Start
{
	void setText();
	void setTextOrigin();
	void setSpriteOrigin();
	void loadTextures(Component::Singleton::TexturesContainer& loadedTextures);
	void loadSprites(const Component::Singleton::TexturesContainer& loadedTextures);
	void loadSounds(Component::Singleton::SoundsContainer& loadedSounds);
	void setColor();
}

namespace Control
{
	void doPlayerControl
	(
		const Entity player,
		const DeltaTime dt
	);
	void doDash(const Entity player);
}

namespace Update
{
	void doButtonTimer(const DeltaTime dt);
	void applyButtonHoverAnimation();
	void applyButtonIdleAnimation();
	void applyButtonClickedAnimation();
	void enableButtonNextScene();
	void move(const DeltaTime dt);
	void drag(const DeltaTime dt);
	void doDashTimer
	(
		const Entity player,
		const DeltaTime dt
	);
	void boundaryBounce
	(
		const double left,
		const double right,
		const double top,
		const double bottom
	);
	void hitEffect
	(
		int& skipFrame,
		Entity player,
		Component::Singleton::CameraShake& cameraShake,
		Component::Singleton::SoundsContainer& loadedSounds
	);
	void collision(DeltaTime dt);
	void shakeCamera
	(
		const DeltaTime dt,
		sf::RenderWindow& window,
		Component::Singleton::CameraShake& cameraShake
	);
}

namespace Render
{
	void doZIndex(std::queue<Entity>& renderQueue);
	void render
	(
		const int skipFrame,
		sf::Shader& shader,
		sf::Sprite& sceneSprite,
		sf::RenderWindow& window,
		sf::RenderTexture& renderTexture,
		std::queue<Entity>& renderQueue
	);
}

#endif
