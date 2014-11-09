#include "game_screen.hpp"
#include "../game/physics_system.hpp"
#include "../map/map_loader.hpp"
#include <utility/ssa_loaders.hpp>

#include <iostream>
#undef max
#undef min

#include <algorithm>

void GameScreen::on_key_up(Window::Event, Window&, unsigned int p_code, unsigned int)
{
	KeyCode code = static_cast<KeyCode>(p_code);
	if (code == KeyCode::LeftArrow)
	{
		if (m_physics.player_input.x < 0)
			m_physics.player_input.x = 0.f;
	}
	else if (code == KeyCode::RightArrow)
	{
		if (m_physics.player_input.x > 0)
			m_physics.player_input.x = 0.f;
	}
	else if (code == KeyCode::UpArrow)
	{
		m_physics.player_input.y = 0.f;
	}
}

void GameScreen::on_key_down(Window::Event, Window&, unsigned int p_code, unsigned int)
{
	KeyCode code = static_cast<KeyCode>(p_code);
	if (code == KeyCode::LeftArrow)
	{
		m_physics.player_input.x = -2.5f;
	}
	else if (code == KeyCode::RightArrow)
	{
		m_physics.player_input.x = 2.5f;
	}
	else if (code == KeyCode::UpArrow)
	{
		m_physics.player_input.y = -7.f;
	}
}


GameScreen::GameScreen(ScreenStack<GameContext>& p_screen_machine) :
Screen(p_screen_machine),
m_player_emitter(100)
{
	m_context = nullptr;
	m_player_texture = nullptr;
	m_particle_texture = nullptr;
	m_base_blur = nullptr;
	m_blender = nullptr;
	m_map_texture = nullptr;
}

GameScreen::~GameScreen()
{
	delete m_player_texture;
	delete m_particle_texture;

	delete m_base_blur;
	delete m_map_texture;
}

void GameScreen::on_activation(GameContext& p_context)
{
	m_particle_system.init(p_context.render_device, p_context.renderer2D);

	m_player_emitter.emission_rate = 0.05f;
	m_player_emitter.lifetime = 1.f;
	m_player_emitter.start_velocity = float2(-1.f, -1.f);
	m_player_emitter.randomizer_velocity = float2(2.f, 2.f);
	m_player_emitter.start_speed = 10;
	m_player_emitter.start_scale = ssa::float2(0.1f);

	ssa::loaded_data pd;
	unsigned long pw, ph;
	if (!ssa::load_png("particletexture.png", pd, pw, ph))
	{
		std::cout << "kill yourself" << std::endl;
	}

	m_particle_texture = new Texture(p_context.render_device);
	if (!m_particle_texture->create(pw, ph, Texture::Format::RGBA8Unorm, false, &pd[0]))
	{
		std::cout << "1231231" << std::endl;
	}

	m_player_emitter.texture = m_particle_texture;

	m_particle_system.emitters.push_back(&m_player_emitter);

	m_context = &p_context;

	m_player_component.size = float2(30.f, 30.f);
	m_player_component.velocity = float2(0.f, 0.f);
	m_physics.set_gravity(15.f);
	m_physics.set_player(&m_player_component);
	m_physics.set_active(true);

	ssa::loaded_data ptd;
	if (!ssa::load_png("sprite.png", ptd, pw, ph))
	{
		std::cout << "kill yourself" << std::endl;
	}

	m_player_texture = new Texture(p_context.render_device);
	if (!m_player_texture->create(pw, ph, Texture::Format::RGBA8Unorm, false, &ptd[0]))
	{
		std::cout << "1231231" << std::endl;
	}

	//m_player_sprite.set_texture(*m_player_texture);
	m_player_sprite.set_rect(30.f, 30.f);
	m_player_sprite.set_color(float4(0.4f, 0.1f, 0.2f, 1.f));

	if (!load_map("sample_map7.tmx", m_map))
		std::cout << "Failed to load map " << std::endl;

	m_player_component.position = m_map.spawn;

	// Loading textures
	for (const auto& tileset : m_map.tile_sets)
	{
		ssa::loaded_data texture_data;
		unsigned long width, height;
		if (!ssa::load_png(tileset.source_image, texture_data, width, height))
		{
			std::cout << "Failed to load image : " << tileset.source_image << std::endl;
			continue;
		}

		m_texture_atlases.push_back(Texture(p_context.render_device));
		if (!p_context.render_device.create_texture(width, height, ssa::Format::RGBA8Unorm, false, &texture_data[0], m_texture_atlases.back()))
			std::cout << "Failed to create texture from image : " << tileset.source_image << std::endl;
	}

	// Counting sprites
	unsigned int sprite_buffer_size{ 0 };
	for (const auto& tilelayer : m_map.tile_layers)
		sprite_buffer_size += tilelayer.width * tilelayer.height;

	m_sprite_buffer.resize(sprite_buffer_size);

	// Loading objects
	unsigned int buffer_index{ 0 };
	for (const auto& tilelayer : m_map.tile_layers)
	{
		unsigned int count{ 0 };
		for (const auto& tile : tilelayer.ids)
		{
			m_sprite_buffer[buffer_index].position.x = 16 * (count % tilelayer.width);
			m_sprite_buffer[buffer_index].position.y = 16 * (count / tilelayer.width);

			// No tile set
			m_sprite_buffer[buffer_index].set_color(float4(1.f, 1.f, 1.f, 1.f));
			if (tile != 0)
			{
				unsigned int texture_index{ 0 };
				unsigned int new_id{ 0 };
				for (const auto& tileset : m_map.tile_sets)
				{
					if (tile >= tileset.first_id && tile <= (tileset.first_id + tileset.width * tileset.height))
					{
						new_id = tile - tileset.first_id;
						break;
					}
					++texture_index;
				}

				m_sprite_buffer[buffer_index].set_texture(m_texture_atlases[texture_index]);

				unsigned int x = ((new_id) * 16) % m_map.tile_sets[texture_index].width;
				unsigned int y = ((new_id) * 16) / m_map.tile_sets[texture_index].width * 16;
				m_sprite_buffer[buffer_index].set_texture_rect(x, y, 16, 16);

				m_sprite_buffer[buffer_index].set_rect(16, 16);
				m_sprite_buffer[buffer_index].scale = ssa::float2(1.1f, 1.1f );
			}
			else
			{
				m_sprite_buffer[buffer_index].set_color(float4(1.f, 1.f, 1.f, 0.f));
			}
			++count;
			++buffer_index;
		}
	}

	ssa::loaded_data texture_data;
	unsigned long width, height;
	if (!ssa::load_png("assets/map.png", texture_data, width, height))
		std::cout << "Failed to load map texture" << std::endl;

	m_map_texture = new Texture(p_context.render_device);
	if (!m_map_texture->create(width, height, ssa::Format::RGBA8Unorm, false, &texture_data[0]))
		std::cout << "Failed to create map texture" << std::endl;

	m_base_blur = new BlurEffect(p_context.render_device);
	//p_context.renderer2D.push_effect(*m_base_blur);

	m_background.init(p_context.render_device, p_context.renderer2D);
	m_fragment_manager.init(m_map, p_context.render_device, p_context.renderer2D);

	m_map_sprite.set_texture(*m_map_texture);
}

void GameScreen::on_deactivation(GameContext& p_context)
{

}

void GameScreen::update(GameContext& p_context, unsigned int p_milliseconds, bool p_is_hidden)
{
	m_player_emitter.position = m_player_sprite.position;

	m_physics.set_delta_time(p_milliseconds);
	m_physics.preprocess();
	for (auto& object : m_map.objects)
	{
		m_physics.process(object);
	}
	m_physics.postprocess();
	if (m_player_component.position.x <= 0)
		m_player_component.position.x = 1;
	if (m_player_component.position.x >= (m_map.width - 1) * 16)
		m_player_component.position.x = (m_map.width -1 ) * 16 ;
	if (m_player_component.position.y <= 0)
		m_player_component.position.y = 1;
	if (m_player_component.position.y >= (m_map.height - 1) * 16)
		m_player_component.position.y = (m_map.height - 1) * 16;

	m_player_emitter.update(p_milliseconds);
	m_fragment_manager.update(m_player_component.position, ssa::float2(15,15), p_milliseconds);
	m_background.update(p_milliseconds);

	if (m_fragment_manager.is_over())
		exit(1);
}


void GameScreen::render(GameContext& p_context, unsigned int p_milliseconds)
{
	m_background.render();

	float displacement_x{ 0 };
	float displacement_y{ 0 };

	if (m_player_component.position.x >= 1280.f / 2 &&
		m_player_component.position.x <= m_map.width * 16 - 1280.f / 2)
	{
		displacement_x = 0 - m_player_component.position.x + 1280.f / 2;
		m_player_sprite.position.x = 1280.f / 2;
	}
	else
	{
		if (m_player_component.position.x < 1280.f / 2)
		{
			m_player_sprite.position.x = m_player_component.position.x;
		}
		else
		{
			m_player_sprite.position.x = 1280.f - (m_map.width * 16 - m_player_component.position.x);
			displacement_x = 0 - (m_map.width * 16 - 1280.f / 2) + 1280.f / 2;
		}
	}

	if (m_player_component.position.y >= 720.f / 2 &&
		m_player_component.position.y <= m_map.height * 16 - 720.f / 2)
	{
		displacement_y = 0 - m_player_component.position.y + 720.f / 2;
		m_player_sprite.position.y = 720.f / 2;
	}
	else
	{
		if (m_player_component.position.y < 720.f / 2)
		{
			m_player_sprite.position.y = m_player_component.position.y;
		}
		else
		{
			m_player_sprite.position.y = 720.f - (m_map.height * 16 - m_player_component.position.y);
			//displacement_y = 0 - (m_map.height * 16 - 720.f / 2) + 720.f / 2;
			displacement_y = 0 - (m_map.height * 16 - 720.f);
		}
	}

	p_context.renderer2D.begin(Renderer2D::SortMode::None);

	for (auto& sprite : m_sprite_buffer)
	{
		sprite.position.x += displacement_x;
		sprite.position.y += displacement_y;
	}

	unsigned int layer_count = m_sprite_buffer.size() / (m_map.width * m_map.height);
	for (unsigned int i = 0; i < layer_count; ++i)
	{
		// Displacement is in float, rounding it to nearest 
		int start_x = std::floor(-displacement_x / 16);
		int start_y = std::floor(-displacement_y / 16);

		int base_index = (m_map.width * m_map.height * i) + m_map.width * start_y + start_x;
		if (start_x + 1280 / 16 == m_map.width &&
			start_y + 720 / 16 == m_map.height)
		{
			for (int row = 0; row < 720 / 16; ++row)
			{
				for (int col = 0; col < 1280 / 16; ++col)
				{
					int index = base_index + m_map.width * row + col;
					p_context.renderer2D.render(m_sprite_buffer[index]);
				}
			}
		}
		else if (start_x + 1280 / 16 == m_map.width &&
			start_y + 720 / 16 != m_map.height)
		{
			for (int row = 0; row <= 720 / 16; ++row)
			{
				for (int col = 0; col < 1280 / 16; ++col)
				{
					int index = base_index + m_map.width * row + col;
					p_context.renderer2D.render(m_sprite_buffer[index]);
				}
			}
		}
		else if (start_x + 1280 / 16 != m_map.width &&
			start_y + 720 / 16 == m_map.height)
		{
			for (int row = 0; row < 720 / 16; ++row)
			{
				for (int col = 0; col <= 1280 / 16; ++col)
				{
					int index = base_index + m_map.width * row + col;
					p_context.renderer2D.render(m_sprite_buffer[index]);
				}
			}
		}
		else
		{
			for (int row = 0; row <= 720 / 16; ++row)
			{
				for (int col = 0; col <= 1280 / 16; ++col)
				{
					int index = base_index + m_map.width * row + col;
					p_context.renderer2D.render(m_sprite_buffer[index]);
				}
			}
		}
	}

	p_context.renderer2D.render(m_player_sprite);

	m_map_sprite.position += ssa::float2(displacement_x, displacement_y);
	p_context.renderer2D.render(m_map_sprite);
	p_context.renderer2D.end();
	m_map_sprite.position -= ssa::float2(displacement_x, displacement_y);

	for (auto& sprite : m_sprite_buffer)
	{
		sprite.position.y -= displacement_y;
		sprite.position.x -= displacement_x;
	}

	m_fragment_manager.render(ssa::float2(displacement_x, displacement_y));

	////m_particle_system.render(p_milliseconds);
}