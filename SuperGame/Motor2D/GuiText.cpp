#include "GuiText.h"
#include "j1App.h"
#include "j1Render.h"


GuiText::GuiText() 
{
	callback = nullptr;
	font = DEFAULT_FONT;
	parent = nullptr;
	texture = nullptr;
	draggable = false;
	interactable = false;
}

GuiText::GuiText(j1Module* g_callback, bool g_isStatic)
{
	callback = g_callback;
	font = DEFAULT_FONT;
	isStatic = g_isStatic;
}
GuiText::~GuiText() {}

void GuiText::Init(iPoint g_pos, p2SString g_text) 
{
	screen_pos = g_pos;
	text = g_text;
	if (text.Length() > 0)
	{
		texture = App->font->Print(text.GetString());
	}
	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
	App->font->CalcSize(text.GetString(), rect.w, rect.h);

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}
}


bool GuiText::Update(float dt) 
{
	bool ret = true;
	if (parent != nullptr)
	{
		screen_pos.x = parent->screen_pos.x + local_pos.x;
		screen_pos.y = parent->screen_pos.y + local_pos.y;
	}
	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
	return ret;
}

bool GuiText::Draw() 
{
	if (text.Length() > 0)
	{
		texture = App->font->Print(text.GetString());
		App->render->Blit(texture, rect.x, rect.y);
	}
	return true;
}

bool GuiText::CleanUp() 
{
	return true;
}
