//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_resource_batch.hpp"
#include "ssa_commander.hpp"

namespace ssa
{
	class ssa_export ResourceFactory
	{
	public:
		ResourceFactory(Commander& p_commander);
		~ResourceFactory();

		ResourceBatch<TextureInternal>::index_t create_texture();
		TextureInternal& get_texture(ResourceBatch<TextureInternal>::index_t p_id);
		void destroy_texture(ResourceBatch<TextureInternal>::index_t p_id);

		ResourceBatch<BufferInternal>::index_t create_buffer();
		BufferInternal& get_buffer(ResourceBatch<BufferInternal>::index_t p_id);
		void destroy_buffer(ResourceBatch<BufferInternal>::index_t p_id);

		ResourceBatch<ShaderInternal>::index_t create_shader();
		ShaderInternal& get_shader(ResourceBatch<ShaderInternal>::index_t p_id);
		void destroy_shader(ResourceBatch<ShaderInternal>::index_t p_id);

		ResourceBatch<SamplerInternal>::index_t create_sampler();
		SamplerInternal& get_sampler(ResourceBatch<SamplerInternal>::index_t p_id);
		void destroy_sampler(ResourceBatch<SamplerInternal>::index_t p_id);

		ResourceBatch<BlenderInternal>::index_t create_blender();
		BlenderInternal& get_blender(ResourceBatch<BlenderInternal>::index_t p_id);
		void destroy_blender(ResourceBatch<BlenderInternal>::index_t p_id);

	private:
		Commander& m_commander;

		ResourceBatch<TextureInternal>			m_textures;
		ResourceBatch<BufferInternal>			m_buffers;
		ResourceBatch<ShaderInternal>			m_shaders;
		ResourceBatch<SamplerInternal>			m_samplers;
		ResourceBatch<BlenderInternal>			m_blenders;
	};
}