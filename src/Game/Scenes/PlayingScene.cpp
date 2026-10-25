#include <SFML/Graphics.hpp>

#include <optional>
#include <queue>

#include "../src/Game/Headers/GameManager.hpp"
#include "../src/Game/Headers/Scenes.hpp"

constexpr double PLAYER_DASH_POWER = 2.5;
constexpr double PLAYER_DASH_COOLDOWN = 2.0;

void playingScene()
{
	// game state variables
	sf::Clock clock;
	std::queue<Entity> renderQueue;

	Component::Singleton::TexturesContainer loadedTextures{};
	Component::Singleton::CameraShake cameraShake{};

	// entity instantiation
	Entity player = Maker::player
	(
		PLAYER_DASH_POWER,
		PLAYER_DASH_COOLDOWN,
		sf::Color::Green,
		Enum::Layer::MAIN,
		sf::Vector2f
		{
			0.f,
			0.f
		},
		sf::Vector2f
		{
			40.f,
			40.f
		},
		sf::Vector2f
		{
			-300.f,
			-300.f
		},
		sf::Vector2f
		{
			300.f,
			300.f
		},
		sf::Vector2f
		{
			500.f,
			500.f
		},
		sf::Vector2f
		{
			50.f,
			50.f
		},
		Enum::Texture::TEXTURE_PLACEHOLDER,
		Enum::OriginPoint::CENTER
	);

	Entity object = Maker::object
	(
		sf::Color::Red,
		Enum::Layer::MAIN,
		sf::Vector2f
		{
			250.f,
			0.f
		},
		sf::Vector2f
		{
			40.f,
			40.f
		},
		Enum::Texture::TEXTURE_PLACEHOLDER,
		Enum::OriginPoint::CENTER
	);

	int skipFrame = 0;

	// onstart systems
	Start::loadTextures(loadedTextures);
	Start::loadSprites(loadedTextures);
	Start::setSpriteOrigin();
	Start::setColor();

	while (Asset::window().isOpen())
	{
		DeltaTime dt = clock.restart().asSeconds();

		while (const std::optional event = Asset::window().pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				Asset::window().close();
			}

			if (skipFrame <= 0)
			{
				if (const auto& keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->scancode == sf::Keyboard::Scancode::Space)
					{
						Control::doDash(player);
					}
				}
			}
		}

		// systems
		if (skipFrame <= 0)
		{
			Control::doPlayerControl
			(
				player,
				dt
			);
		}

		Update::doDashTimer
		(
			player,
			dt
		);
		Update::boundaryBounce
		(
			-(Config::SCREEN_WIDTH / 2.0),
			(Config::SCREEN_WIDTH / 2.0),
			-(Config::SCREEN_HEIGHT / 2.0),
			(Config::SCREEN_HEIGHT / 2.0)
		);
		Update::hitEffect
		(
			skipFrame,
			player,
			cameraShake
		);
		Update::collision(dt);

		if (skipFrame > 0)
		{
			// std::cout << "test" << "\n";
			--skipFrame;
		}

		else if (skipFrame <= 0)
		{
			Update::move(dt);
			Update::drag(dt);
		}

		Update::shakeCamera
		(
			dt,
			Asset::window(),
			cameraShake
		);
		
		Asset::window().clear();
		// render systems
		Render::doZIndex(renderQueue);
		Render::render
		(
			Asset::window(),
			renderQueue
		);
		Asset::window().display();
	}
}