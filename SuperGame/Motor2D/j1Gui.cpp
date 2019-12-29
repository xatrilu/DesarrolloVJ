#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "brofiler/Brofiler/Brofiler.h"


j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
	atlas = nullptr;
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI spritesheet");
	bool ret = true;

	atlas_file_name = conf.child("image").attribute("file").as_string("");

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
	BROFILER_CATEGORY("GuiPreUpdate", Profiler::Color::Lime)

		if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
			debug = !debug;

	for (p2List_item<j1UI_Element*>* item = ui_elements.end; item != nullptr; item = item->prev)
	{
		iPoint mouse_motion;
		App->input->GetMouseMotion(mouse_motion.x, mouse_motion.y);


		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			if ((item->data->draggable) && (focused_element == item->data)) {

				item->data->screen_pos.x += mouse_motion.x;
				focused_element = item->data;
			}
		}

		if (item->data->OnHover())
		{
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				if (item->data->interactable) {
					focused_element = item->data;
					focused_element->HandleFocusEvent(FocusEvent::FOCUS_IN);
					focused_element->Input();
				}
			}
			break;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			if ((focused_element != nullptr) && (!item->data->OnHover())) {
				focused_element->HandleFocusEvent(FocusEvent::FOCUS_OUT);
				focused_element = nullptr;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {
		if (focused_element == nullptr)
		{
			focused_element = ui_elements.start->data;
		}
		else
		{
			int item = ui_elements.find(focused_element);
			if (item == ui_elements.count() - 1)
			{
				item = -1;
			}
			focused_element = ui_elements[item + 1];
		}
	}

	return true;
}

bool j1Gui::Update(float dt) {
	BROFILER_CATEGORY("GuiUpdate", Profiler::Color::BlueViolet)
		bool ret = true;
	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		item->data->Update(dt);
	}
	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	BROFILER_CATEGORY("GuiPostUpdate", Profiler::Color::DarkKhaki)
		for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
		{
			item->data->Draw();
		}

	if (debug) DebugDraw();

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	DestroyAllGui();
	App->tex->UnLoad(atlas);
	atlas = nullptr;
	return true;
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

j1UI_Element* j1Gui::CreateUIElement(UI_Type type, j1Module* callback, j1UI_Element* parent, bool draggable, bool interactable) {

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
	/*case UI_Type::SLIDER:
		ui_element = new GuiSlider(callback);*/
	case UI_Type::MAX_UI_ELEMENTS:
		break;
	default:
		break;
	}

	ui_element->draggable = draggable;
	ui_element->interactable = interactable;
	ui_element->parent = parent;
	ui_element->type = type;

	ui_elements.add(ui_element);

	return ui_element;
}

void j1Gui::DestroyUIElement(j1UI_Element* element) {

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		if (item->data == element) {
			item->data->CleanUp();
			delete item->data;
			item->data = nullptr;
			ui_elements.del(item);
		}
	}
}

void j1Gui::DestroyAllGui() {

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		DestroyUIElement(item->data);
	}

	ui_elements.clear();
}

void j1Gui::DestroyUIlist(p2List<j1UI_Element*> list) {
	for (p2List_item<j1UI_Element*>* item = list.start; item != nullptr; item = item->next)
	{
		item->data->CleanUp();
	}
}

void j1Gui::DebugDraw() {

	for (p2List_item<j1UI_Element*>* item = ui_elements.start; item != nullptr; item = item->next)
	{
		if (focused_element == item->data) {
			App->render->DrawQuad(item->data->rect, 0, 255, 0, 255, false);
		}
		else
		{
			App->render->DrawQuad(item->data->rect, 255, 0, 0, 255, false);
		}
		if (item->data->OnHover()) {
			App->render->DrawQuad(item->data->rect, 0, 0, 255, 255, false);
		}
	}
}

