#include "GuiInputText.h"
#include "j1Render.h"
#include "j1App.h"


GuiInputText::GuiInputText(j1Module* g_callback) {
	background = new GuiImage(g_callback);
	text = new GuiText(g_callback);
	callback = g_callback;
}

GuiInputText::~GuiInputText() {
	delete background;
	delete text;
}

void GuiInputText::InitializeInputText(iPoint position, p2SString g_text, SDL_Rect image_section) {

	screen_position = position;
	rect = { position.x, position.y, image_section.w, image_section.h };

	background->parent = text->parent = (j1UI_Element*)this;
	background->draggable = text->draggable = false;

	rect.x = screen_position.x;
	rect.y = screen_position.y;

	if (parent != nullptr)
	{
		local_position.x = screen_position.x - parent->screen_position.x;
		local_position.y = screen_position.y - parent->screen_position.y;
	}

	background->InitializeImage(position, image_section);
	text->InitializeText(position, g_text);
}

bool GuiInputText::Input() {
	return true;
}


bool GuiInputText::Draw() {

	background->screen_position.x = screen_position.x + background->local_position.x;
	background->screen_position.y = screen_position.y + background->local_position.y;

	text->rect.x = text->screen_position.x = background->rect.x + 10;
	text->rect.y = text->screen_position.y = background->rect.y + background->rect.h * 0.5 - text->rect.h * 0.5;

	background->Draw();
	text->Draw();

	return true;
}