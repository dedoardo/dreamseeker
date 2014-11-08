//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/ssa_commander.hpp>

// ssa
#include <graphics/ssa_commander_utils.hpp>
#include <window/ssa_window.hpp>

// C++ STD
#include <cassert>

// D3D
#include <D3Dcompiler.h>

#include <iostream>

// Linking
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")

namespace ssa
{
	PipelineResource::id_t PipelineResource::m_id_counter{ 0 };

	Commander::Commander() :
		m_device{ nullptr },
		m_device_context{ nullptr },
		m_adapter{ nullptr },
		m_factory{ nullptr },

		m_vertex_buffer{ PipelineResource::invalid_id },

		m_vertex_shader{ PipelineResource::invalid_id },
		m_geometry_shader{ PipelineResource::invalid_id },
		m_pixel_shader{ PipelineResource::invalid_id }
	{

	}

	Commander::~Commander()
	{
		if (m_device != nullptr)
			m_device->Release();
		if (m_device_context != nullptr)
			m_device_context->Release();
		if (m_adapter != nullptr)
			m_adapter->Release();
		if (m_factory != nullptr)
			m_factory->Release();
	}

	bool Commander::init(unsigned int p_adapter_index)
	{
		D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3 // Windows Phone 8 D3D Feature Level
		};

		D3D_FEATURE_LEVEL feature_level;

		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
#ifdef _DEBUG
			D3D11_CREATE_DEVICE_DEBUG,
#else
			0,
#endif
			nullptr, 0,
			D3D11_SDK_VERSION, &m_device,
			&feature_level, &m_device_context);

		if (FAILED(hr))
			return false;

		IDXGIDevice* dxgi_device;
		hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
		if (FAILED(hr))
			return false;

		hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&m_adapter));
		if (FAILED(hr))
		{
			dxgi_device->Release();
			return false;
		}

		hr = m_adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_factory));
		if (FAILED(hr))
		{
			dxgi_device->Release();
			return false;
		}

		dxgi_device->Release();

		switch (feature_level)
		{
		case D3D_FEATURE_LEVEL_11_0:
			m_capabilities.version_major = 11;
			m_capabilities.version_minor = 0;
			m_capabilities.geometry_shaders_enabled = true;
			m_capabilities.msaa = 8;
			break;
		case D3D_FEATURE_LEVEL_10_1:
			m_capabilities.version_major = 10;
			m_capabilities.version_minor = 1;
			m_capabilities.geometry_shaders_enabled = true;
			m_capabilities.msaa = 4;
			break;
		case D3D_FEATURE_LEVEL_10_0:
			m_capabilities.version_major = 10;
			m_capabilities.version_minor = 0;
			m_capabilities.geometry_shaders_enabled = true;
			m_capabilities.msaa = 4;
			break;
		case D3D_FEATURE_LEVEL_9_3:
			m_capabilities.version_major = 9;
			m_capabilities.version_minor = 3;
			m_capabilities.geometry_shaders_enabled = false;
			m_capabilities.msaa = 4; // @TODO : Check to be sure
			break;
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////
	/// CREATION
	///////////////////////////////////////////////////////////////////////
	bool Commander::create_texture(unsigned int p_width, unsigned int p_height, Format p_format, bool p_dynamic, const void* p_data, TextureInternal& p_texture)
	{
		assert(m_device != nullptr);

		if (!p_width && !p_height)
			return false;

		destroy_texture(p_texture);
		p_texture = TextureInternal();

		// Creating Texture 
		D3D11_TEXTURE2D_DESC texture_desc;
		texture_desc.Width = p_width;
		texture_desc.Height = p_height;
		texture_desc.ArraySize = 1;
		texture_desc.Format = format_to_raw(p_format);

		//@ TODO : implement multisampling
		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;

		texture_desc.Usage = p_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		if (p_dynamic)
			texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		else
			texture_desc.CPUAccessFlags = 0;

		// @TODO : implement mipmapping
		texture_desc.MipLevels = 1;
		texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if (!p_dynamic) // Only *non* dynamic resources cannot be used as render targets
			texture_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags = 0;

		// Initial data 
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = p_data;
		data.SysMemPitch = p_width * 4; // TODO : implement compute_pixel_size
		data.SysMemSlicePitch = 0;

		HRESULT hr = m_device->CreateTexture2D(&texture_desc,
			p_data == nullptr ? nullptr : &data,
			&p_texture.texture_handle);
		
		if (FAILED(hr))
		{
			destroy_texture(p_texture);
			return false;
		}

		p_texture.capabilities |= TextureCapabilities::Surface;
		p_texture.data.width = p_width;
		p_texture.data.height = p_height;
		p_texture.data.format = p_format;

		// ShaderResource capability
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
		shader_resource_view_desc.Format = texture_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		shader_resource_view_desc.Texture2D.MostDetailedMip = 0;

		hr = m_device->CreateShaderResourceView(p_texture.texture_handle,
			&shader_resource_view_desc,
			&p_texture.shader_view);

		if (FAILED(hr))
		{
			destroy_texture(p_texture);
			return false;
		}

		p_texture.capabilities |= TextureCapabilities::ShaderResource;

		// This is simply to avoid nesting the code even further
		if (!(texture_desc.BindFlags & D3D11_BIND_RENDER_TARGET))
			return true; // @TODO: Issue a warning maybe ? 

		if (!is_render_target_format_valid(p_format))
			return true; // @TODO : Issue a warning

		hr = m_device->CreateRenderTargetView(p_texture.texture_handle,
			nullptr,
			&p_texture.render_target_view);
		if (FAILED(hr))
		{
			destroy_texture(p_texture);
			return false;
		}

		// @TODO: Add creation of depth view

		return true;
	}

	bool Commander::create_render_window(Window& p_window, TextureInternal& p_render_window)
	{
		assert(m_device != nullptr);
		assert(m_factory != nullptr);

		destroy_texture(p_render_window);
		p_render_window = TextureInternal();

		// Getting window's info 
		unsigned int width{ p_window.get_width() };
		unsigned int height{ p_window.get_height() };


		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.Width = width;
		swap_chain_desc.BufferDesc.Height = height;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Flags = 0;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.OutputWindow = p_window.get_handle();
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		if (p_window.is_fullscreen())
			swap_chain_desc.Windowed = false;
		else
			swap_chain_desc.Windowed = true;

		HRESULT hr = m_factory->CreateSwapChain(
			m_device,
			&swap_chain_desc,
			&p_render_window.swap_chain);
		if (FAILED(hr))
		{
			destroy_texture(p_render_window);
			std::abort();
			return false;
		}

		p_render_window.capabilities |= TextureCapabilities::RenderWindow;
		p_render_window.data.width = swap_chain_desc.BufferDesc.Width;
		p_render_window.data.height = swap_chain_desc.BufferDesc.Height;
		p_render_window.data.format = raw_to_format(swap_chain_desc.BufferDesc.Format);

		//! Getting the backbuffer
		hr = p_render_window.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&p_render_window.texture_handle));
		if (FAILED(hr))
		{
			destroy_texture(p_render_window);
			std::abort();
			return false;
		}

		p_render_window.capabilities |= TextureCapabilities::Surface;

		//! Creating the view to access it
		hr = m_device->CreateRenderTargetView(
			p_render_window.texture_handle,
			nullptr,
			&p_render_window.render_target_view);

		if (FAILED(hr))
		{
			destroy_texture(p_render_window);
			std::abort();
			return false;
		}

		p_render_window.capabilities |= TextureCapabilities::RenderTarget;

		//// Creating depth stencil view
		//ID3D11Texture2D* depth_texture;
		//D3D11_TEXTURE2D_DESC depth_text;

		//depth_text.Width = width;
		//depth_text.Height = height;
		//depth_text.MipLevels = 1;
		//depth_text.ArraySize = 1;
		//depth_text.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//depth_text.SampleDesc.Count = 1;
		//depth_text.SampleDesc.Quality = 0;

		//depth_text.Usage = D3D11_USAGE_DEFAULT;
		//depth_text.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//depth_text.CPUAccessFlags = 0;
		//depth_text.MiscFlags = 0;

		//hr = m_device->CreateTexture2D(&depth_text, nullptr, &depth_texture);
		//if (FAILED(hr))
		//{
		//	destroy_texture(p_render_window);
		//	return false;
		//}

		//hr = m_device->CreateDepthStencilView(depth_texture, nullptr, &p_render_window.depth_stencil_view);
		//if (FAILED(hr))
		//{
		//	destroy_texture(p_render_window);
		//	depth_texture->Release();
		//	return false;
		//}

		//depth_texture->Release();

		//p_render_window.capabilities |= TextureCapabilities::HasDepthBuffer;

		return true;
	}

	void Commander::destroy_texture(TextureInternal& p_texture)
	{
		assert(m_device_context != nullptr);

		p_texture.capabilities = TextureCapabilities::None;

		if (p_texture.texture_handle != nullptr)
			p_texture.texture_handle->Release();

		if (p_texture.shader_view != nullptr)
			p_texture.shader_view->Release();

		if (p_texture.render_target_view != nullptr)
			p_texture.render_target_view->Release();

		if (p_texture.swap_chain != nullptr)
			p_texture.swap_chain->Release();

		if (p_texture.depth_stencil_view != nullptr)
			p_texture.depth_stencil_view->Release();

		m_device_context->Flush();
	}

	bool Commander::create_sampler(SamplerFilterType p_filter, SamplerInternal& p_sampler)
	{
		assert(m_device != nullptr);

		destroy_sampler(p_sampler);
		p_sampler = SamplerInternal();

		D3D11_SAMPLER_DESC sampler_desc;
		std::memset(&sampler_desc, 0, sizeof(D3D11_SAMPLER_DESC));
		sampler_desc.AddressU = sampler_desc.AddressV = sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.Filter = filter_to_raw(p_filter);
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

		if (FAILED(m_device->CreateSamplerState(&sampler_desc, &p_sampler.sampler_handle)))
			return false;

		p_sampler.filter = p_filter;

		return true;
	}

	void Commander::destroy_sampler(SamplerInternal& p_sampler)
	{
		if (p_sampler.sampler_handle != nullptr)
			p_sampler.sampler_handle->Release();
	}

	bool Commander::create_buffer(BufferType p_type, std::size_t p_element_size, std::size_t p_element_count, bool p_dynamic, const void* p_data, BufferInternal& p_buffer)
	{
		assert(m_device != nullptr);

		destroy_buffer(p_buffer);
		p_buffer = BufferInternal();

		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = p_element_count * p_element_size;
		buffer_desc.Usage = p_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = p_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer_desc.MiscFlags = 0;

		if (p_type == BufferType::Vertex)
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		else if (p_type == BufferType::Index)
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		else if (p_type == BufferType::Constant)
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initial_data;
		if (p_data != nullptr)
			initial_data.pSysMem = p_data;

		HRESULT hr = m_device->CreateBuffer(&buffer_desc,
			p_data == nullptr ? nullptr : &initial_data,
			&p_buffer.buffer_handle);

		if (FAILED(hr))
			return false;

		p_buffer.stride = p_element_size;
		p_buffer.type = p_type;

		return true;
	}

	void Commander::destroy_buffer(BufferInternal& p_buffer)
	{
		p_buffer.stride = 0;
		if (p_buffer.buffer_handle != nullptr)
			p_buffer.buffer_handle->Release();
	}

	bool Commander::create_shader(ShaderType p_type, const std::string& p_code, const std::string& p_entry_point, const std::vector<std::pair<std::string, std::string>>& p_macros, ShaderInternal& p_shader)
	{
		assert(m_device != nullptr);

		destroy_shader(p_shader);
		p_shader = ShaderInternal();

		// Extrapolate macros
		std::vector<D3D_SHADER_MACRO> macros;
		for (const auto& macro : p_macros)
			macros.push_back(D3D_SHADER_MACRO{ macro.first.c_str(), macro.second.c_str() });

		// Deciding shader model based on D3D Version
		std::string target;
		if (p_type == ShaderType::Vertex)
		{
			target = "vs_5_0";
		}
		else if (p_type == ShaderType::Geometry)
		{
			target = "gs_5_0";
		}
		else if (p_type == ShaderType::Pixel)
		{
			target = "ps_5_0";
		}

		// Compilation stage
		ID3D10Blob* data;
		ID3D10Blob* error;

		HRESULT hr = D3DCompile(
			p_code.c_str(),
			p_code.size(),
			nullptr,
			macros.empty() ? nullptr : &macros[0],
			nullptr,
			p_entry_point.c_str(),
			target.c_str(), 0, 0,
			&data, &error);
		if (FAILED(hr))
		{
			std::string error_str{ static_cast<char*>(error->GetBufferPointer()) };
			error->Release();
			std::cout << error_str << std::endl;
			return false;
		}

		// Creating Shader Object
		if (p_type == ShaderType::Vertex)
			hr = m_device->CreateVertexShader(data->GetBufferPointer(), data->GetBufferSize(), nullptr, reinterpret_cast<ID3D11VertexShader**>(&p_shader.shader_handle));
		else if (p_type == ShaderType::Pixel)
			hr = m_device->CreatePixelShader(data->GetBufferPointer(), data->GetBufferSize(), nullptr, reinterpret_cast<ID3D11PixelShader**>(&p_shader.shader_handle));
		else if (p_type == ShaderType::Geometry)
			hr = m_device->CreateGeometryShader(data->GetBufferPointer(), data->GetBufferSize(), nullptr, reinterpret_cast<ID3D11GeometryShader**>(&p_shader.shader_handle));

		if (FAILED(hr))
		{
			data->Release();
			return false;
		}

		p_shader.code = p_code;
		p_shader.type = p_type;

		// Shader-Reflection stage
		ID3D11ShaderReflection* reflection;
		hr = D3DReflect(data->GetBufferPointer(),
			data->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			reinterpret_cast<void**>(&reflection));

		if (FAILED(hr))
		{
			data->Release();
			return false;
		}

		// Read input layout description from shader info
		D3D11_SHADER_DESC shader_desc;
		reflection->GetDesc(&shader_desc);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for (unsigned int i = 0; i< shader_desc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			reflection->GetInputParameterDesc(i, &paramDesc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			//save element desc
			inputLayoutDesc.push_back(elementDesc);
		}

		// Try to create Input Layout
		hr = m_device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), data->GetBufferPointer(), data->GetBufferSize(), &p_shader.input_layout);

		if (FAILED(hr))
		{
			reflection->Release();
			return false;
		}

		// Now the fun begins and we will try to list all the variables in the shader
		for (unsigned int cbuffer_index{ 0 }; cbuffer_index < shader_desc.ConstantBuffers; ++cbuffer_index)
		{
			auto cbuffer = reflection->GetConstantBufferByIndex(cbuffer_index);
			if (cbuffer == nullptr)
			{
				reflection->Release();
				return false;
			}
			
			D3D11_SHADER_BUFFER_DESC cbuffer_desc;
			if (FAILED(cbuffer->GetDesc(&cbuffer_desc)))
			{
				reflection->Release();
				return false;
			}

			// Adding constant buffer

			ShaderInternal::ConstantBuffer cbuffer_internal;
			cbuffer_internal.dirty = true;
			cbuffer_internal.slot = cbuffer_index;
			cbuffer_internal.type = BufferType::Constant;
			cbuffer_internal.stride = cbuffer_desc.Size;
		
			p_shader.constant_buffers.push_back(cbuffer_internal);

			p_shader.constant_buffers.back().raw_buffer.resize(cbuffer_desc.Size);

			if (!create_buffer(BufferType::Constant, cbuffer_desc.Size, 1, true, nullptr, p_shader.constant_buffers.back()))
			{
				reflection->Release();
				return false;
			}

			// now adding all the internal variables
			for (unsigned int cbuffer_var_index{ 0 }; cbuffer_var_index < cbuffer_desc.Variables; ++cbuffer_var_index)
			{
				auto cbuffer_var = cbuffer->GetVariableByIndex(cbuffer_var_index);
				if (cbuffer_var == nullptr)
				{
					reflection->Release();
					return false;
				}

				D3D11_SHADER_VARIABLE_DESC cbuffer_var_desc;
				if (FAILED(cbuffer_var->GetDesc(&cbuffer_var_desc)))
				{
					reflection->Release();
					return false;
				}

				auto cbuffer_var_type = cbuffer_var->GetType();
				if (cbuffer_var_type == nullptr)
				{
					reflection->Release();
					return false;
				}

				D3D11_SHADER_TYPE_DESC cbuffer_var_type_desc;
				if (FAILED(cbuffer_var_type->GetDesc(&cbuffer_var_type_desc)))
				{
					reflection->Release();
					return false;
				}
				
				ShaderInternal::Variable new_variable;
				
				new_variable.name = cbuffer_var_desc.Name;
				new_variable.size = cbuffer_var_desc.Size;

				// Deducing type
				switch (cbuffer_var_type_desc.Type)
				{
				case D3D_SVT_BOOL:
					new_variable.type = ShaderVariableType::Bool;
					break;
				case D3D_SVT_INT:
					new_variable.type = ShaderVariableType::Int;
					break;
				case D3D_SVT_UINT:
					new_variable.type = ShaderVariableType::UnsignedInt;
					break;
				case D3D_SVT_FLOAT:
					new_variable.type = ShaderVariableType::Float;
					break;
				}

				// Deducing dimensions ( vector / matrix )
				if (cbuffer_var_type_desc.Class == D3D_SVC_VECTOR)
					new_variable.dim1 = cbuffer_var_type_desc.Columns;
				else if (cbuffer_var_type_desc.Class == D3D_SVC_MATRIX_COLUMNS ||
					cbuffer_var_type_desc.Class == D3D_SVC_MATRIX_ROWS)
				{
					new_variable.dim1 = cbuffer_var_type_desc.Rows;
					new_variable.dim2 = cbuffer_var_type_desc.Columns;
				}
				else
				{
					new_variable.dim1 = new_variable.dim2 = 0;
				}

				new_variable.offset = cbuffer_var_desc.StartOffset;
				new_variable.bind_point = cbuffer_index;

				p_shader.variables.insert(std::make_pair(cbuffer_var_desc.Name, new_variable));
			}
		}


		// Textures, samplers & more
		std::size_t textures{ 0 };
		std::size_t samplers{ 0 };

		for (unsigned int var_index{ 0 }; var_index < shader_desc.BoundResources; ++var_index)
		{
			D3D11_SHADER_INPUT_BIND_DESC resource_bind_desc;
			if (FAILED(reflection->GetResourceBindingDesc(var_index, &resource_bind_desc)))
			{
				reflection->Release();
				return false;
			}
			
			ShaderInternal::Variable new_var;
			new_var.name = resource_bind_desc.Name;

			if (resource_bind_desc.Type == D3D_SIT_TEXTURE)
			{
				new_var.type = ShaderVariableType::Texture;

				++textures;
			}
			else if (resource_bind_desc.Type == D3D_SIT_SAMPLER)
			{
				new_var.type = ShaderVariableType::Sampler;

				++samplers;
			}
			else
				continue;

			new_var.size = 0;
			new_var.offset = new_var.dim1 = new_var.dim2 = 0;
			new_var.bind_point = resource_bind_desc.BindPoint;

			p_shader.variables.insert(std::make_pair(new_var.name, new_var));
		}

		p_shader.textures.resize(textures);
		p_shader.samplers.resize(samplers);
		
		if (!p_shader.textures.empty())
			std::memset(&p_shader.textures[0], 0, sizeof(TextureInternal*)* textures);

		if (!p_shader.samplers.empty())
			std::memset(&p_shader.samplers[0], 0, sizeof(SamplerInternal*)* samplers);

		//Free allocation shader reflection memory
		reflection->Release();

		return true;
	}

	void Commander::destroy_shader(ShaderInternal& p_shader)
	{
		p_shader.variables.clear();
		p_shader.constant_buffers.clear();
		p_shader.textures.clear();
		p_shader.samplers.clear();

		if (p_shader.shader_handle != nullptr)
			p_shader.shader_handle->Release();

		if (p_shader.input_layout != nullptr)
			p_shader.input_layout->Release();
	}

	bool Commander::create_blender(BlendType p_type, BlenderInternal& p_blender)
	{
		assert(m_device != nullptr);

		destroy_blender(p_blender);
		p_blender = BlenderInternal();

		D3D11_BLEND_DESC blender_desc;

		// @TODO Implement different blending per RT
		blender_desc.RenderTarget[0].BlendEnable = true;
		if (p_type == BlendType::None)
		{
			blender_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blender_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		}
		else if (p_type == BlendType::Alpha)
		{
			blender_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blender_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		}
		else
		{
			blender_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blender_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		}

		blender_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blender_desc.RenderTarget[0].SrcBlendAlpha = 
			blender_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blender_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blender_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blender_desc.AlphaToCoverageEnable = false;
		blender_desc.IndependentBlendEnable = false;
		
		HRESULT hr = m_device->CreateBlendState(&blender_desc, &p_blender.blender_handle);
		if (FAILED(hr))
			return false;

		return true;
	}

	void Commander::destroy_blender(BlenderInternal& p_blender)
	{
		if (p_blender.blender_handle != nullptr)
			p_blender.blender_handle->Release();
	}

	bool Commander::update_buffer(BufferInternal& p_buffer, UpdateType p_type, const void* p_data, std::size_t p_size)
	{
		assert(m_device_context != nullptr);
		
		if (p_data == nullptr)
			return false;

		D3D11_MAPPED_SUBRESOURCE buffer_map{ 0 };

		// Discard should be fine i guess, but i have to check it out and will @TODO it later
		
		D3D11_MAP buffer_map_type = map_to_raw(p_type);

		buffer_map_type = D3D11_MAP_WRITE_DISCARD;

		HRESULT hr = m_device_context->Map(p_buffer.buffer_handle, 0, buffer_map_type, 0 /* We want to wait, this is strictly blocking if resource is being used */, &buffer_map);
		if (FAILED(hr))
			return false;

		std::memcpy(buffer_map.pData, p_data, p_size);

		m_device_context->Unmap(p_buffer.buffer_handle, 0);

		return true;
	}

	///////////////////////////////////////////////////////////////////////
	/// QUERYING
	///////////////////////////////////////////////////////////////////////
	bool Commander::query_texture_info(const TextureInternal& p_texture, TextureData& p_data)
	{
		assert(p_texture.texture_handle != nullptr);
		
		D3D11_TEXTURE2D_DESC texture_desc;
		p_texture.texture_handle->GetDesc(&texture_desc);

		p_data.width = texture_desc.Width;
		p_data.height = texture_desc.Height;

		return true;
	}

	///////////////////////////////////////////////////////////////////////
	/// BINDING
	///////////////////////////////////////////////////////////////////////
	bool Commander::bind_buffer(const BufferInternal& p_buffer)
	{
		assert(m_device_context != nullptr);
		assert(p_buffer.buffer_handle != nullptr);

		if (p_buffer.type == BufferType::Vertex)
		{
			if (p_buffer.id == m_vertex_buffer)
				return true;

			// @TODO : make them static ??
			UINT stride = static_cast<UINT>(p_buffer.stride);
			UINT offsets = 0;
			m_device_context->IASetVertexBuffers(0, 1, &p_buffer.buffer_handle, &stride, &offsets);
			m_vertex_buffer = p_buffer.id;
		}
		else
		{
			// @TODO : define common format for indices ( 16 bit unsigned ? )
			//m_device_context->IASetIndexBuffer(p_buffer.buffer_handle, DXGI_FORMAT_)
		}

		return true;
	}

	void Commander::unbind_buffer(BufferType p_type)
	{

	}

	bool Commander::bind_shader(ShaderInternal& p_shader)
	{
		assert(m_device_context != nullptr);
		assert(p_shader.shader_handle != nullptr);

		if (p_shader.type == ShaderType::Vertex)
		{
			//if (m_vertex_shader == p_shader.id)
			//	return true;

			// Binding input layout
			m_device_context->IASetInputLayout(p_shader.input_layout);

			// Binding buffers
			for (unsigned int cbuffer_index{ 0 }; cbuffer_index < p_shader.constant_buffers.size(); ++cbuffer_index)
			{
				// Updating if dirty
				if (p_shader.constant_buffers[cbuffer_index].dirty)
				{
					update_buffer(p_shader.constant_buffers[cbuffer_index], UpdateType::Discard, &p_shader.constant_buffers[cbuffer_index].raw_buffer[0], p_shader.constant_buffers[cbuffer_index].stride);
					p_shader.constant_buffers[cbuffer_index].dirty = false;
				}

				m_device_context->VSSetConstantBuffers(cbuffer_index, 1, &p_shader.constant_buffers[cbuffer_index].buffer_handle);
			}

			for (const auto& texture : p_shader.textures)
			{
				m_device_context->VSSetShaderResources(0, 1, &texture->shader_view);
			}

			for (const auto& sampler : p_shader.samplers)
			{
				m_device_context->VSSetSamplers(0, 1, &sampler->sampler_handle);
			}

			// @TODO : Add class instances
			m_device_context->VSSetShader(static_cast<ID3D11VertexShader*>(p_shader.shader_handle), nullptr, 0);
			m_vertex_shader = p_shader.id;
		}
		else if (p_shader.type == ShaderType::Geometry)
		{
			//if (m_geometry_shader == p_shader.id)
			//	return true;

			m_geometry_shader = p_shader.id;

			// Binding buffers
			for (unsigned int cbuffer_index{ 0 }; cbuffer_index < p_shader.constant_buffers.size(); ++cbuffer_index)
			{
				// Updating if dirty
				if (p_shader.constant_buffers[cbuffer_index].dirty)
				{
					update_buffer(p_shader.constant_buffers[cbuffer_index], UpdateType::Discard, &p_shader.constant_buffers[cbuffer_index].raw_buffer[0], p_shader.constant_buffers[cbuffer_index].stride);
					p_shader.constant_buffers[cbuffer_index].dirty = false;
				}
				
				m_device_context->GSSetConstantBuffers(cbuffer_index, 1, &p_shader.constant_buffers[cbuffer_index].buffer_handle);
			}

			for (const auto& texture : p_shader.textures)
			{
				m_device_context->GSSetShaderResources(0, 1, &texture->shader_view);
			}

			for (const auto& sampler : p_shader.samplers)
			{
				m_device_context->GSSetSamplers(0, 1, &sampler->sampler_handle);
			}

			m_device_context->GSSetShader(static_cast<ID3D11GeometryShader*>(p_shader.shader_handle), nullptr, 0);
			m_geometry_shader = p_shader.id;
		}
		else if (p_shader.type == ShaderType::Pixel)
		{
			if (m_pixel_shader == p_shader.id)
				return true;

			m_pixel_shader = p_shader.id;

			// Binding buffers
			for (unsigned int cbuffer_index{ 0 }; cbuffer_index < p_shader.constant_buffers.size(); ++cbuffer_index)
			{
				// Updating if dirty
				if (p_shader.constant_buffers[cbuffer_index].dirty)
				{
					update_buffer(p_shader.constant_buffers[cbuffer_index], UpdateType::Discard, &p_shader.constant_buffers[cbuffer_index].raw_buffer[0], p_shader.constant_buffers[cbuffer_index].stride);
					p_shader.constant_buffers[cbuffer_index].dirty = false;
				}
				
				m_device_context->PSSetConstantBuffers(cbuffer_index, 1, &p_shader.constant_buffers[cbuffer_index].buffer_handle);
			}

			for (const auto& texture : p_shader.textures)
			{
				if (texture != nullptr)
					m_device_context->PSSetShaderResources(0, 1, &texture->shader_view);
			}

			for (const auto& sampler : p_shader.samplers)
			{
				if (sampler != nullptr)
					m_device_context->PSSetSamplers(0, 1, &sampler->sampler_handle);
			}

			m_device_context->PSSetShader(static_cast<ID3D11PixelShader*>(p_shader.shader_handle), nullptr, 0);
			m_pixel_shader = p_shader.id;
		}

		return true;
	}

	void Commander::unbind_shader(ShaderType p_type)
	{

	}

	bool Commander::bind_target(const TextureInternal& p_render_target, bool p_bind_depth)
	{
		assert(m_device_context != nullptr);

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = viewport.TopLeftY = 0;
		ssa::TextureData data;
		query_texture_info(*reinterpret_cast<const ssa::TextureInternal*>(&p_render_target), data);
		viewport.Width = static_cast<float>(data.width);
		viewport.Height = static_cast<float>(data.height);
		viewport.MinDepth = 0.1f;
		viewport.MaxDepth = 1.f;

		m_device_context->RSSetViewports(1, &viewport);

		ID3D11DepthStencilView* depth_view{ nullptr };
		if (p_bind_depth)
			depth_view = p_render_target.depth_stencil_view;

		m_device_context->OMSetRenderTargets(1, &p_render_target.render_target_view, depth_view);

		return true;
	}

	bool Commander::bind_targets(TextureInternal * const* p_render_targets, std::size_t p_count, bool p_bind_depth)
	{
		assert(m_device_context != nullptr);

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = viewport.TopLeftY = 0;
		ssa::TextureData data;
		query_texture_info(*p_render_targets[0], data);
		viewport.Width = static_cast<float>(data.width);
		viewport.Height = static_cast<float>(data.height);
		viewport.MinDepth = 0.1f;
		viewport.MaxDepth = 1.f;

		m_device_context->RSSetViewports(1, &viewport);

		std::vector<ID3D11RenderTargetView*> views;
		for (unsigned int i{ 0 }; i < p_count; ++i)
			views.push_back(p_render_targets[0]->render_target_view);

		std::vector<ID3D11DepthStencilView*> depth_views;
		if (p_bind_depth)
		{
			for (unsigned int i{ 0 }; i < p_count; ++i)
				depth_views.push_back(p_render_targets[0]->depth_stencil_view);
		}

		m_device_context->OMSetRenderTargets(p_count, &views[0],
			p_bind_depth ? depth_views[0] : nullptr);

		return true;
	}

	void Commander::unbind_targets()
	{
		assert(m_device_context != nullptr);

		ID3D11RenderTargetView* views[] { nullptr};
		m_device_context->OMSetRenderTargets(1, &views[0], nullptr);
	}

	bool Commander::bind_blender(const BlenderInternal& p_blender)
	{
		assert(m_device_context != nullptr);

		m_device_context->OMSetBlendState(p_blender.blender_handle, nullptr, 0xffffffff);

		return true;
	}

	void Commander::unbind_blender()
	{
		assert(m_device_context != nullptr);

		m_device_context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	bool Commander::set_value(ShaderInternal& p_shader, const std::string& p_name, const void* p_data, std::size_t p_size)
	{
		assert(m_device_context != nullptr);
		
		auto var = p_shader.variables.at(p_name);

		// Not implemented yet
		if (var.type == ShaderVariableType::Sampler)
		{
			p_shader.samplers[var.bind_point] = static_cast<const SamplerInternal*>(p_data);

			if (p_shader.type == ShaderType::Vertex && p_shader.id == m_vertex_shader)
			{
				m_device_context->VSSetSamplers(var.bind_point, 1, &p_shader.samplers[var.bind_point]->sampler_handle);
			}
			else if (p_shader.type == ShaderType::Geometry && p_shader.id == m_geometry_shader)
			{
				m_device_context->GSSetSamplers(var.bind_point, 1, &p_shader.samplers[var.bind_point]->sampler_handle);
			}
			else if (p_shader.type == ShaderType::Pixel && p_shader.id == m_pixel_shader)
			{
				m_device_context->PSSetSamplers(var.bind_point, 1, &p_shader.samplers[var.bind_point]->sampler_handle);
			}
		}
		else if (var.type == ShaderVariableType::Texture)
		{
			p_shader.textures[var.bind_point] = static_cast<const TextureInternal*>(p_data);

			if (p_shader.type == ShaderType::Vertex && p_shader.id == m_vertex_shader)
			{
				m_device_context->VSSetShaderResources(var.bind_point, 1, &p_shader.textures[var.bind_point]->shader_view);
			}
			else if (p_shader.type == ShaderType::Geometry && p_shader.id == m_geometry_shader)
			{
				m_device_context->GSSetShaderResources(var.bind_point, 1, &p_shader.textures[var.bind_point]->shader_view);
			}
			else if (p_shader.type == ShaderType::Pixel && p_shader.id == m_pixel_shader)
			{
				m_device_context->PSSetShaderResources(var.bind_point, 1, &p_shader.textures[var.bind_point]->shader_view);
			}
		}
		else
		{
			// See if sizes match
			if (var.size != p_size)
				return false;

			std::vector<unsigned char>& buffer = p_shader.constant_buffers[var.bind_point].raw_buffer;
			std::memcpy((&buffer[0]), p_data, var.size);
			p_shader.constant_buffers[var.bind_point].dirty = true;

			if (p_shader.id == m_vertex_shader ||
				p_shader.id == m_pixel_shader ||
				p_shader.id == m_geometry_shader)
			{
				return update_buffer(p_shader.constant_buffers[var.bind_point],UpdateType::NoOverwrite, p_data, var.size);
			}
		}

		return true;
	}

	void Commander::unset_value(ShaderInternal& p_shader, const std::string& p_name)
	{
		assert(m_device_context != nullptr);

		auto var = p_shader.variables.at(p_name);

		// Not implemented yet
		if (var.type == ShaderVariableType::Sampler)
		{
			if (p_shader.type == ShaderType::Vertex && p_shader.id == m_vertex_shader)
			{
				m_device_context->VSSetSamplers(var.bind_point, 1, nullptr);
			}
			else if (p_shader.type == ShaderType::Geometry && p_shader.id == m_geometry_shader)
			{
				m_device_context->GSSetSamplers(var.bind_point, 1, nullptr);
			}
			else if (p_shader.type == ShaderType::Pixel && p_shader.id == m_pixel_shader)
			{
				m_device_context->PSSetSamplers(var.bind_point, 1, nullptr);
			}
		}
		else if (var.type == ShaderVariableType::Texture)
		{
			if (p_shader.type == ShaderType::Vertex && p_shader.id == m_vertex_shader)
			{
				ID3D11ShaderResourceView* view[] = { nullptr };
				m_device_context->VSSetShaderResources(var.bind_point, 1, &view[0]);
			}
			else if (p_shader.type == ShaderType::Geometry && p_shader.id == m_geometry_shader)
			{
				ID3D11ShaderResourceView* view[] = { nullptr };
				m_device_context->GSSetShaderResources(var.bind_point, 1, &view[0]);
			}
			else if (p_shader.type == ShaderType::Pixel && p_shader.id == m_pixel_shader)
			{
				ID3D11ShaderResourceView* view[] = { nullptr };
				m_device_context->PSSetShaderResources(var.bind_point, 1, &view[0]);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	/// DRAWING
	///////////////////////////////////////////////////////////////////////
	bool Commander::clear(const TextureInternal& p_render_target, float4 p_color)
	{
		assert(m_device_context != nullptr);
		assert(p_render_target.render_target_view != nullptr);

		float clear_color[] = { 0.f, 0.f, 0.f, 1.f };

		m_device_context->ClearRenderTargetView(p_render_target.render_target_view, &clear_color[0]);

		return true;
	}

	bool Commander::clear_depth(const TextureInternal& p_render_target, float p_depth)
	{
		assert(m_device_context != nullptr);

		m_device_context->ClearDepthStencilView(p_render_target.depth_stencil_view, D3D11_CLEAR_DEPTH, p_depth, 0);

		return true;
	}

	bool Commander::draw(PrimitiveTopologyType p_primitive_topology, unsigned int p_vertices_count, unsigned int p_vertices_offset)
	{
		assert(m_device_context != nullptr);

		// @TODO : call method
		m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_device_context->Draw(p_vertices_count, p_vertices_offset);

		return true;
	}

	bool Commander::finalize(const TextureInternal& p_render_window, bool p_vsync)
	{
		assert(p_render_window.swap_chain != nullptr);

		// @TODO : Present DXGI_PRESENT_RESTART AFTER RESIZING ( use flag ) 
		unsigned int vsync = 0;
		if (p_vsync)
			vsync = 1;

		p_render_window.swap_chain->Present(vsync, 0);

		return true;
	}
}