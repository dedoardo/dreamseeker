//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../core/ssa_math.hpp"
#include "ssa_commander_internals.hpp"
#include "ssa_commander_params.hpp"

namespace ssa
{
	// Forward declaration
	class Window;

	//! \brief Base interface for each underlying graphics API that is implemented in the respective CPP file
	class ssa_export Commander
	{
	public:
		Commander();
		virtual ~Commander();

		//! \brief Initializes the commander attaching it to the specified adapter
		bool init(unsigned int p_adapter_index);

		// @TODO : Implement adapter list

		///////////////////////////////////////////////////////////////////////
		/// CREATION
		///////////////////////////////////////////////////////////////////////
	
		//! \brief Creates a new texture
		//!	
		//! Generates a new texture and tries to assign ShaderResource && RenderTarget && HasDepthBuffer based on parameters [ format ] passed
		//! @TODO : Add more info, based on how parameters are generated
		//! \param [in] p_width Width of the texture
		//! \param [in] p_height Height of the texture
		//! \param [in] p_format Format describing how pixel are arranged
		//! \param [in] p_data Data, nullptr is *valid* there is no need to initialize the texture with something
		//! \param [in] p_dynamic True if texture will be updated frequently, on a per-frame basis
		//! \param [out] p_texture Texture that will be created
		//! \return True if call was successful, false otherwise
		bool create_texture(unsigned int p_width, unsigned int p_height, Format p_format, bool p_dynamic, const void* p_data, TextureInternal& p_texture);

		//! \brief Creates a new Texture from an existing window
		//!
		//! The RenderWindow capability will be set to true ( if call is successful ofc ) 
		//! \param [in] p_handle Handle to the window, this is dependant on the operating system rather than the API
		//! \param [out] p_texture Texture that will be created
		//! \return True if call was successful, false otherwise
		bool create_render_window(Window& p_window, TextureInternal& p_render_window);

		//! \brief Releases all the resources associated with the texture
		//! \param [in] p_texture Texture to be released, will be invalid after
		void destroy_texture(TextureInternal& p_texture);

		//! \brief Creates a new sampler
		//!
		//! Filter is applied on all coordinates & operations [ @TODO: add more filtering options
		//! If anisotropic is specified the maximum level is used
		//! \param [in] p_filter Filter to be used
		//! \param [out] p_sampler Sampler to be created
		bool create_sampler(SamplerFilterType p_filter, SamplerInternal& p_sampler);

		//! \brief Releases all the resources associated with the sampler
		//! \param [in] p_texture Sampler to be released, will be invalid after
		void destroy_sampler(SamplerInternal& p_sampler);

		//! \brief Creates a new buffer 
		//! 
		//! [D3D] Constant buffer creation is allowed here to not cluster the code, but should not be used to create constant buffers
		//! \param [in] p_type Type of the buffer ( Vertex or Index ) 
		//! \param [in] p_element_size Size of the single element in the buffer
		//! \param [in] p_element_count Number of elements contained in the buffer. Size is calculated this way : p_element_size * p_element_count
		//! \param [in] p_dynamic True if the frame will be updated frequently, on a per-frame basis
		//! \param [in] p_data Data to initialize the buffer with, can be nullptr
		//! \param [out] p_buffer Buffer that will be created 
		//! \return True if call was successful, false otherwise
		bool create_buffer(BufferType p_type, std::size_t p_element_size, std::size_t p_element_count, bool p_dynamic, const void* p_data, BufferInternal& p_buffer);

		//! \brief Releases all the resources associated with the buffer
		//! \param [in] p_texture Buffer to be released, will be invalid after
		void destroy_buffer(BufferInternal& p_buffer);

		//! \brief Compiles and Reflects a shader with the specified code
		//!
		//!	Unfortunately right now shader bytecode is not supported due to API differences [ D3D / OGL ], so no pre-compilation is valid
		//! It will create the shader with the highest possible shader value
		//! \param [in] p_type Type of the shader
		//! \param [in] p_code Raw text string containg the code [ NO BYTECODE | PRECOMPILED ]
		//! \param [in] p_entry_point Name of the function that should be called when starting the shader
		//! \param [in] p_macros Array of macros that should be set when compiling the shader [ @TEST : Should work tho ]
		//! \param [out] p_shader Shader that will be created
		bool create_shader(ShaderType p_type, const std::string& p_code, const std::string& p_entry_point, const std::vector<std::pair<std::string, std::string>>& p_macros, ShaderInternal& p_shader);

		//! \brief Releases all the resources associated with the shader
		//! \param [in] p_texture Shader to be released, will be invalid after
		void destroy_shader(ShaderInternal& p_shader);

		//! @TODO: Comment and add more options
		bool create_blender(BlendType p_type, BlenderInternal& p_blender);

		//! \brief Releases all the resources associated with the blender
		//! \param [in] p_texture Blender to be released, will be invalid after
		void destroy_blender(BlenderInternal& p_blender);

		//! \brief Updates the content of the buffer on the GPU, this is quite an expensive operation
		//!
		//! @TODO : Add support for updating non -dynamic textures ( can you do it in OpenGL ? ) 
		//! \param [out] p_buffer Buffer whose content will be udpated
		//! \param [in] p_type Describes how to treat already existing data 
		//! \param [in] p_data Data to update the buffer with, it cannot be nullptr
		//! \return True if call was successful, false otherwise
		bool update_buffer(BufferInternal& p_buffer, UpdateType p_type, const void* p_data, std::size_t p_size);

		///////////////////////////////////////////////////////////////////////
		/// QUERYING
		///////////////////////////////////////////////////////////////////////
		bool query_texture_info(const TextureInternal& p_texture, TextureData& p_data);

		///////////////////////////////////////////////////////////////////////
		/// BINDING
		///////////////////////////////////////////////////////////////////////
		//! \brief Binds a vertex buffer or an index buffer to the pipeline
		//! \param [in] p_buffer Vertex or Index buffer to be bound
		//! \return True if buffer is already bound or if binding is successful
		bool bind_buffer(const BufferInternal& p_buffer);

		//! @TODO : implement
		void unbind_buffer(BufferType p_type);

		//! \brief Binds a Shader to the pipeline
		//! \param [in] p_shader Shader to be bound
		//! \return True if sahder is already bound or if binding is successful
		bool bind_shader(ShaderInternal& p_shader);

		//! @TODO : implement
		void unbind_shader(ShaderType p_type);

		//! \brief Binds a RenderTarget capable texture as main render target
		//! \param [in] p_render_target Texture with RenderTarget capability that will be bound
		//! \param [in] p_bind_depth If true the associated depth buffer will be bound too
		//! \return True if binding was successful
		bool bind_target(const TextureInternal& p_render_target, bool p_bind_depth);

		//! \brief Binds multiple render targets 
		//! @TODO : implement
		//! \param [in] p_render_targets Array of render targets
		//! \param [in] p_count Size of the array
		//! \param [in] p_bind_depth If true the associated depth buffer will be bound too
		//! \return True if binding was successful, false otherwise
		bool bind_targets(TextureInternal * const* p_render_targets, std::size_t p_count, bool p_bind_depth);

		//! \brief Unbinds all the currenly bound render targets
		void unbind_targets();

		//! \brief Binds the blender to the pipeline
		//! \param [in] p_blender Blender to be bound
		//! \return True if is already bound or if binding was succcessful, false otherwise
		bool bind_blender(const BlenderInternal& p_blender);

		//! @TODO : Implement
		void unbind_blender();

		//! \brief Sets a value of the shader, if the shader is bound the update will be immediate
		//!		otherwise it will be deferred to the shader's binding time
		//! \param [in] p_shader Shader containing the variable
		//! \param [in] p_name Name of the variable
		//! \param [in] p_data Data of the variable, this should be raw data for normal variables
		//!		otherwise SamplerInternal* for samplers, TextureInternal* for textures
		//! \param [in] p_size Size of the data, valid only for normal variables, ignored for resources
		//! \return True if setting was successful, false otherwise
		bool set_value(ShaderInternal& p_shader, const std::string& p_name, const void* p_data, std::size_t p_size);

		//! \brief Unsets a variable previously set, this should be used for textures / samplers, 
		//!		for normal variables
		//! \param [in] p_shader Shader the variable is associated to
		//! \param [in] p_name Name of the variable
		void unset_value(ShaderInternal& p_shader, const std::string& p_name);

		///////////////////////////////////////////////////////////////////////
		/// DRAWING
		///////////////////////////////////////////////////////////////////////
		//! \brief Clears the RenderTarget ( Texture with RT capability )
		//! \param [in] p_render_target RenderTarget to be cleared
		//! \param [in] p_color Color the RenderTarget will be cleared with
		bool clear(const TextureInternal& p_render_target, float4 p_color);

		//! \brief Clears the depth buffer associated with the RenderTarget, must have HasDepthBuffer set to true
		//! \param [in] p_render_target RenderTarget whose depth buffer ( HasDepthBuffer compatibility needed ) will be cleared
		//! \param [in] p_depth Value to clear the RenderTarget with
		bool clear_depth(const TextureInternal& p_render_target, float p_depth);
		
		//! \brief Issues a non-indexed draw call 
		//! \param [in] p_primitive_topology Defines how data in the bound vertex buffer should be interpreted
		//! \param [in] p_vertices_count Number of vertices to draw ( 0 < p_vertices_count < N* of vertices in the bound vertex buffer )
		//! \param [in] p_offset Offset to start counting in the vertex buffer
		bool draw(PrimitiveTopologyType p_primitive_topology, unsigned int p_vertices_count, unsigned int p_vertices_offset);

		//! \brief Finalizes the rendering on the specified RenderWindow, Texture Must have a RenderWindow capability
		//! \param [in] p_render_window Swaps buffers of the specified RenderWindow
		//! \param [in] p_vsync True to enable VSync ( this will maximaze framerate )
		//! \return True if swapping was successful
		bool finalize(const TextureInternal& p_render_window, bool p_vsync);

	protected:
		GraphicsCapabilities	m_capabilities;
		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_device_context;
		IDXGIAdapter*			m_adapter;
		IDXGIFactory*			m_factory;

	private :
		PipelineResource::id_t	m_vertex_buffer;

		PipelineResource::id_t  m_vertex_shader;
		PipelineResource::id_t  m_geometry_shader;
		PipelineResource::id_t	m_pixel_shader;
	};
}