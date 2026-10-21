#include "Headers/EntityMaker.hpp"

#include <SFML/Graphics.hpp>
#include "../Engine/NacreCoordinator.hpp"
#include "Headers/Components.hpp"
#include "Headers/Enums.hpp"

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
	)
	{
		Entity entity = nc().createEntity();

		nc().addComponent
		(
			entity,
			Component::Position
			{
				pos.x,
				pos.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::ZIndex{ static_cast<int>(layer) }
		);
		nc().addComponent
		(
			entity,
			Component::OriginOffsets{}
		);
		nc().addComponent
		(
			entity,
			Component::OriginPoint{ originPoint }
		);
		nc().addComponent
		(
			entity,
			Component::Velocity
			{
				minVelocity.x,
				minVelocity.y,
				maxVelocity.x,
				maxVelocity.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::Speed
			{
				speed.x,
				speed.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::Drag
			{
				drag.x,
				drag.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::PlayerController
			{
				true
			}
		);
		nc().addComponent
		(
			entity,
			Component::Transform
			{
				size.x,
				size.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::Texture{ texture }
		);
		nc().addComponent
		(
			entity,
			Component::Sprite{}
		);

		nc().addComponent
		(
			entity,
			Component::Color{ col }
		);

		return entity;
	}

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
	)
	{
		Entity entity = nc().createEntity();

		nc().addComponent(
			entity,
			Component::Position
			{
				pos.x,
				pos.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::Transform
			{
				size.x,
				size.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::OriginOffsets{}
		);
		nc().addComponent
		(
			entity,
			Component::OriginPoint{ originPoint }
		);
		nc().addComponent
		(
			entity,
			Component::Button
			{
				true,
				hoverAnimationDuration,
				idleAnimationDuration,
				clickedAnimationDuration
			}
		);

		nc().addComponent
		(
			entity,
			Component::Text
			{
				sf::Text(font),
				str,
				textSize,
				textColor,
				format
			}
		);
		nc().addComponent
		(
			entity,
			Component::NextScene
			{
				scene
			}
		);
		nc().addComponent
		(
			entity,
			Component::ZIndex{ static_cast<int>(layer) }
		);
		nc().addComponent
		(
			entity,
			Component::Transform
			{
				size.x,
				size.y
			}
		);
		nc().addComponent
		(
			entity,
			Component::Texture{ texture }
		);
		nc().addComponent
		(
			entity,
			Component::Sprite{}
		);

		nc().addComponent
		(
			entity,
			Component::Color{ col }
		);

		return entity;
	}

	Entity text
	(
		const int size,
		const sf::Font& font,
		const sf::Color col,
		const Enum::Layer layer,
		const std::string str,
		const sf::Vector2f pos,
		const Enum::TextFormat format
	)
	{
		Entity entity = nc().createEntity();

		nc().addComponent(
			entity,
			Component::Position
			{
				pos.x,
				pos.y
			}
		);

		nc().addComponent
		(
			entity,
			Component::Text
			{
				sf::Text(font),
				str,
				size,
				col,
				format
			}
		);

		nc().addComponent
		(
			entity,
			Component::ZIndex{ static_cast<int>(layer) }
		);

		nc().addComponent // unused but required
		(
			entity,
			Component::Transform{}
		);

		return entity;
	}
}
