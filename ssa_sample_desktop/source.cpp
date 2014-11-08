#include <stegosaurus.hpp>
#include "screens/screen_stack.hpp"
#include "screens/game_context.hpp"
#include "screens/game_screen.hpp"

#include <iostream>
#include <chrono>

#ifdef _DEBUG
	#pragma comment(lib, "ssa_desktop_d.lib")
#else
#	pragma comment(lib, "ssa_desktop.lib")
#endif
const unsigned int update_time_ms{ 16 };

int entry_point(cmd_args_t& p_args, ssa::Platform p_platform)
{
	using namespace ssa;
	using namespace std::placeholders;

	// Initializing device
	RenderDevice render_device;
	if (!render_device.init(0))
	{
		std::cout << "Failed to initialize render device " << std::endl;
		return EXIT_FAILURE;
	}

	// Creating window
	Window window;
	if (!window.create(1280, 720, "SadEmoji", Window::Style::Default, Window::Orientation::Landscape))
	{
		std::cout << "Failed to create window " << std::endl;
		return EXIT_FAILURE;
	}

	// Creating renderer
	Renderer2D renderer2D(render_device);
	Blender blender(render_device);
	if (!blender.create(BlendType::Alpha))
	{
		std::cout << "Failed to create blend state " << std::endl;
		return EXIT_FAILURE;
	}

	// Creating Render Window
	ssa::Texture render_window(render_device);
	if (!render_window.create_from_window(window))
	{
		std::cout << "Failed to create render window " << std::endl;
		return EXIT_FAILURE;
	}

	// Screen Stacks
	ScreenStack<GameContext> screen_stack;
	GameContext game_context(render_device, renderer2D, window, render_window);
	GameScreen game_screen(screen_stack);
	
	game_context.window.register_callback(ssa::Window::Event::KeyDown, std::bind(&GameScreen::on_key_down, &game_screen, _1, _2, _3, _4));
	game_context.window.register_callback(ssa::Window::Event::KeyUp, std::bind(&GameScreen::on_key_up, &game_screen, _1, _2, _3, _4));

	screen_stack.push_state(game_screen, game_context);
	std::chrono::time_point<std::chrono::high_resolution_clock> prev, now;
	prev = std::chrono::high_resolution_clock::now();

	unsigned int stacked_update_ms{ 0 };

	render_device.push_target(render_window, false);

	while (true)
	{
		// Updating window
		window.update();

		now = std::chrono::high_resolution_clock::now();
		unsigned int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
		prev = now;

		stacked_update_ms += elapsed_ms;

		while (stacked_update_ms >= update_time_ms)
		{
			screen_stack.update(game_context, update_time_ms);
			stacked_update_ms -= update_time_ms;
		}

		render_device.bind_blender(blender);
		render_device.clear(render_window, float4(0.f, 0.f, 0.f, 0.f));

		screen_stack.render(game_context, elapsed_ms);

		render_device.finalize(render_window, false);
	}

	render_window.destroy();

	return EXIT_SUCCESS;
}

ssa_entry_point(entry_point);