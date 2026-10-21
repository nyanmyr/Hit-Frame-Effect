#include <SFML/Graphics.hpp>

#include <optional>
#include <queue>

#include "../src/Game/Headers/GameManager.hpp"
#include "../src/Game/Headers/Scenes.hpp"

static constexpr double DEFAULT_HOVER_ANIMATION_DURATION = 0.4;
static constexpr double DEFAULT_IDLE_ANIMATION_DURATION = 0.4;
static constexpr double DEFAULT_CLICKED_ANIMATION_DURATION = 0.15;

void menuScene()
{

	// game state variables
	sf::Clock clock;
	std::queue<Entity> renderQueue;

	Component::Singleton::TexturesContainer loadedTextures{};

	Entity title = Maker::text
	(
		64,
		Asset::font(Asset::Font::MAIN),
		sf::Color::White,
		Enum::Layer::TEXT,
		"Nacre Engine",
		sf::Vector2f
		{
			0.f,
			-200.f
		},
		Enum::TextFormat::CENTER
	);

	// entity instantiation
	Entity playButton = Maker::button
	(
		64,
		Scene::PLAYING,
		DEFAULT_HOVER_ANIMATION_DURATION,
		DEFAULT_IDLE_ANIMATION_DURATION,
		DEFAULT_CLICKED_ANIMATION_DURATION,
		sf::Color::Red,
		sf::Color::White,
		Asset::font(Asset::Font::MAIN),
		Enum::Layer::UI,
		"Play",
		sf::Vector2f
		{
			0.f,
			0.f
		},
		sf::Vector2f
		{
			200.f,
			100.f
		},
		Enum::TextFormat::CENTER,
		Enum::OriginPoint::CENTER,
		Enum::Texture::TEXTURE_PLACEHOLDER
	);

	// onstart systems
	Start::setText();
	Start::setTextOrigin();

	Start::loadTextures(loadedTextures);
	Start::loadSprites(loadedTextures);
	Start::setSpriteOrigin();
	Start::setColor();

	while (Asset::window().isOpen())
	{
		DeltaTime dt = clock.restart().asSeconds();

		auto& pixelPos = sf::Mouse::getPosition(Asset::window());
		auto& worldPos = Asset::window().mapPixelToCoords(pixelPos);

		while (const std::optional event = Asset::window().pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				Asset::window().close();
			}

			if (const auto& mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mousePressed->button == sf::Mouse::Button::Left)
				{
					nc().call<Event::MouseClick>
						(
							Event::MouseClick
							{
								sf::Vector2f
								{
									worldPos.x,
									worldPos.y
								}
							}
						);
				}
			} else if (event->is<sf::Event::MouseMovedRaw>() ||
				event->is<sf::Event::MouseLeft>() ||
				event->is<sf::Event::MouseEntered>())
			{
				nc().call<Event::MouseMoved>
					(
						Event::MouseMoved
						{
							sf::Vector2f
							{
								worldPos.x,
								worldPos.y
							}
						}
					);
			}
		}

		// update systems
		Update::doButtonTimer(dt);
		Update::applyButtonHoverAnimation();
		Update::applyButtonIdleAnimation();
		Update::applyButtonClickedAnimation();
		Update::enableButtonNextScene();

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