#include <SFML/Graphics.hpp>

#include <optional>
#include <queue>

#include "../src/Game/Headers/GameManager.hpp"
#include "../src/Game/Headers/Scenes.hpp"

void playingScene()
{
	// game state variables
	sf::Clock clock;
	std::queue<Entity> renderQueue;

	Component::Singleton::TexturesContainer loadedTextures{};

	// entity instantiation
	Entity player = Maker::player
	(
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
		}

		// systems
		Control::doPlayerControl
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
		Update::move(dt);
		Update::drag(dt);

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