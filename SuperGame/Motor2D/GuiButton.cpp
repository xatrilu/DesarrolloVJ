#include "GuiButton.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Render.h"
//#include "GuiText.h"
#include "p2SString.h"

GuiButton::GuiButton(j1Module* g_callback) {
	callback = g_callback;
}

void GuiButton::InitializeButton(iPoint g_position, SDL_Rect g_normal_rect, SDL_Rect g_hover_rect, SDL_Rect g_click_rect, p2SString text) {
	screen_position = g_position;
	tex = (SDL_Texture*)App->gui->GetAtlas();
	normal_rect = g_normal_rect;
	hover_rect = g_normal_rect;
	click_rect = g_click_rect;

	if (parent != nullptr)
	{
		local_position.x = screen_position.x - parent->screen_position.x;
		local_position.y = screen_position.y - parent->screen_position.y;
	}


}

bool GuiButton::Input() {
	if (current_rect != nullptr)
	{
		rect.w = current_rect->w;
		rect.h = current_rect->h;
	}
	iPoint mouse_motion;
	if (!MouseHovering())
	{
		current_rect = &normal_rect;
	}
	else {
		current_rect = &hover_rect;
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			current_rect = &click_rect;
		}
	}


	return true;
}

bool GuiButton::Draw() {
	App->render->Blit(tex, screen_position.x, screen_position.y, current_rect);
	return true;
}