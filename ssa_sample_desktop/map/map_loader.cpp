#include "map_loader.hpp"
#include "map_structs.hpp"
#include "rapidxml/rapidxml.hpp"

#include <iostream>
#include <fstream>

const char* try_get_attribute(const rapidxml::xml_node<>* p_node, const char* p_name)
{
	rapidxml::xml_attribute<>* attribute = p_node->first_attribute(p_name);
	if (attribute == nullptr)
	{
		std::cout << "Failed to retrieve attribute " << p_name << "from node " << p_node->name() << std::endl;
		return nullptr;
	}
	return attribute->value();
}

#define TRY_ASSIGN_ATTRIBUTE(p_var, p_node, p_name) \
{ \
	const char* tmp = try_get_attribute(p_node, p_name); \
if (tmp != nullptr) \
	p_var = tmp; \
} \

#define TRY_ASSIGN_ATTRIBUTE_INT(p_var, p_node, p_name) \
{ \
	const char* tmp = try_get_attribute(p_node, p_name); \
if (tmp != nullptr) \
	p_var = std::stoi(tmp); \
} \

#define TRY_ASSIGN_ATTRIBUTE_FLOAT(p_var, p_node, p_name) \
{ \
	const char* tmp = try_get_attribute(p_node, p_name); \
if (tmp != nullptr) \
	p_var = std::stof(tmp); \
} \

bool load_map(const std::string& p_filename, Map& p_map)
{
	// Clearing map
	p_map.objects.clear();
	p_map.tile_layers.clear();
	p_map.tile_sets.clear();
	
	using namespace rapidxml;

	std::ifstream map_stream(p_filename);
	if (!map_stream.good())
	{
		std::cout << "Failed to open map: " << p_filename << std::endl;
		return false;
	}

	std::vector<char> buffer((std::istreambuf_iterator<char>(map_stream)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	xml_document<> map_xml;
	map_xml.parse<0>(&buffer[0]);

	xml_node<>* root_node = map_xml.first_node();
	
	if (root_node == nullptr || std::strcmp(root_node->name(), "map"))
		std::cout << "Failed to find map root node" << std::endl;

	TRY_ASSIGN_ATTRIBUTE_INT(p_map.width, root_node, "width");
	TRY_ASSIGN_ATTRIBUTE_INT(p_map.height, root_node, "height");

	for (xml_node<>* next_node = root_node->first_node(); next_node; next_node = next_node->next_sibling())
	{
		// Parsing tileset
		if (std::strcmp(next_node->name(), "tileset") == 0)
		{
			TileSet new_set;
			TRY_ASSIGN_ATTRIBUTE(new_set.name, next_node, "name");
			TRY_ASSIGN_ATTRIBUTE(new_set.source_image, next_node->first_node("image"), "source");
			TRY_ASSIGN_ATTRIBUTE_INT(new_set.first_id, next_node, "firstgid");
			TRY_ASSIGN_ATTRIBUTE_INT(new_set.width, next_node->first_node("image"), "width");
			TRY_ASSIGN_ATTRIBUTE_INT(new_set.height, next_node->first_node("image"), "height");

			p_map.tile_sets.push_back(new_set);
		}
		// Parsing Tile Layer
		else if (std::strcmp(next_node->name(), "layer") == 0)
		{
			TileLayer new_layer;
			TRY_ASSIGN_ATTRIBUTE_INT(new_layer.width, next_node, "width");
			TRY_ASSIGN_ATTRIBUTE_INT(new_layer.height, next_node, "height");

			xml_node<>* data_node = next_node->first_node("data");
			if (data_node == nullptr)
				std::cout << "Failed to retrieve data node for layer " << next_node->first_attribute("name")->value() << std::endl;

			for (xml_node<>* tile_node = data_node->first_node("tile"); tile_node; tile_node = tile_node->next_sibling())
			{
				unsigned int next_id;
				TRY_ASSIGN_ATTRIBUTE_INT(next_id, tile_node, "gid");
				new_layer.ids.push_back(next_id);
			}

			p_map.tile_layers.push_back(new_layer);
		}
		// Parsing Object Layer
		else if (std::strcmp(next_node->name(), "objectgroup") == 0)
		{
			for (xml_node<>* object_node = next_node->first_node("object"); object_node; object_node = object_node->next_sibling())
			{
				Object object;
				TRY_ASSIGN_ATTRIBUTE_FLOAT(object.position.x, object_node, "x");
				TRY_ASSIGN_ATTRIBUTE_FLOAT(object.position.y, object_node, "y");
				TRY_ASSIGN_ATTRIBUTE_FLOAT(object.size.x, object_node, "width");
				TRY_ASSIGN_ATTRIBUTE_FLOAT(object.size.y, object_node, "height");
			
				xml_node<>* properties_root = object_node->first_node("properties");
				if (properties_root != nullptr)
				{
					for (xml_node<>* property = properties_root->first_node("property"); property; property = property->next_sibling())
					{
						std::pair<std::string, std::string> new_property;
						TRY_ASSIGN_ATTRIBUTE(new_property.first, property, "name");
						TRY_ASSIGN_ATTRIBUTE(new_property.second, property, "value");

						if (new_property.first == "Spawn")
							p_map.spawn = object.position;

						object.properties.push_back(new_property);
					}
				}

				p_map.objects.push_back(object);
			}
		}
		// Ignored nodes
		else
		{
			std::cout << "Ignored node " << next_node->name() << std::endl;
		}
	}

	std::cout << "================================" << std::endl;
	std::cout << "Finished parsing " << p_filename << std::endl;
	std::cout << "Report : " << std::endl;
	std::cout << "Total objects : " << p_map.objects.size() << std::endl;
	std::cout << "Total tilesets : " << p_map.tile_sets.size() << std::endl;
	std::cout << "Total tilelayers : " << p_map.tile_layers.size() << std::endl;
	std::cout << "================================" << std::endl;

	return true;
}