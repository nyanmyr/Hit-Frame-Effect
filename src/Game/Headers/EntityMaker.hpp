#ifndef ENTITY_MAKER_HPP
#define ENTITY_MAKER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include "../../Engine/Core.hpp"
#include "Scenes.hpp"
#include "Enums.hpp"

namespace Maker
{
	Entity player
	(
		const sf::Color col,
		const Enum::Layer layer,
		const sf::Vector2f pos,
		const sf::Vector2f size,
		const sf::Vector2f minVelocity,
		const sf::Vector2f maxVelocity,
		const sf::Vector2f speed,
		const sf::Vector2f drag,
		const Enum::Texture texture,
		const Enum::OriginPoint originPoint
	);
	Entity button
	(
		const int textSize,
		const Scene scene,
		const double hoverAnimationDuration,
		const double idleAnimationDuration,
		const double clickedAnimationDuration,
		const sf::Color col,
		const sf::Color textColor,
		const sf::Font& font,
		const Enum::Layer layer,
		const std::string str,
		const sf::Vector2f pos,
		const sf::Vector2f size,
		const Enum::TextFormat format,
		const Enum::OriginPoint originPoint,
		const Enum::Texture texture
	);
	Entity text
	(
		const int size,
		const sf::Font& font,
		const sf::Color col,
		const Enum::Layer layer,
		const std::string str,
		const sf::Vector2f pos,
		const Enum::TextFormat format
	);
}

#endif