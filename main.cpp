#include <sstream>

#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include "AudioVisualizer.h"
#include "Random.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Oscilloscope", sf::Style::Close | sf::Style::Titlebar);

	sf::Texture sunset;
	sunset.loadFromFile("sunset.jpg");
	sf::Sprite background = sf::Sprite(sunset);

	sf::Texture logo;
	logo.loadFromFile("logo.png");
	sf::Sprite logo_sprite = sf::Sprite(logo);
	logo_sprite.setOrigin(75, 75);
	logo_sprite.setPosition(400, 320);

	sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(800, 150));
	rect.setFillColor(sf::Color(0, 0, 0, 100));
	sf::VertexArray vol = sf::VertexArray(sf::Quads, 4);

	sf::Font simplifica;
	simplifica.loadFromFile("simplifica.ttf");
	sf::Text t1 = sf::Text("Zedd - I want you to know", simplifica, 64);
	sf::Text t2 = sf::Text("0:0", simplifica, 30);

	sf::SoundBuffer buffer;
	buffer.loadFromFile("Zedd - I Want You To Know (Official Music Video) ft. Selena Gomez.ogg");
	sf::Sound sound = sf::Sound(buffer);
	sound.play();

	sf::RectangleShape progress = sf::RectangleShape(sf::Vector2f(0, 2));
	progress.setFillColor(sf::Color::White);

	AudioVis av = AudioVis(sound, buffer);

	while (window.isOpen()) {
		sf::Event evnt;
		while (window.pollEvent(evnt)) {
			if (evnt.type == evnt.Closed) {
				window.close();
				return 0;
			}
		}

		window.clear();

		window.draw(background);
		av.render(window);

		window.draw(rect);
		int alpha = av.max_volume * 4;
		if (alpha > 255) alpha = 255;
		vol[0].position = sf::Vector2f(0, 150);
		vol[0].color = sf::Color(255, 255, 255, alpha);
		vol[1].position = sf::Vector2f(800, 150);
		vol[1].color = sf::Color(255, 255, 255, alpha);
		vol[2].position = sf::Vector2f(800, std::sqrt(av.max_volume * 8) + 150);
		vol[2].color = sf::Color(255, 255, 255, 0);
		vol[3].position = sf::Vector2f(0, std::sqrt(av.max_volume * 8) + 150);
		vol[3].color = sf::Color(255, 255, 255, 0);
		window.draw(vol);

		t1.setFillColor(sf::Color(0, 0, 0, 128));
		int xt1 = 402 - t1.getLocalBounds().width / 2;
		int width = t1.getLocalBounds().width;
		t1.setPosition(xt1, 22);
		window.draw(t1);
		t1.setFillColor(sf::Color::White);
		t1.move(-2, -2);
		window.draw(t1);

		std::ostringstream str;
		int mins = (int)(sound.getPlayingOffset().asSeconds() / 60), secs = ((int)sound.getPlayingOffset().asSeconds() % 60);
		if (mins < 10) str << '0';
		str << mins << ':';
		if (secs < 10) str << '0';
		str << secs;
		t2.setString(str.str());
		t2.setFillColor(sf::Color(0, 0, 0, 128));
		t2.setPosition(xt1, 100);
		window.draw(t2);
		t2.move(-2, -2);
		t2.setFillColor(sf::Color::White);
		window.draw(t2);

		progress.setFillColor(sf::Color(0, 0, 0, 128));
		progress.setSize(sf::Vector2f(sound.getPlayingOffset().asSeconds() * width / (buffer.getSampleCount() / buffer.getSampleRate() / buffer.getChannelCount()), 2));
		progress.setPosition(402 - width / 2, 103);
		window.draw(progress);
		progress.setFillColor(sf::Color::White);
		progress.move(-2, -2);
		window.draw(progress);
		progress.setFillColor(sf::Color(255, 255, 255, 128));
		progress.setSize(sf::Vector2f(width, 2));
		window.draw(progress);

		logo_sprite.setScale(1.0 + (float)av.max_volume / 400, 1.0 + (float)av.max_volume / 400);
		logo_sprite.setPosition(400, 320);
		if (av.max_volume > 50) logo_sprite.move(random.randomInteger(-av.max_volume + 50, av.max_volume - 50) / 20, random.randomInteger(-av.max_volume + 50, av.max_volume - 50) / 20);
		window.draw(logo_sprite);

		window.display();
	}
	return 0;
}