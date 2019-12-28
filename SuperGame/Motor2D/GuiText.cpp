#include "GuiText.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"

GuiText::GuiText() {
	callback = nullptr;
	font = DEFAULT_FONT;
	parent = nullptr;
	texture = nullptr;
	draggable = false;
	interactable = false;
}

GuiText::GuiText(j1Module* g_callback) {
	callback = g_callback;
	font = DEFAULT_FONT;
}

GuiText::~GuiText() {}

void GuiText::Init(iPoint g_position, p2SString g_text, char* g_font) {
	screen_pos = g_position;
	text = g_text;
	font = g_font;

	if (text.Length() > 0)
	{
		if (font == DEFAULT_FONT)
		{
			texture = App->font->Print(text.GetString());
			App->font->CalcSize(text.GetString(), rect.w, rect.h);
		}
		else
		{
			texture = App->font->Print(text.GetString(), { (255),(255),(255),(255) }, App->font->console_font);
			App->font->CalcSize(text.GetString(), rect.w, rect.h, App->font->console_font);
		}
	}

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}

	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
}

bool GuiText::CleanUp() {
	bool ret = true;
	App->tex->UnLoad(texture);
	texture = nullptr;
	return ret;
}

bool GuiText::Update(float dt) {

	bool ret = true;

	if (parent != nullptr)
	{
		screen_pos.x = parent->screen_pos.x + local_pos.x;
		screen_pos.y = parent->screen_pos.y + local_pos.y;
	}

	rect.x = screen_pos.x - App->render->camera.x;
	rect.y = screen_pos.y - App->render->camera.y;

	return ret;
}

bool GuiText::Draw() {

	if (text.Length() > 0)
	{
		if (texture != nullptr)
		{
			App->render->Blit(texture, rect.x, rect.y);
		}
	}

	return true;
}

void GuiText::UpdateText() {

	if (font == DEFAULT_FONT)
	{
		App->tex->UnLoad(texture);
		texture = App->font->Print(text.GetString());
	}
	else
	{
		App->tex->UnLoad(texture);
		texture = App->font->Print(text.GetString(), { (255),(255),(255),(255) }, App->font->console_font);
	}
}