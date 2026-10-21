#ifndef ENUMS_HPP
#define ENUMS_HPP

namespace Enum
{
	enum class Layer
	{
		MAIN = 0,
		UI = 1,
		TEXT = 0
	};

	enum class TextFormat
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		CENTER,
	};

	enum class Texture
	{
		TEXTURE_PLACEHOLDER,
	};

	enum class ButtonState
	{
		IDLE,
		HOVERED,
		CLICKED,
	};

	enum class OriginPoint
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		CENTER,
	};
}

#endif