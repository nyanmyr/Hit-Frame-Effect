#ifndef SCENES_HPP
#define SCENES_HPP

#include <SFML/Graphics.hpp>

enum class Scene
{
	MENU,
	PLAYING,
};

void playScene(Scene scene);
void menuScene();
void playingScene();


#endif