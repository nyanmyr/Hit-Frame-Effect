#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>

namespace Config
{
	inline constexpr int SCREEN_WIDTH = 800;
	inline constexpr int SCREEN_HEIGHT = 600;
	inline constexpr int MAX_FPS = 60;
	inline const std::string WINDOW_NAME = "Nacre Engine";
}

namespace Asset
{
	enum class Font
	{
		MAIN
	};

	inline auto& font(Asset::Font font)
	{
		switch (font)
		{
		case Asset::Font::MAIN:
		{
			static sf::Font font =
				[] {
				sf::Font f;
				if (!f.openFromFile(RESOURCES_PATH "arial.ttf"))
				{
					throw std::runtime_error("Font not found: " + std::string(RESOURCES_PATH "arial.ttf"));
				}
				return f;
				}();

			return font;
		}
		default:
			throw std::out_of_range("Error: unhandled font case requested!");
		}
	}

	inline auto& window()
	{
		static sf::RenderWindow window =
			[] {
			sf::RenderWindow w
			(
				sf::VideoMode
				({
					Config::SCREEN_WIDTH,
					Config::SCREEN_HEIGHT
					}),
				Config::WINDOW_NAME,
				sf::Style::Close
			);

			w.setFramerateLimit(Config::MAX_FPS);

			// resets the centerpoint of the window camera to (0,0)
			w.setView
			(
				sf::View
				(
					sf::Vector2f
					(
						0.f,
						0.f
					),
					sf::Vector2f
					(
						Config::SCREEN_WIDTH,
						Config::SCREEN_HEIGHT
					)
				)
			);

			return w;
			}();

		return window;
	}
}

#endif