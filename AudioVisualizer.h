#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include <complex>
#include <valarray>
#include <vector>
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

struct AudioVis {
	const sf::Sound& sound;
	const sf::SoundBuffer& buffer;
	long long sampleCount;
	int sampleRate;
	int channelCount;

	//this should be a power of 2
	const int bufferSize = 2048;
	sf::VertexArray va;
	CArray complex;
	std::vector<float> window;
	int pre_width = 0;
	int max_volume = 0;
	const float base;

	AudioVis(const sf::Sound& sound, const sf::SoundBuffer& buffer) : sound(sound), buffer(buffer), base(1.05) {
		sampleCount = buffer.getSampleCount();
		channelCount = buffer.getChannelCount();
		sampleRate = buffer.getSampleRate() * channelCount;
		complex.resize(bufferSize);
		for (int i = 0; i < (bufferSize); ++i) window.push_back(0.54 - 0.46 * std::cos(2 * 3.141592653589 * i / (float)(bufferSize)));
	}
	
	//Fast Fourier Transform
	void fft(CArray &x) {
		const int N = x.size();
		if (N <= 1) return;
		CArray even = x[std::slice(0, N / 2, 2)];
		CArray odd = x[std::slice(1, N / 2, 2)];
		fft(even);
		fft(odd);

		for (int k = 0; k < N / 2; k++) {
			Complex t = std::polar(1.0, -2 * 3.141592653589 * k / N) * odd[k];
			x[k] = even[k] + t;
			x[k + N / 2] = even[k] - t;
		}
	}

	void render(sf::RenderTarget& canvas) {
		//collect samples
		int mark = sound.getPlayingOffset().asSeconds() * sampleRate;
		for (int i = mark; i < mark + (bufferSize) && i < sampleCount; ++i) {
			complex[i - mark] = Complex(buffer.getSamples()[i] * window[i - mark], 0.0);
		}
		
		//analyze frequencies
		fft(complex);
		
		//rendering on an exponential scale
		va.setPrimitiveType(sf::Quads);
		va.clear();
		int pre = 0;
		int x = 0;
		int off = (800 - pre_width * 4) / 2;
		int previous = 0;
		int max_vol = 0;
		for (float i = 1; i < bufferSize >> 1; i *= base) {
			if (i * sampleRate / bufferSize > 4000) break;
			if ((int)i == pre) continue;
			++x;
			pre = (int)i;
			int mag = std::abs(complex[(int)i]) / 50000;
			if (mag > max_vol) max_vol = mag;

			int mixed = std::sqrt(0.25*mag*mag + 0.25*previous*previous);
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 2, 582), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 5, 582), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 5, 581 - mixed), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 2, 581 - mixed), sf::Color(0, 0, 0, 128)));

			va.append(sf::Vertex(sf::Vector2f(x * 4 + off, 580), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 3, 580), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 3, 579 - mixed), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off, 579 - mixed), sf::Color::White));
			++x;
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 2, 582), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 5, 582), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 5, 581 - mag), sf::Color(0, 0, 0, 128)));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 2, 581 - mag), sf::Color(0, 0, 0, 128)));

			va.append(sf::Vertex(sf::Vector2f(x * 4 + off, 580), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 3, 580), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off + 3, 579 - mag), sf::Color::White));
			va.append(sf::Vertex(sf::Vector2f(x * 4 + off, 579 - mag), sf::Color::White));
			previous = mag;
		}
		max_volume = max_vol * 0.3 + max_volume * 0.7;

		pre_width = x;

		canvas.draw(va);
	}
};