#include "j1App.h"
#include "j1Gui.h"
#include "GuiImage.h"
#include "j1Render.h"
#include "j1Textures.h"

GuiImage::GuiImage(j1Module* g_callback) {
	texture = (SDL_Texture*)App->gui->GetAtlas();
	callback = g_callback;
}

void GuiImage::Init(iPoint position, SDL_Rect g_section) {
	screen_pos = position;
	section = g_section;
	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
	rect.w = section.w;
	rect.h = section.h;

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}
}

bool GuiImage::CleanUp() {
	bool ret = true;
	texture = nullptr;
	return ret;
}


bool GuiImage::Update(float dt) {
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

bool GuiImage::Draw() {
	App->render->Blit(texture, rect.x, rect.y, &section);
	return true;
}