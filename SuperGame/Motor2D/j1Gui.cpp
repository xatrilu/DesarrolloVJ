#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
/*
#include "j1Fonts.h"
*/
#include "j1Input.h"
#include "j1Gui.h"


j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	SDL_Event event;

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		item->data->Input();
	}

	for (p2List_item<j1UI_Element*>* item = ui_elements.end; item != nullptr; item = item->prev)
	{
		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) && (item->data->draggable))
		{
			iPoint mouse_motion;
			App->input->GetMouseMotion(mouse_motion.x, mouse_motion.y);

			if (item->data->MouseHovering())
			{
				item->data->screen_position += mouse_motion;
				item->data->rect.x = item->data->screen_position.x;
				item->data->rect.y = item->data->screen_position.y;
				focusing_element = item->data;
				item->data->Input();
				break;
			}
			focusing_element = nullptr;
		}

		if (item->data->parent != nullptr)
		{
			item->data->screen_position.x = item->data->parent->screen_position.x + item->data->local_position.x;
			item->data->screen_position.y = item->data->parent->screen_position.y + item->data->local_position.y;
		}

		item->data->rect.x = item->data->screen_position.x;
		item->data->rect.y = item->data->screen_position.y;
	}

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {
		if (focusing_element == nullptr)
		{
			focusing_element = ui_elements.start->data;
		}
		else
		{
			int item = ui_elements.find(focusing_element);
			if (item == ui_elements.count() - 1)
			{
				item = -1;
			}
			focusing_element = ui_elements[item + 1];
		}
	}

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		if (focusing_element == item->data)
		{
			item->data->callback->OnEvent(item->data);
			break;
		}
	}

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		item->data->Draw();
	}

	if (debug)
	{
		DebugDraw();
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

j1UI_Element* j1Gui::CreateUIElement(UI_Type type, j1Module* callback, j1UI_Element* g_parent, bool draggable) {
	j1UI_Element* ui_element = nullptr;
	switch (type)
	{
	
	case UI_Type::BUTTON:
		ui_element = new GuiButton(callback);
		break;
	case UI_Type::IMAGE:
		ui_element = new GuiImage(callback);
		break;
	case UI_Type::TEXT:
		ui_element = new GuiText(callback);
		break;
	case UI_Type::INPUT_TEXT:
		ui_element = new GuiInputText(callback);
		break;
	case UI_Type::MAX_UI_ELEMENTS:
		break;
	default:
		break;
	}
	ui_element->parent = g_parent;
	ui_element->draggable = draggable;
	ui_elements.add(ui_element);

	return ui_element;
}

void j1Gui::DebugDraw() {

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		if (focusing_element == item->data) {
			App->render->DrawQuad(item->data->rect, 0, 255, 0, 255, false);
		}
		else
		{
			App->render->DrawQuad(item->data->rect, 255, 0, 0, 255, false);
		}
	}
}
