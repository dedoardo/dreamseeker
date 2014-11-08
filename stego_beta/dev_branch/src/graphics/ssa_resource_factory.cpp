//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/ssa_resource_factory.hpp>

namespace ssa
{
	ResourceFactory::ResourceFactory(Commander& p_commander) :
		m_commander{ p_commander },
		m_textures{ 10 },
		m_buffers{ 50 },
		m_shaders{ 10 },
		m_samplers{ 10 },
		m_blenders{ 10 }
	{

	}

	ResourceFactory::~ResourceFactory()
	{

	}

	ResourceBatch<TextureInternal>::index_t ResourceFactory::create_texture()
	{
		return m_textures.create_resource();
	}

	TextureInternal& ResourceFactory::get_texture(ResourceBatch<TextureInternal>::index_t p_id)
	{
		return m_textures.get(p_id);
	}

	void ResourceFactory::destroy_texture(ResourceBatch<TextureInternal>::index_t p_id)
	{
		m_textures.recycle(p_id);
	}

	ResourceBatch<BufferInternal>::index_t ResourceFactory::create_buffer()
	{
		return m_buffers.create_resource();
	}

	BufferInternal& ResourceFactory::get_buffer(ResourceBatch<BufferInternal>::index_t p_id)
	{
		return m_buffers.get(p_id);
	}

	void ResourceFactory::destroy_buffer(ResourceBatch<BufferInternal>::index_t p_id)
	{
		m_buffers.recycle(p_id);
	}

	ResourceBatch<ShaderInternal>::index_t ResourceFactory::create_shader()
	{
		return m_shaders.create_resource();
	}

	ShaderInternal& ResourceFactory::get_shader(ResourceBatch<ShaderInternal>::index_t p_id)
	{
		return m_shaders.get(p_id);
	}

	void ResourceFactory::destroy_shader(ResourceBatch<ShaderInternal>::index_t p_id)
	{
		m_shaders.recycle(p_id);
	}

	ResourceBatch<SamplerInternal>::index_t ResourceFactory::create_sampler()
	{
		return m_samplers.create_resource();
	}

	SamplerInternal& ResourceFactory::get_sampler(ResourceBatch<SamplerInternal>::index_t p_id)
	{
		return m_samplers.get(p_id);
	}

	void ResourceFactory::destroy_sampler(ResourceBatch<SamplerInternal>::index_t p_id)
	{
		m_samplers.recycle(p_id);
	}

	ResourceBatch<BlenderInternal>::index_t ResourceFactory::create_blender()
	{
		return m_blenders.create_resource();
	}

	BlenderInternal& ResourceFactory::get_blender(ResourceBatch<BlenderInternal>::index_t p_id)
	{
		return m_blenders.get(p_id);
	}

	void ResourceFactory::destroy_blender(ResourceBatch<BlenderInternal>::index_t p_id)
	{
		m_blenders.recycle(p_id);
	}
}