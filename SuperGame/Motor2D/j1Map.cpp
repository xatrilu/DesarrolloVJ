#include <math.h>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "brofiler/Brofiler/Brofiler.h"
#include "j1EntityManager.h"


j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map() {}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	App->collision->AddCollider({ -5,0,5,1000 }, COLLIDER_WALL);

	folder.create(config.child("folder").child_value());

	//level 1 data
	level_1_music = config.child("level1").child("music").attribute("song").as_string();
	level_1_initial_camera_position.x = config.child("level1").child("initial_camera").attribute("x").as_int();
	level_1_initial_camera_position.y = config.child("level1").child("initial_camera").attribute("y").as_int();

	//level 2 data
	level_2_music = config.child("level2").child("music").attribute("song").as_string();
	level_2_initial_camera_position.x = config.child("level2").child("initial_camera").attribute("x").as_int();
	level_2_initial_camera_position.y = config.child("level2").child("initial_camera").attribute("y").as_int();

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("MapDraw", Profiler::Color::Purple)

		if (map_loaded == false)
			return;

	p2List_item<MapLayer*>* item = data.layers.start;
	for (; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = MapToWorld(x, y);
					//camera cooling
					if ((pos.x < -App->render->camera.x + App->render->camera.w + 32) && (pos.x > -App->render->camera.x - 32)) {

						if (layer->properties.Get("speed", 0) == 1) {
							tileset->parallax = tileset->texture;
							App->render->Blit(tileset->parallax, pos.x, pos.y, &r, SDL_FLIP_NONE, layer->ParallaxSpeed2);
							App->render->Blit(tileset->parallax, pos.x + 6000, pos.y, &r, SDL_FLIP_NONE, layer->ParallaxSpeed2);
							tileset->texture = tileset->parallax;
						}

						else
						{
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE);
						}
					}
				}
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		App->tex->UnLoad(item->data->texture);
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	//Remove all layers

	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	//Remove all colliders

	p2List_item<ObjectGroup*>* item3;
	item3 = data.objectgroups.start;
	while (item3 != NULL)
	{
		LOG("Objectgroups releasing");

		for (uint i = 0; i < item3->data->size; i++)
		{
			if (item3->data->object[i].type == ObjectType::COLLIDER)
			{
				item3->data->object[i].collider->to_delete = true;
				item3->data->object[i].collider = nullptr;
			}
		}
		App->entities->DestroyAllEntities();
		//delete[] item3->data->collider;
		RELEASE(item3->data);
		item3 = item3->next;
	}
	data.objectgroups.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Level data ---------------------

	if (file_name == "Level1.tmx") {
		App->scene->current_level = LEVEL_1;
		App->audio->PlayMusic(level_1_music.GetString());
		App->scene->initial_camera_position = level_1_initial_camera_position;
	}
	if (file_name == "Level2.tmx") {
		App->scene->current_level = LEVEL_2;
		App->audio->PlayMusic(level_2_music.GetString());
		App->scene->initial_camera_position = level_2_initial_camera_position;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.add(set);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while (item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while (item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			/*
			if (l->name == "Collisions") {
				LoadCollisions(l);
			}
			*/
			item_layer = item_layer->next;
		}

		p2List_item<ObjectGroup*>* item_object = data.objectgroups.start;
		while (item_object != NULL)
		{
			ObjectGroup* o = item_object->data;
			LOG("ObjectGroup ----");
			LOG("name: %s", o->name.GetString());
			item_object = item_object->next;
		}

	}

	//Load objectgroup info -------------------------------------

	pugi::xml_node objectgroup;
	for (objectgroup = map_file.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		ObjectGroup* set = new ObjectGroup();

		if (ret == true)
		{
			ret = LoadObjectGroup(objectgroup, set);
		}
		data.objectgroups.add(set);
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;
	pugi::xml_node tile = node.child("data").child("tile");


	if (tile == NULL)
	{
		LOG("Error loading layer");
		ret = false;
	}

	else
	{
		layer->name = node.attribute("name").as_string();
		layer->width = node.attribute("width").as_uint();
		layer->height = node.attribute("height").as_uint();

		LoadProperties(node, layer->properties);
		pugi::xml_node layer_data = node.child("data");
		//layer->ParallaxSpeed = node.child("properties").child("property").attribute("value").as_float();
		layer->ParallaxSpeed2 = node.child("properties").child("property").attribute("value").as_float();

		if (layer_data == NULL)
		{
			LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
			ret = false;
			RELEASE(layer);
		}
		else
		{
			layer->data = new uint[layer->width * layer->height];
			memset(layer->data, 0, layer->width * layer->height);

			int i = 0;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				layer->data[i++] = tile.attribute("gid").as_int(0);
			}
		}
	}
	return ret;
}

bool j1Map::LoadObjectGroup(pugi::xml_node& node, ObjectGroup* objectgroup) {
	bool ret = true;
	pugi::xml_node object_node = node.child("object");
	SDL_Rect rect = { 0,0,0,0 };
	objectgroup->name = node.attribute("name").as_string();
	uint i = 0u;
	p2SString type;

	if (object_node == NULL)
	{
		LOG("Error loading object group");
		ret = false;
	}

	else
	{
		//objectgroup->object = new SDL_Rect[MAX_COLLIDERS];
		//objectgroup->collider = new Collider*[MAX_COLLIDERS];
		objectgroup->object = new Object[MAX_OBJECTS];
		while (object_node != NULL)
		{
			objectgroup->object[i].rect.x = object_node.attribute("x").as_int();
			objectgroup->object[i].rect.y = object_node.attribute("y").as_int();
			objectgroup->object[i].rect.w = object_node.attribute("width").as_int();
			objectgroup->object[i].rect.h = object_node.attribute("height").as_int();
			//objectgroup->object[i].y -= COLLIDER_OFFSET;

			p2SString type(object_node.attribute("type").as_string());

			if (type == "Collider") {
				objectgroup->object[i].collider = App->collision->AddCollider(objectgroup->object[i].rect, COLLIDER_WALL);
				objectgroup->object[i].type = ObjectType::COLLIDER;
			}

			if (type == "Death") {
				objectgroup->object[i].collider = App->collision->AddCollider(objectgroup->object[i].rect, COLLIDER_DEATH);
				objectgroup->object[i].type = ObjectType::COLLIDER;
			}

			if (type == "Platform") {
				objectgroup->object[i].collider = App->collision->AddCollider(objectgroup->object[i].rect, COLLIDER_PLATFORM);
				objectgroup->object[i].type = ObjectType::COLLIDER;
			}

			if (type == "Trigger") {
				objectgroup->object[i].collider = App->collision->AddCollider(objectgroup->object[i].rect, TRIGGER);
				objectgroup->object[i].type = ObjectType::COLLIDER;
				pugi::xml_node property_node = object_node.child("properties").child("property");
				while (property_node != nullptr) {
					p2SString property(property_node.attribute("name").as_string());
					if (property == "level_change")
					{
						if (property_node.attribute("value").as_int() == 1) {
							objectgroup->object[i].collider->level_change = true;
						}
					}
					if (property == "Checkpoint") {
						if (property_node.attribute("value").as_int() == 1) {
							objectgroup->object[i].collider->isCheckpoint = true;
						}
					}
					property_node = property_node.next_sibling("property");
				}
			}

			if (type == "Player") {
				objectgroup->object->entity = App->entities->CreateEntity(EntityType::PLAYER, objectgroup->object[i].rect.x, objectgroup->object[i].rect.y);
				objectgroup->object[i].type = ObjectType::ENTITY;
			}

			if (type == "Knight") {
				objectgroup->object->entity = App->entities->CreateEntity(EntityType::WALKING_ENEMY, objectgroup->object[i].rect.x, objectgroup->object[i].rect.y);
				objectgroup->object[i].type = ObjectType::ENTITY;
			}

			if (type == "Bat") {
				objectgroup->object->entity = App->entities->CreateEntity(EntityType::FLYING_ENEMY, objectgroup->object[i].rect.x, objectgroup->object[i].rect.y);
				objectgroup->object[i].type = ObjectType::ENTITY;
			}

			if (type == "Collectible")
			{
				objectgroup->object->entity = App->entities->CreateEntity(EntityType::COLLECTIBLE, objectgroup->object[i].rect.x, objectgroup->object[i].rect.y);
				objectgroup->object[i].type = ObjectType::ENTITY;
			}

			object_node = object_node.next_sibling("object");
			i++;
		}
		objectgroup->size = i;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0, 0);
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);
	if (data.type == MAPTYPE_ORTHOGONAL) {
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.add(p);
		}
	}

	return ret;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

bool j1Map::LoadCollisions(MapLayer* layer)
{
	bool ret = true;
	SDL_Rect collider_rect = { 0,0,32,32 };
	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int tile_id = layer->Get(x, y);
			if (tile_id > 0)
			{
				TileSet* tileset = GetTilesetFromTileId(tile_id);
				SDL_Rect r = tileset->GetTileRect(tile_id);
				iPoint pos = MapToWorld(x, y);
				collider_rect.x = pos.x;
				collider_rect.y = pos.y;
				App->collision->AddCollider(collider_rect, COLLIDER_WALL);
			}
		}
	}
	return ret;
}