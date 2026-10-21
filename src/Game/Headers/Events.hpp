#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <SFML/Graphics.hpp>
#include "../src/Engine/NacreCoordinator.hpp"

#include <string>

#include "Components.hpp"
#include "Enums.hpp"

namespace Event
{
	struct SetSpriteOrigin
	{
		Entity target = NULL_ENTITY;
		Component::OriginOffsets& originOffsets;
		const Component::OriginPoint& originPoint;
	};
	struct SetColor
	{
		Entity target = NULL_ENTITY;
		const Component::Color& color;
	};
	struct LoadSprite
	{
		Entity target = NULL_ENTITY;
		Component::Sprite& sprite;
		const Component::Singleton::TexturesContainer& loadedTextures;
	};

	struct SetTextString
	{
		Entity target = NULL_ENTITY;
		const std::string& string;
		std::optional<sf::Text>& text;
	};
	struct SetTextSize
	{
		Entity target = NULL_ENTITY;
		const int size = 0;
		std::optional<sf::Text>& text;
	};
	struct SetTextColor
	{
		Entity target = NULL_ENTITY;
		const sf::Color& color;
		std::optional<sf::Text>& text;
	};
	struct SetTextOrigin
	{
		Entity target = NULL_ENTITY;
		Component::Text& text;
	};

	struct MouseClick
	{
		const sf::Vector2f pos;
	};
	struct MouseMoved
	{
		const sf::Vector2f pos;
	};

	struct PlayScene
	{
		const Scene next;
	};
}

#endif