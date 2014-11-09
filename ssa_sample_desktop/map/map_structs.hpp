#pragma once

#include <stegosaurus.hpp>
#include <vector>
#include <utility>

struct Object
{
	ssa::float2 position;
	ssa::float2 size;
	std::vector<std::pair<std::string, std::string>> properties;
};

struct InteractiveObject : Object
{
	unsigned int fragment;
};

struct TileSet
{
	std::string     name;
	std::string		source_image;
	unsigned int	width;
	unsigned int	height;
	unsigned int	first_id;
};

// Each layer should have the same width / height
struct TileLayer
{
	unsigned int width;
	unsigned int height;

	std::vector<unsigned int> ids;
};

struct Map
{
	ssa::float2 spawn;

	// Objects are global, not linked to single layer,
	// They are only shapes used for collision detection
	std::vector<Object> objects;
	std::vector<InteractiveObject> fragments;

	std::vector<TileLayer> tile_layers;
	std::vector<TileSet> tile_sets;

	unsigned int width;
	unsigned int height;
};