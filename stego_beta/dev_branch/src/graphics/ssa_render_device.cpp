//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/ssa_render_device.hpp>
#include <graphics/ssa_buffer.hpp>
#include <graphics/ssa_shader.hpp>
#include <graphics/ssa_sampler.hpp>
#include <graphics/ssa_blender.hpp>
#include <graphics/ssa_texture.hpp>

namespace ssa
{
	RenderDevice::RenderDevice() :
		m_commander{},
		m_resource_factory{ m_commander }
	{

	}

	RenderDevice::~RenderDevice()
	{

	}

	bool RenderDevice::init(unsigned int p_adapter_index)
	{
		if (!m_commander.init(p_adapter_index))
			return false;

		return true;
	}

	///////////////////////////////////////////////////////////////////////
	/// CREATION
	///////////////////////////////////////////////////////////////////////

	bool RenderDevice::create_texture(unsigned int p_width, unsigned int p_height, Format p_format, bool p_dynamic, const void* p_data, Texture& p_texture)
	{
		std::size_t new_texture;
		if (p_texture.is_valid())
			new_texture = p_texture.get_id();
		else
			new_texture = m_resource_factory.create_texture();
		
		if (!m_commander.create_texture(p_width, p_height, p_format, p_dynamic, p_data, m_resource_factory.get_texture(new_texture)))
		{
			m_resource_factory.destroy_texture(new_texture);
			return false;
		}

		// Setting Resource's data
		p_texture.m_id = new_texture;

		// Setting Texture's data
		p_texture.m_data.width = p_width;
		p_texture.m_data.height = p_height;
		p_texture.m_data.format = p_format;

		return true;
	}

	bool RenderDevice::create_render_window(Window& p_window, Texture& p_render_window)
	{
		std::size_t new_render_window;
		if (p_render_window.is_valid())
			new_render_window = p_render_window.get_id();
		else
			new_render_window = m_resource_factory.create_texture();

		if (!m_commander.create_render_window(p_window, m_resource_factory.get_texture(new_render_window)))
		{
			m_resource_factory.destroy_texture(new_render_window);
			return false;
		}

		// Setting Resource's data
		p_render_window.m_id = new_render_window;

		// Setting Texture's data
		TextureData texture_data;
		if (!m_commander.query_texture_info(m_resource_factory.get_texture(new_render_window), texture_data))
			return false;

		p_render_window.m_data = texture_data;

		return true;
	}

	void RenderDevice::destroy_texture(Texture& p_texture)
	{
		if (p_texture.is_valid())
			m_commander.destroy_texture(m_resource_factory.get_texture(p_texture.get_id()));
	}

	bool RenderDevice::create_sampler(SamplerFilterType p_filter, Sampler& p_sampler)
	{
		std::size_t new_sampler;
		if (p_sampler.is_valid())
			new_sampler = p_sampler.get_id();
		else
			new_sampler = m_resource_factory.create_sampler();

		if (!m_commander.create_sampler(SamplerFilterType::Anisotropic, m_resource_factory.get_sampler(new_sampler)))
		{
			m_resource_factory.destroy_sampler(new_sampler);
			return false;
		}

		// Setting Resource's data
		p_sampler.m_id = new_sampler;

		// Setting Sampler's data
		p_sampler.m_filter = m_resource_factory.get_sampler(new_sampler).filter;

		return true;
	}

	void RenderDevice::destroy_sampler(Sampler& p_sampler)
	{
		if (p_sampler.is_valid())
			m_commander.destroy_sampler(m_resource_factory.get_sampler(p_sampler.get_id()));
	}

	bool RenderDevice::create_buffer(BufferType p_type, std::size_t p_element_size, std::size_t p_element_count, bool p_dynamic, const void* p_data, Buffer& p_buffer)
	{
		std::size_t new_buffer;
		if (p_buffer.is_valid())
			new_buffer = p_buffer.get_id();
		else
			new_buffer = m_resource_factory.create_buffer();

		if (!m_commander.create_buffer(p_type, p_element_size, p_element_count, p_dynamic, p_data, m_resource_factory.get_buffer(new_buffer)))
		{
			m_resource_factory.destroy_buffer(new_buffer);
			return false;
		}

		// Setting Resource's data
		p_buffer.m_id = new_buffer;

		// Setting Buffer's data
		p_buffer.m_type = p_type;
		p_buffer.m_stride = p_element_size;

		return true;
	}

	void RenderDevice::destroy_buffer(Buffer& p_buffer)
	{
		if (p_buffer.is_valid())
			m_commander.destroy_buffer(m_resource_factory.get_buffer(p_buffer.get_id()));
	}

	bool RenderDevice::create_shader(ShaderType p_type, const std::string& p_code, const std::string& p_entry_point, const std::vector<std::pair<std::string, std::string>>& p_macros, Shader& p_shader)
	{
		std::size_t new_shader;
		if (p_shader.is_valid())
			new_shader = p_shader.get_id();
		else
			new_shader = m_resource_factory.create_shader();

		if (!m_commander.create_shader(p_type, p_code, p_entry_point, p_macros, m_resource_factory.get_shader(new_shader)))
		{
			m_resource_factory.destroy_shader(new_shader);
			return false;
		}

		// Setting Resource's data
		p_shader.m_id = new_shader;

		// Setting Shader's data
		p_shader.m_type = p_type;
		p_shader.m_code = p_code;

		return true;
	}

	void RenderDevice::destroy_shader(Shader& p_shader)
	{
		if (p_shader.is_valid())
			m_commander.destroy_shader(m_resource_factory.get_shader(p_shader.get_id()));
	}

	bool RenderDevice::create_blender(BlendType p_type, Blender& p_blender)
	{
		std::size_t new_blender;
		if (p_blender.is_valid())
			new_blender = p_blender.get_id();
		else
			new_blender = m_resource_factory.create_blender();
		
		if (!m_commander.create_blender(p_type, m_resource_factory.get_blender(new_blender)))
		{
			m_resource_factory.destroy_blender(new_blender);
			return false;
		}

		// Setting Resource's data
		p_blender.m_id = new_blender;

		p_blender.m_type = p_type;

		return true;
	}

	void RenderDevice::destroy_blender(Blender& p_blender)
	{
		if (p_blender.is_valid())
			m_commander.destroy_blender(m_resource_factory.get_blender(p_blender.get_id()));
	}

	bool RenderDevice::update_buffer(Buffer& p_buffer, UpdateType p_type, const void* p_data, std::size_t p_size)
	{
		if (p_buffer.is_valid())
		{
			auto& internal_buffer = m_resource_factory.get_buffer(p_buffer.get_id());

			return m_commander.update_buffer(internal_buffer, p_type, p_data, p_size);
		}

		return false;
	}

	///////////////////////////////////////////////////////////////////////
	/// BINDING
	///////////////////////////////////////////////////////////////////////
	bool RenderDevice::bind_buffer(const Buffer& p_buffer)
	{
		const BufferInternal& buffer_internal = m_resource_factory.get_buffer(p_buffer.get_id());
		
		return m_commander.bind_buffer(buffer_internal);
	}
	
	bool RenderDevice::bind_shader(Shader& p_shader)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());
	
		return m_commander.bind_shader(shader_internal);
	}

	bool RenderDevice::push_target(Texture& p_render_target, bool p_bind_depth, bool p_stack)
	{
		// If we have no render targets bound we ignore the stack parameter
		if (m_render_target_stack.empty())
		{
			m_render_target_stack.push_back(RenderTargetBlock());
			m_render_target_stack.back().push_back(p_render_target);
		}
		else
		{
			if (p_stack)
				m_render_target_stack.back().push_back(p_render_target);
			else
			{
				m_render_target_stack.push_back(RenderTargetBlock());
				m_render_target_stack.back().push_back(p_render_target);
			}
		}

		// Stacking up render targets
		std::vector<TextureInternal*> render_targets;
		for (auto& render_target : m_render_target_stack.back())
		{
			render_targets.push_back(&m_resource_factory.get_texture(render_target.get().get_id()));
		}

		return m_commander.bind_targets(&render_targets[0], render_targets.size(), p_bind_depth);
	}

	bool RenderDevice::pop_targets()
	{
		if (m_render_target_stack.empty())
			return false;

		m_render_target_stack.pop_back();

		if (m_render_target_stack.empty())
			return true;

		// Stacking up render targets
		std::vector<TextureInternal*> render_targets;
		for (auto& render_target : m_render_target_stack.back())
		{
			render_targets.push_back(&m_resource_factory.get_texture(render_target.get().get_id()));
		}

		return m_commander.bind_targets(&render_targets[0], render_targets.size(), false);

	}

	bool RenderDevice::bind_blender(const Blender& p_blender)
	{
		const auto& blender = m_resource_factory.get_blender(p_blender.get_id());

		return m_commander.bind_blender(blender);
	}

	void RenderDevice::unbind_blender()
	{
		m_commander.unbind_blender();
	}

	bool RenderDevice::set_value(Shader& p_shader, const std::string& p_name, const void* p_data, std::size_t p_size)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());

		return m_commander.set_value(shader_internal, p_name, p_data, p_size);
	}

	bool RenderDevice::set_texture(Shader& p_shader, const std::string& p_name, const Texture& p_texture)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());
		auto& texture_internal = m_resource_factory.get_texture(p_texture.get_id());

		return m_commander.set_value(shader_internal, p_name, &texture_internal, 0);
	}
	
	bool RenderDevice::set_sampler(Shader& p_shader, const std::string& p_name, const Sampler& p_sampler)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());
		auto& sampler_internal = m_resource_factory.get_sampler(p_sampler.get_id());

		return m_commander.set_value(shader_internal, p_name, &sampler_internal, 0);
	}

	void RenderDevice::unset_texture(Shader& p_shader, const std::string& p_name)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());
		m_commander.unset_value(shader_internal, p_name);
	}
	
	void RenderDevice::unset_sampler(Shader& p_shader, const std::string& p_name)
	{
		auto& shader_internal = m_resource_factory.get_shader(p_shader.get_id());
		m_commander.unset_value(shader_internal, p_name);
	}

	///////////////////////////////////////////////////////////////////////
	/// DRAWING
	///////////////////////////////////////////////////////////////////////
	bool RenderDevice::clear(const Texture& p_render_target, float4 p_color)
	{
		const auto& render_target_internal = m_resource_factory.get_texture(p_render_target.get_id());

		return m_commander.clear(render_target_internal, p_color);
	}

	bool RenderDevice::clear_depth(const Texture& p_render_target, float p_depth)
	{
		const auto& render_target_internal = m_resource_factory.get_texture(p_render_target.get_id());

		return m_commander.clear_depth(render_target_internal, p_depth);
	}

	bool RenderDevice::draw(PrimitiveTopologyType p_primitive_topology, unsigned int p_vertices_count, unsigned int p_vertices_offset)
	{
		return m_commander.draw(p_primitive_topology, p_vertices_count, p_vertices_offset);
	}

	bool RenderDevice::finalize(const Texture& p_render_window, bool p_vsync)
	{
		const auto& render_window_internal = m_resource_factory.get_texture(p_render_window.get_id());

		return m_commander.finalize(render_window_internal, p_vsync);
	}
}