//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

namespace ssa
{
	// Base class for anything that will be bound to the pipeline, it simply gives an id to each
	// instance for easy state caching
	class PipelineResource
	{
	public:
		typedef std::size_t id_t;

		// I'd like to use numeric_limits, but it appears to be bugged here on vs :S
		static const id_t invalid_id{ ULONG_MAX };

	public:
		PipelineResource() : id{ m_id_counter++ } { }

		id_t id;

	private:
		static id_t m_id_counter;
	};

	// All supported formats, they are for the most part ( all at the time of writing ) 
	// an intersection between the formats of all the supported APIs
	enum class Format
	{
		Unknown,
		RGBA8Unorm,
		RGBA32Float,
	};

	enum TextureCapabilities
	{
		None =				0x00,
		Surface =			0x01,
		ShaderResource =	0x02,
		RenderTarget =		0x04,
		RenderWindow =		0x08,
		HasDepthBuffer =	0x10
	};

	// Type of sampling to be used when sampling textures
	enum class SamplerFilterType
	{
		Point, // Usually know as nearest neighbor [ GO MINECRAFT! ]
		Linear,
		Anisotropic
	};

	// Macros when compiling shaders, they simply provide a list of macros that will 
	// be passed to the compiler
	typedef std::vector<std::pair<std::string, std::string>> shader_macro_t;

	// Type of the Shader, all types are listed here, somes ( e.g. Geometry ) might not
	// be supported on all platforms
	enum class ShaderType
	{
		Vertex,
		Geometry,
		Pixel
	};

	// Type of a ShaderVariable, as with the ShaderType, they are all list here, but some 
	// might not be supported
	enum class ShaderVariableType
	{
		Bool,
		Int,
		UnsignedInt,
		Float,
		Sampler,
		Texture,
	};

	// Type of Buffers
	enum class BufferType
	{
		Vertex,
		Index,
		Constant // Only supported on D3D/HLSL
	};

	// Specified what to do with data already present in the resource when updating it
	enum class UpdateType
	{
		Discard,
		NoOverwrite,
	};

	// Describes the way to render primitives contained in the vertex buffer
	enum class PrimitiveTopologyType
	{
		TriangleList
	};

	// Informations about the texture that can be pulled out of the graphics adapter
	struct TextureData
	{
		unsigned int width;
		unsigned int height;
		Format		 format;
	};

	enum class BlendType
	{
		None,
		Additive,
		Alpha
	};
}