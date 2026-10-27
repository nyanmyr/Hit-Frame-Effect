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

	inline auto& renderTexture()
	{
		static sf::RenderTexture renderTexture =
			[] {
			sf::RenderTexture r;

			if (!r.resize({ Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT }))
			{
				throw std::runtime_error("Error: Cannot load shader into graphics memory");
			}

			r.setView
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

			return r;
			}();

		return renderTexture;
	}

	inline auto& shader()
	{
		static sf::Shader shader =
			[] {
			sf::Shader s;
			if (!s.loadFromFile(RESOURCES_PATH "shader.frag", sf::Shader::Type::Fragment))
			{
				throw std::runtime_error("Shader not found: " + std::string(RESOURCES_PATH "shader.frag"));
			}

			s.setUniform("texture", sf::Shader::CurrentTexture);

			return s;
			}();

		return shader;
	}

	inline auto& sceneSprite()
	{
		static sf::Sprite sceneSprite =
			[] {
			sf::Sprite sS(renderTexture().getTexture());

			sS.setOrigin
			(
				sf::Vector2f
				{
					Config::SCREEN_WIDTH * 0.5f,
					Config::SCREEN_HEIGHT * 0.5f
				}
			);

			return sS;
			}();

		return sceneSprite;
	}
}

#endif