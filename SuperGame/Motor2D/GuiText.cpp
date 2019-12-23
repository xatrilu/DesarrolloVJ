#include "GuiText.h"
#include "j1App.h"
#include "j1Render.h"

GuiText::GuiText(j1Module* g_callback) {
	callback = g_callback;
}

GuiText::~GuiText() {}

void GuiText::InitializeText(iPoint g_position, p2SString g_text) {
	screen_position = g_position;
	text = g_text;
	texture = App->font->Print(text.GetString());
	rect.x = g_position.x;
	rect.y = g_position.y;
	App->font->CalcSize(text.GetString(), rect.w, rect.h);

	if (parent != nullptr)
	{
		local_position.x = screen_position.x - parent->screen_position.x;
		local_position.y = screen_position.y - parent->screen_position.y;
	}
}

bool GuiText::Draw() {
	App->render->Blit(texture, rect.x, rect.y);
	return true;
}