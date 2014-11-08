//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// D3D
#include <D3D11.h>

// C++ STD
#include <string>
#include <vector>
#include <unordered_map>

// ssa
#include "ssa_commander_params.hpp"

namespace ssa
{
	// Internal Representation of a texture in D3D, Textures here in the stegosaurus are a quite different concept.
	// A texture is any kind of surface that can either be 2D or 3D ( not supported at the time of writing ). This means
	// that textures, render targets, and even windows are the same, this allows for a lot of powerful stuff ( easier to code too ).
	// When a texture is created it is generated with the maximum number of capabilities ( TextureCapabilities ) possible [ it usually depends on the kind of paramters passed
	struct TextureInternal
	{
		TextureInternal() :
		capabilities{ TextureCapabilities::None },
		texture_handle{ nullptr },
		shader_view{ nullptr },
		render_target_view{ nullptr },
		swap_chain{ nullptr },
		depth_stencil_view{ nullptr } { }

		unsigned int				capabilities; // bit-flag of TextureCapabilities

		// Handle to the D3D Resource
		ID3D11Texture2D*			texture_handle;
		
		// Valid if ShaderResource capability is flagged as true
		ID3D11ShaderResourceView*	shader_view;

		// Valid if RenderTarget capability is flagged as true
		ID3D11RenderTargetView*		render_target_view;

		// Valid if RenderWindow capability is flagged as true
		IDXGISwapChain* swap_chain;

		// Valid if HasDepthBuffer capability is flagged as true
		ID3D11DepthStencilView* depth_stencil_view;

		TextureData	data;
	};

	// Native window handle on windows.. uahu an opaque type
	// U DON'T SAY!
	typedef HWND native_window_handle_t;

	// Internal representation of a sampler that describes how a textures should be sample
	struct SamplerInternal : PipelineResource
	{
		SamplerInternal() :
		sampler_handle{ nullptr } { } 

		// D3D handle
		ID3D11SamplerState* sampler_handle;
		SamplerFilterType	filter;
	};

	// Internal representation of a buffer on D3D the Constant ( as type ) is valid
	struct BufferInternal : PipelineResource
	{
		BufferInternal() :
		stride{ 0 },
		buffer_handle{ nullptr } { } 

		// Type of the buffer
		BufferType type;

		// Size of the single element or of the entire buffer if composed by a single element
		std::size_t stride;

		// D3D handle to the buffer
		ID3D11Buffer* buffer_handle;
	};

	// Ok, this one is quite huge too, what does it contain ? :
	// - All variables, buffers and samplers
	// - Mapping of names to variables ( for easy access )
	// - InputLayout valid only if type is shader
	// - More shitty stuff
	struct ShaderInternal : PipelineResource
	{
		// Shader variable's data
		struct Variable
		{
			// Name of the variables, used also for indexing
			std::string name;

			// Type of the variable
			ShaderVariableType type;

			// Size in bytes of the variable, for more complex types, such as structures, it will ( not implemented yet )
			// indicate the size of the entire structure
			std::size_t	size;

			// [ Only valid for simple variables ] Offset of the parent structure
			unsigned int offset;

			// Size of array / vector / matrix row
			unsigned int dim1;

			// Matrix cols
			unsigned int dim2;

			// Bind point for resources or simply index of the cbuffer [ known as slot ]
			unsigned int bind_point; 
		};

		// Constant buffer used to group up variables
		struct ConstantBuffer : BufferInternal
		{
			// Index of the cbuffer in the shader
			unsigned int				slot;

			// Raw buffer used to update variables
			std::vector<unsigned char>	raw_buffer;

			// Used as flag to know when to update buffer
			bool						dirty;
		};

		ShaderInternal() :
			shader_handle{ nullptr },
			input_layout{ nullptr } { } 

		// Type of the shader ( see ShaderType for more info )
		ShaderType		   type;

		// Code used to generate the shader, it's kept here for information purposes, no real need
		// can be removed in the future
		std::string		   code;

		// D3D handle to the shader's base class
		ID3D11DeviceChild* shader_handle;

		// D3D handle to the input layout, valid only if shader type is vertex, nullptr otherwise
		ID3D11InputLayout* input_layout;

		// Dictionary of variables
		std::unordered_map<std::string, Variable> variables;

		// Constant buffers used to map variables
		std::vector<ConstantBuffer> constant_buffers;

		// Textures bound to the shader
		std::vector<const TextureInternal*> textures;

		// Samplers bound to the shader
		std::vector<const SamplerInternal*> samplers;
	};

	// Representation of the blending equation to apply for the rendering operations on the current
	// render targets.
	// 'Blender' as name kinda sucks, should change it
	struct BlenderInternal
	{
		BlenderInternal() :
		blender_handle{ nullptr } { } 

		// D3D Handle to the blend
		ID3D11BlendState* blender_handle;

		BlendType type;
	};

	struct GraphicsCapabilities
	{
		// MSAA Capability [ MSAA Support is not really planned out yet ], since it requires some overall 
		// attention. It will be done, but instead you should use FXAA ( postprocess ) for antialiasing
		unsigned int msaa;
		
		// Major version of the API, in D3D this corresponds to the feature level
		unsigned int version_major;

		// Minor version of the API, in D3D this corresponds to the feature level
		unsigned int version_minor;

		// True if geometry shaders are available [ D3D10+ ]
		bool		 geometry_shaders_enabled;
	};
}