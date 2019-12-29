#include "GuiText.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"

GuiText::GuiText()
{
	callback = nullptr;
	font = DEFAULT_FONT;
	parent = nullptr;
	texture = nullptr;
	draggable = false;
	interactable = false;
}

GuiText::GuiText(j1Module* callback) 
{
	callback = callback;
	font = DEFAULT_FONT;
}

GuiText::~GuiText() {}

void GuiText::Init(iPoint pos, p2SString _text, char* _font) 
{
	screenPos = pos;
	text = _text;
	font = _font;

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
		localPos.x = screenPos.x - parent->screenPos.x;
		localPos.y = screenPos.y - parent->screenPos.y;
	}

	rect.x = screenPos.x;
	rect.y = screenPos.y;
}

bool GuiText::CleanUp() 
{
	bool ret = true;
	App->tex->UnLoad(texture);
	texture = nullptr;
	return ret;
}

bool GuiText::Update(float dt) 
{
	bool ret = true;

	if (parent != nullptr)
	{
		screenPos.x = parent->screenPos.x + localPos.x;
		screenPos.y = parent->screenPos.y + localPos.y;
	}
	rect.x = screenPos.x - App->render->camera.x;
	rect.y = screenPos.y - App->render->camera.y;
	return ret;
}

bool GuiText::Draw() 
{
	if (text.Length() > 0) if (texture != nullptr) App->render->Blit(texture, rect.x, rect.y);	
	return true;
}

void GuiText::UpdateText() 
{
	if (font == DEFAULT_FONT)
	{
		App->tex->UnLoad(texture);
		texture = nullptr;
		texture = App->font->Print(text.GetString());
	}
	else
	{
		App->tex->UnLoad(texture);
		texture = nullptr;
		texture = App->font->Print(text.GetString(), { (255),(255),(255),(255) }, App->font->console_font);
	}
}