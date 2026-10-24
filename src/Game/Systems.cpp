#include "Headers/Systems.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <utility>
#include <vector>
#include <stdexcept>

#include "../Engine/NacreCoordinator.hpp"
#include "Headers/Components.hpp"
#include "Headers/Enums.hpp"
#include "Headers/Events.hpp"

// -------------------------------------------------------
// start systems
// -------------------------------------------------------
void Start::setText()
{
	auto& textArray = nc().getComponentArray<Component::Text>();

	for (auto& [entity, text] : textArray->getAll())
	{
		nc().call<Event::SetTextString>
			(
				Event::SetTextString
				{ entity, text.string, text.box }
			);
		nc().call<Event::SetTextSize>
			(
				Event::SetTextSize
				{ entity, text.size, text.box }
			);
		nc().call<Event::SetTextColor>
			(
				Event::SetTextColor
				{ entity, text.color, text.box }
			);
	}
}
void Start::setTextOrigin()
{
	auto& textArray = nc().getComponentArray<Component::Text>();

	for (auto& [entity, text] : textArray->getAll())
	{
		nc().call<Event::SetTextOrigin>
			(
				Event::SetTextOrigin
				{ entity, text }
			);
	}
}
void Start::setSpriteOrigin()
{
	auto& originOffsetsArray = nc().getComponentArray<Component::OriginOffsets>();
	auto& originPointArray = nc().getComponentArray<Component::OriginPoint>();

	for (auto& [entity, origin] : originOffsetsArray->getAll())
	{
		if (!originPointArray->hasData(entity)) continue;

		auto& originPoint = originPointArray->getData(entity);

		nc().call<Event::SetSpriteOrigin>
			(
				Event::SetSpriteOrigin
				{ entity, origin, originPoint }
			);
	}
}
void Start::loadTextures(Component::Singleton::TexturesContainer& loadedTextures)
{
	loadedTextures.map.emplace(Enum::Texture::TEXTURE_PLACEHOLDER, sf::Texture(SPRITES_PATH "placeholder_texture.jpeg"));
}
void Start::loadSprites(const Component::Singleton::TexturesContainer& loadedTextures)
{
	auto& spriteArray = nc().getComponentArray<Component::Sprite>();

	for (auto& [entity, sprite] : spriteArray->getAll())
	{
		nc().call<Event::LoadSprite>
			(
				Event::LoadSprite
				{ entity, sprite, loadedTextures }
			);
	}
}
void Start::setColor()
{
	auto& colorArray = nc().getComponentArray<Component::Color>();

	for (auto& [entity, color] : colorArray->getAll())
	{
		nc().call<Event::SetColor>
			(
				Event::SetColor
				{ entity, color }
			);
	}
}

// -------------------------------------------------------
// control systems
// -------------------------------------------------------
void Control::doPlayerControl
(
	const Entity player,
	const DeltaTime dt
)
{
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& speedArray = nc().getComponentArray<Component::Speed>();
	auto& playerControllerArray = nc().getComponentArray<Component::PlayerController>();
	auto& dashArray = nc().getComponentArray<Component::Dash>();

	if (!velocityArray->hasData(player) ||
		!speedArray->hasData(player) ||
		!playerControllerArray->hasData(player) ||
		!dashArray->hasData(player))
	{
		return;
	}

	auto& velocity = velocityArray->getData(player);
	const auto& dash = dashArray->getData(player);
	const auto& playerController = playerControllerArray->getData(player);
	const auto& speed = speedArray->getData(player);

	if (!playerController.enabled)
	{
		return;
	}

	double newSpeedX = 0.0;
	double newSpeedY = 0.0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		newSpeedY = -speed.y;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		newSpeedY = speed.y;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		newSpeedX = -speed.x;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		newSpeedX = speed.x;
	}

	// applies the speed (even if there aren't any changes)
	velocity.x += (newSpeedX * dt);
	velocity.y += (newSpeedY * dt);

	if (dash.time > 0.0)
	{
		return;
	}

	if (velocity.x > velocity.maxX)
	{
		velocity.x = velocity.maxX;
	}
	else if (velocity.x < velocity.minX)
	{
		velocity.x = velocity.minX;
	}

	if (velocity.y > velocity.maxY)
	{
		velocity.y = velocity.maxY;
	}
	else if (velocity.y < velocity.minY)
	{
		velocity.y = velocity.minY;
	}

}
void Control::doDash(const Entity player)
{
	auto& dashArray = nc().getComponentArray<Component::Dash>();
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& speedArray = nc().getComponentArray<Component::Speed>();

	if (!dashArray->hasData(player) ||
		!velocityArray->hasData(player) ||
		!speedArray->hasData(player))
	{
		return;
	}

	auto& dash = dashArray->getData(player);
	auto& velocity = velocityArray->getData(player);
	const auto& speed = speedArray->getData(player);

	if (dash.time > 0.0)
	{
		return;
	}

	nc().call<Event::Dash>
		(
			Event::Dash
			{
				dash,
				velocity
			}
		);
}

// -------------------------------------------------------
// update systems
// -------------------------------------------------------
void Update::doButtonTimer(const DeltaTime dt)
{
	auto& buttonArray = nc().getComponentArray<Component::Button>();

	for (auto& [entity, button] : buttonArray->getAll())
	{
		if (button.timer <= 0.0)
		{
			continue;
		}

		button.timer -= dt;

		if (button.timer <= 0.0) button.timer = 0.0;
	}
}

template<typename T>
static T easeOutElastic(T current, T max, T min)
{
	T progress = 1.0 - [](T target, T min, T max)
		{
			return (target - min) / (max - min);
		}
	(current, min, max);

	static const double c4 = (2 * std::acos(-1.0)) / 3;

	if (progress <= 0) return 0.0;
	else if (progress >= 1) return 1.0;

	return std::pow(2.0, -10.0 * progress) *
		std::sin((progress * 10.0 - 0.75) * c4) + 1.0;
}

static constexpr double DEFAULT_SCALE_X = 1.0;
static constexpr double DEFAULT_SCALE_Y = 1.0;

static constexpr double HOVER_SCALE_X = 1.1;
static constexpr double HOVER_SCALE_Y = 1.1;

static constexpr double CLICKED_SCALE_X = 0.9;
static constexpr double CLICKED_SCALE_Y = 0.9;

void Update::applyButtonHoverAnimation()
{
	auto& buttonArray = nc().getComponentArray<Component::Button>();
	auto& spriteArray = nc().getComponentArray<Component::Sprite>();
	auto& textArray = nc().getComponentArray<Component::Text>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();

	for (auto& [entity, button] : buttonArray->getAll())
	{
		if (button.timer <= 0.0 ||
			button.state != Enum::ButtonState::HOVERED)
		{
			continue;
		}

		Component::Transform& transform = transformArray->getData(entity);
		Component::Sprite& sprite = spriteArray->getData(entity);

		double scalingFactor = easeOutElastic
		(
			button.timer,
			button.hoverAnimationDuration,
			0.0
		);

		sprite.body->setScale
		(
			sf::Vector2f
			(
				(HOVER_SCALE_X * (transform.width / sprite.body->getTexture().getSize().x)) * scalingFactor,
				(HOVER_SCALE_Y * (transform.height / sprite.body->getTexture().getSize().y)) * scalingFactor
			)
		);

		if (!textArray->hasData(entity))
		{
			continue;
		}

		Component::Text& text = textArray->getData(entity);
		text.box->setScale
		(
			sf::Vector2f
			(
				HOVER_SCALE_X * scalingFactor,
				HOVER_SCALE_Y * scalingFactor
			)
		);
	}
}
void Update::applyButtonIdleAnimation()
{
	auto& buttonArray = nc().getComponentArray<Component::Button>();
	auto& spriteArray = nc().getComponentArray<Component::Sprite>();
	auto& textArray = nc().getComponentArray<Component::Text>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();

	for (auto& [entity, button] : buttonArray->getAll())
	{
		if (button.timer <= 0.0 ||
			button.state != Enum::ButtonState::IDLE)
		{
			continue;
		}

		auto& transform = transformArray->getData(entity);
		auto& sprite = spriteArray->getData(entity);

		double scalingFactor = easeOutElastic
		(
			button.timer,
			button.idleAnimationDuration,
			0.0
		);

		sprite.body->setScale
		(
			sf::Vector2f
			(
				(DEFAULT_SCALE_X * (transform.width / sprite.body->getTexture().getSize().x)) * scalingFactor,
				(DEFAULT_SCALE_X * (transform.height / sprite.body->getTexture().getSize().y)) * scalingFactor
			)
		);

		if (!textArray->hasData(entity))
		{
			continue;
		}

		auto& text = textArray->getData(entity);
		text.box->setScale
		(
			sf::Vector2f
			(
				DEFAULT_SCALE_X * scalingFactor,
				DEFAULT_SCALE_X * scalingFactor
			)
		);
	}
}
void Update::applyButtonClickedAnimation()
{
	auto& buttonArray = nc().getComponentArray<Component::Button>();
	auto& spriteArray = nc().getComponentArray<Component::Sprite>();
	auto& textArray = nc().getComponentArray<Component::Text>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();

	for (auto& [entity, button] : buttonArray->getAll())
	{
		if (button.timer <= 0.0 ||
			button.state != Enum::ButtonState::CLICKED)
		{
			continue;
		}

		auto& transform = transformArray->getData(entity);
		auto& sprite = spriteArray->getData(entity);

		double scalingFactor = easeOutElastic
		(
			button.timer,
			button.clickedAnimationDuration,
			0.0
		);

		sprite.body->setScale
		(
			sf::Vector2f
			(
				(CLICKED_SCALE_X * (transform.width / sprite.body->getTexture().getSize().x)) * scalingFactor,
				(CLICKED_SCALE_X * (transform.height / sprite.body->getTexture().getSize().y)) * scalingFactor
			)
		);

		if (!textArray->hasData(entity))
		{
			continue;
		}

		auto& text = textArray->getData(entity);
		text.box->setScale
		(
			sf::Vector2f
			(
				CLICKED_SCALE_X * scalingFactor,
				CLICKED_SCALE_X * scalingFactor
			)
		);
	}
}
void Update::enableButtonNextScene()
{
	auto& buttonArray = nc().getComponentArray<Component::Button>();
	auto& nextSceneArray = nc().getComponentArray<Component::NextScene>();

	for (auto& [entity, button] : buttonArray->getAll())
	{
		if (button.timer > 0.0 ||
			button.state != Enum::ButtonState::CLICKED ||
			!nextSceneArray->hasData(entity))
		{
			continue;
		}

		const auto& nextScene = nextSceneArray->getData(entity);

		nc().call<Event::PlayScene>(Event::PlayScene{nextScene.next});

		break;
	}
}

static constexpr double BOUNCE_FACTOR = 0.5;

void Update::doDashTimer
(
	const Entity player,
	const DeltaTime dt
)
{
	auto& dashArray = nc().getComponentArray<Component::Dash>();

	if (!dashArray->hasData(player))
	{
		return;
	}

	auto& dash = dashArray->getData(player);

	if (dash.time <= 0.0)
	{
		dash.time = 0.0;
		return;
	}

	dash.time -= dt;
}
void Update::boundaryBounce
(
	const double left,
	const double right,
	const double top,
	const double bottom
)
{
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& positionArray = nc().getComponentArray<Component::Position>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();
	auto& originOffsetsArray = nc().getComponentArray<Component::OriginOffsets>();

	for (auto& [entity, velocity] : velocityArray->getAll())
	{
		if (!positionArray->hasData(entity) ||
			!transformArray->hasData(entity) ||
			!originOffsetsArray->hasData(entity))
		{
			continue;
		}

		auto& pos = positionArray->getData(entity);
		auto& trans = transformArray->getData(entity);
		auto& originOffsets = originOffsetsArray->getData(entity);

		if (pos.x + (trans.width - originOffsets.x) > right)
		{
			pos.x = right - (trans.width - originOffsets.x);
			velocity.x = -velocity.x * BOUNCE_FACTOR;
		}
		else if (pos.x - (trans.width - originOffsets.x) < left)
		{
			pos.x = left + (trans.width - originOffsets.x);
			velocity.x = -velocity.x * BOUNCE_FACTOR;
		}

		if (pos.y + (trans.height - originOffsets.y) > bottom)
		{
			pos.y = bottom - (trans.height - originOffsets.y);
			velocity.y = -velocity.y * BOUNCE_FACTOR;
		}
		else if (pos.y - (trans.height - originOffsets.y) < top)
		{
			pos.y = top + (trans.height - originOffsets.y);
			velocity.y = -velocity.y * BOUNCE_FACTOR;
		}
	}
}

static bool checkCollision
(
	sf::Vector2f a,
	sf::Vector2f b,
	sf::Vector2f aTrans,
	sf::Vector2f bTrans
)
{
	// AABB collision
	if (a.x + (aTrans.x / 2.f) > b.x - (bTrans.x / 2.f) &&
		a.x - (aTrans.x / 2.f) < b.x + (bTrans.x / 2.f) &&
		a.y + (aTrans.y / 2.f) > b.y - (bTrans.y / 2.f) &&
		a.y - (aTrans.y / 2.f) < b.y + (bTrans.y / 2.f))
	{
		return true;
	}

	return false;
}

// does not check if there's a collision, just returns the amount
static float overlapAmount
(
	float coordinate1,
	float coordinate2,
	float dimension1,
	float dimension2,
	float offset1,
	float offset2
)
{
	return ((dimension1 - offset1) + (dimension2 - offset2)) - std::abs(coordinate1 - coordinate2);
}

constexpr int SKIP_FRAMES = 60;
constexpr double HIT_VELOCITY_TRESHOLD = 500.0;
constexpr double HIT_BOUNCE_FACTOR = 1.5;

void Update::skipFrame
(
	int& skipFrame,
	Entity player
)
{
	auto& positionArray = nc().getComponentArray<Component::Position>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();
	auto& originOffsetsArray = nc().getComponentArray<Component::OriginOffsets>();
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();

	if (!positionArray->hasData(player) ||
		!transformArray->hasData(player) ||
		!originOffsetsArray->hasData(player) ||
		!velocityArray->hasData(player))
	{
		return;
	}

	auto& playerPos = positionArray->getData(player);
	auto& playerTrans = transformArray->getData(player);
	auto& playerOriginOffsets = originOffsetsArray->getData(player);
	auto& playerVelocity = velocityArray->getData(player);

	const auto playerPosVector = sf::Vector2f
	{
		static_cast<float>(playerPos.x),
		static_cast<float>(playerPos.y)
	};

	const auto playerTransVector = sf::Vector2f
	{
		static_cast<float>(playerTrans.width),
		static_cast<float>(playerTrans.height)
	};

	for (auto& [entity, transform] : transformArray->getAll())
	{
		if (!positionArray->hasData(entity) ||
			!transformArray->hasData(entity) ||
			!originOffsetsArray->hasData(entity) ||
			player == entity)
		{
			continue;
		}

		auto& pos = positionArray->getData(entity);
		auto& trans = transformArray->getData(entity);
		auto& originOffsets = originOffsetsArray->getData(entity);

		const auto posVector = sf::Vector2f
		{
			static_cast<float>(pos.x),
			static_cast<float>(pos.y)
		};

		const auto transVector = sf::Vector2f
		{
			static_cast<float>(trans.width),
			static_cast<float>(trans.height)
		};

		if (checkCollision
		(
			playerPosVector,
			posVector,
			playerTransVector,
			transVector
		) && 
			(
				playerVelocity.x > HIT_VELOCITY_TRESHOLD ||
				playerVelocity.y > HIT_VELOCITY_TRESHOLD ||
				playerVelocity.x < -HIT_VELOCITY_TRESHOLD ||
				playerVelocity.y < -HIT_VELOCITY_TRESHOLD
			)
			)
		{
			float overlapX = overlapAmount
			(
				playerPos.x,
				pos.x,
				playerTrans.width,
				trans.width,
				playerOriginOffsets.x,
				originOffsets.x
			);
			float overlapY = overlapAmount
			(
				playerPos.y,
				pos.y,
				playerTrans.height,
				trans.height,
				playerOriginOffsets.y,
				originOffsets.y
			);

			if (overlapX > overlapY)
			{
				playerVelocity.y *= HIT_BOUNCE_FACTOR;
			}
			else if (overlapX < overlapY)
			{
				playerVelocity.x *= HIT_BOUNCE_FACTOR;
			}

			//std::cout << "test" << "\n";
			skipFrame = SKIP_FRAMES;
		}
	}
}

void Update::collision(DeltaTime dt)
{
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& positionArray = nc().getComponentArray<Component::Position>();
	auto& transformArray = nc().getComponentArray<Component::Transform>();
	auto& originOffsetsArray = nc().getComponentArray<Component::OriginOffsets>();

	for (auto& [entity1, velocity] : velocityArray->getAll())
	{
		if (!positionArray->hasData(entity1) ||
			!transformArray->hasData(entity1) ||
			!originOffsetsArray->hasData(entity1))
		{
			continue;
		}

		auto& pos1 = positionArray->getData(entity1);
		auto& trans1 = transformArray->getData(entity1);
		auto& originOffsets1 = originOffsetsArray->getData(entity1);

		const auto pos1Vector = sf::Vector2f
		{
			static_cast<float>(pos1.x),
			static_cast<float>(pos1.y)
		};

		const auto trans1Vector = sf::Vector2f
		{
			static_cast<float>(trans1.width),
			static_cast<float>(trans1.height)
		};

		for (auto& [entity2, transform] : transformArray->getAll())
		{
			if (!positionArray->hasData(entity2) ||
				!transformArray->hasData(entity2) ||
				!originOffsetsArray->hasData(entity2) ||
				entity1 == entity2)
			{
				continue;
			}

			auto& pos2 = positionArray->getData(entity2);
			auto& trans2 = transformArray->getData(entity2);
			auto& originOffsets2 = originOffsetsArray->getData(entity2);

			const auto pos2Vector = sf::Vector2f
			{
				static_cast<float>(pos2.x),
				static_cast<float>(pos2.y)
			};

			const auto trans2Vector = sf::Vector2f
			{
				static_cast<float>(trans2.width),
				static_cast<float>(trans2.height)
			};
			
			if (checkCollision
			(
				pos1Vector,
				pos2Vector,
				trans1Vector,
				trans2Vector
			)
				)
			{
				float overlapX = overlapAmount
				(
					pos1.x,
					pos2.x,
					trans1.width,
					trans2.width,
					originOffsets1.x,
					originOffsets2.x
				);
				float overlapY = overlapAmount
				(
					pos1.y,
					pos2.y,
					trans1.height,
					trans2.height,
					originOffsets1.y,
					originOffsets2.y
				);

				//std::cout << "overlapX: " << overlapX << "\n";
				//std::cout << "overlapY: " << overlapY << "\n";

				if (overlapX > overlapY)
				{
					// flips the overlap amount if the first entity is behind the second
					overlapY = pos1.y < pos2.y ? -overlapY : overlapY;

					velocity.y = -velocity.y * BOUNCE_FACTOR;
					pos1.y += overlapY;
				}
				else if (overlapX < overlapY)
				{
					overlapX = pos1.x < pos2.x ? -overlapX : overlapX;

					velocity.x = -velocity.x * BOUNCE_FACTOR;
					pos1.x += overlapX;
				}

				//std::cout << "test" << "\n";
			}
		}

		//pos.x += (velocity.x * dt);
		//pos.y += (velocity.y * dt);
	}
}
void Update::move(const DeltaTime dt)
{
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& positionArray = nc().getComponentArray<Component::Position>();

	for (auto& [entity, velocity] : velocityArray->getAll())
	{
		if (!positionArray->hasData(entity))
		{
			continue;
		}

		auto& pos = positionArray->getData(entity);
		pos.x += (velocity.x * dt);
		pos.y += (velocity.y * dt);
	}
}
void Update::drag(const DeltaTime dt)
{
	auto& velocityArray = nc().getComponentArray<Component::Velocity>();
	auto& dragArray = nc().getComponentArray<Component::Drag>();

	for (auto& [entity, velocity] : velocityArray->getAll())
	{
		if (!dragArray->hasData(entity))
		{
			continue;
		}

		const auto& drag = dragArray->getData(entity);

		// can't be exactly 0.0 because it will drift aimlessly
		velocity.x = velocity.x < -0.1 ? velocity.x + (drag.x * dt) :
			velocity.x > 0.1f ? velocity.x - (drag.x * dt) : 0.0;
		velocity.y = velocity.y < -0.1 ? velocity.y + (drag.y * dt) :
			velocity.y > 0.1f ? velocity.y - (drag.y * dt) : 0.0;
	}
}

// -------------------------------------------------------
// rendering systems
// -------------------------------------------------------
void Render::doZIndex(std::queue<Entity>& renderQueue)
{
	auto& zIndexArray = nc().getComponentArray<Component::ZIndex>();

	std::vector<std::pair<int, Entity>> renderVector;
	for (auto& [entity, zIndex] : zIndexArray->getAll())
	{
		if (zIndex.visible) renderVector.emplace_back(zIndex.index, entity);
	}
	std::sort(renderVector.begin(), renderVector.end());

	for (auto& [zIndex, entity] : renderVector)
	{
		renderQueue.push(entity);
	}
}
void Render::render
(
	sf::RenderWindow& window,
	std::queue<Entity>& renderQueue
)
{
	auto& spriteArray = nc().getComponentArray<Component::Sprite>();
	auto& positionArray = nc().getComponentArray<Component::Position>();
	auto& textArray = nc().getComponentArray<Component::Text>();

	while (!renderQueue.empty())
	{
		Entity& popped = renderQueue.front();
		////std::cout << "popped: " << popped << "\n";

		if (!positionArray->hasData(popped))
		{
			// this means the entity does not have a position component
			continue;
		}

		auto& pos = positionArray->getData(popped);

		if (spriteArray->hasData(popped))
		{
			auto& sprite = spriteArray->getData(popped);

			sprite.body->setPosition
			(
				sf::Vector2f
				(
					pos.x,
					pos.y
				)
			);
			window.draw(sprite.body.value());
		}

		if (textArray->hasData(popped))
		{
			auto& text = textArray->getData(popped);

			text.box->setPosition
			(
				sf::Vector2f
				(
					pos.x,
					pos.y
				)
			);
			window.draw(text.box.value());
		}

		renderQueue.pop();
	}
}