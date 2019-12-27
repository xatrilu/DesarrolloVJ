#include "GuiButton.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Render.h"
#include "GuiText.h"
#include "p2SString.h"
#include "j1Fonts.h"

GuiButton::GuiButton(j1Module* g_callback) {
	callback = g_callback;
	text = new GuiText();
	click_rect = { 0,0,0,0 };
	texture = nullptr;
	current_rect = &normal_rect;
	stay_clicked = false;
}

GuiButton::~GuiButton() {
	delete text;
}

void GuiButton::Init(iPoint g_position, SDL_Rect g_normal_rect, SDL_Rect g_hover_rect, SDL_Rect g_click_rect, p2SString g_text, ButtonAction g_action, bool g_stay_clicked) {
	screen_pos = g_position;
	texture = (SDL_Texture*)App->gui->GetAtlas();
	normal_rect = g_normal_rect;
	hover_rect = g_normal_rect;
	click_rect = g_click_rect;
	action = g_action;
	stay_clicked = g_stay_clicked;

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}

	rect = normal_rect;

	SDL_Rect text_rect;

	App->font->CalcSize(g_text.GetString(), text_rect.w, text_rect.h);
	text_rect.x = screen_pos.x + rect.w * 0.5f - text_rect.w * 0.5f;
	text_rect.y = screen_pos.y + rect.h * 0.5f - text_rect.h * 0.5f;

	text->parent = this;
	text->Init({ text_rect.x, text_rect.y }, g_text);
}

bool GuiButton::CleanUp() {
	bool ret = true;
	text->CleanUp();
	texture = nullptr;
	return ret;
}

bool GuiButton::Input() {

	if (stay_clicked) {
		if (current_rect == &click_rect)
		{
			current_rect = &normal_rect;
		}
		else
		{
			current_rect = &click_rect;
		}
	}
	else
	{
		current_rect = &click_rect;
	}

	callback->OnEvent(this, FocusEvent::CLICKED);
	return true;
}

bool GuiButton::Update(float dt) {
	bool ret = true;
	if (!stay_clicked) {
		if (OnHover())
		{
			current_rect = &hover_rect;
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				current_rect = &click_rect;
			}
		}
		else
		{
			current_rect = &normal_rect;
		}
	}

	if (parent != nullptr)
	{
		screen_pos.x = parent->screen_pos.x + local_pos.x;
		screen_pos.y = parent->screen_pos.y + local_pos.y;
	}

	rect.x = screen_pos.x - App->render->camera.x;
	rect.y = screen_pos.y - App->render->camera.y;

	if (text->text.Length() > 0)
	{
		text->Update(dt);
	}

	return true;
}

bool GuiButton::Draw() {

	App->render->Blit(texture, rect.x, rect.y, current_rect);
	if (text->text.Length() > 0) { text->Draw(); }

	return true;
}

