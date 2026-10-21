#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>
#include <unordered_map>

#include "Scenes.hpp"
#include "Enums.hpp"

namespace Component {
	struct Position
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct Transform
	{
		double width = 0.0;
		double height = 0.0;
	};

	struct OriginOffsets
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct OriginPoint
	{
		Enum::OriginPoint data;
	};

	struct Button
	{
		bool enabled = true;
		double hoverAnimationDuration = 0.0;
		double idleAnimationDuration = 0.0;
		double clickedAnimationDuration = 0.0;
		double timer = 0.0;
		Enum::ButtonState state = Enum::ButtonState::IDLE;
	};

	struct Text
	{
		std::optional<sf::Text> box{};
		std::string string = "";
		int size = 12;
		sf::Color color = sf::Color::White;
		Enum::TextFormat format = Enum::TextFormat::CENTER;
	};

	struct NextScene
	{
		Scene next = Scene::MENU;
	};

	struct ZIndex
	{
		int index = 0;
		bool visible = true;
	};

	struct Velocity
	{
		double minX = 0.0;
		double minY = 0.0;
		double maxX = 0.0;
		double maxY = 0.0;
		double x = 0.0;
		double y = 0.0;
	};

	struct Speed
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct PlayerController
	{
		bool enabled = false;
	};

	struct Drag
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct Sprite
	{
		std::optional<sf::Sprite> body{};
	};

	struct Texture
	{
		Enum::Texture data;
	};

	struct Color
	{
		sf::Color data = sf::Color::White;
	};

	namespace Singleton
	{
		struct TexturesContainer
		{
			std::unordered_map<Enum::Texture, sf::Texture> map;
		};
	}
}

#endif