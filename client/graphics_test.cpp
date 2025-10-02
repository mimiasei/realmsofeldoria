/*
 * graphics_test.cpp - Simple test of Canvas rendering
 * Part of Realms of Eldoria
 */
#include <iostream>
#include <SDL2/SDL.h>
#include "../lib/render/Canvas.h"
#include "../lib/geometry/Color.h"

int main(int argc, char* argv[])
{
	std::cout << "Starting Realms of Eldoria Graphics Test..." << std::endl;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Create window
	SDL_Window* window = SDL_CreateWindow(
		"Realms of Eldoria - Graphics Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1920, 1080,
		SDL_WINDOW_SHOWN
	);

	if (!window)
	{
		std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Get window surface
	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);

	// Create canvas from window surface
	Canvas canvas = Canvas::createFromSurface(windowSurface);

	// Test drawing operations
	std::cout << "Testing Canvas drawing..." << std::endl;

	// Fill with dark blue background
	canvas.fill(ColorRGBA(0, 0, 64, 255));

	// Draw some colored rectangles
	canvas.drawRect(Rect(100, 100, 200, 150), ColorRGBA(255, 0, 0, 255)); // Red
	canvas.drawRect(Rect(350, 100, 200, 150), ColorRGBA(0, 255, 0, 255)); // Green
	canvas.drawRect(Rect(600, 100, 200, 150), ColorRGBA(0, 0, 255, 255)); // Blue

	// Draw some borders
	canvas.drawBorder(Rect(100, 300, 200, 150), ColorRGBA(255, 255, 0, 255), 3); // Yellow border
	canvas.drawBorder(Rect(350, 300, 200, 150), ColorRGBA(255, 0, 255, 255), 3); // Magenta border
	canvas.drawBorder(Rect(600, 300, 200, 150), ColorRGBA(0, 255, 255, 255), 3); // Cyan border

	// Draw some lines
	canvas.drawLine(Point(100, 500), Point(800, 500), ColorRGBA(255, 255, 255, 255)); // White horizontal
	canvas.drawLine(Point(450, 500), Point(450, 700), ColorRGBA(255, 255, 255, 255)); // White vertical
	canvas.drawLine(Point(100, 550), Point(800, 700), ColorRGBA(255, 128, 0, 255));   // Orange diagonal

	// Draw grid of points
	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			canvas.drawPoint(Point(900 + x * 10, 100 + y * 10), ColorRGBA(128, 128, 255, 255));
		}
	}

	// Update window
	SDL_UpdateWindowSurface(window);

	std::cout << "Rendering complete. Press any key or close window to exit..." << std::endl;

	// Event loop
	bool running = true;
	SDL_Event event;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
			{
				running = false;
			}
		}
		SDL_Delay(16); // ~60 FPS
	}

	// Cleanup
	SDL_DestroyWindow(window);
	SDL_Quit();

	std::cout << "Graphics test ended successfully." << std::endl;
	return 0;
}
