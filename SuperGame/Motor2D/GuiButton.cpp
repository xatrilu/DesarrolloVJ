#include "GuiButton.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Render.h"
#include "GuiText.h"
#include "p2SString.h"
#include "j1Fonts.h"
#include "p2Log.h"
#include "j1Audio.h"

GuiButton::GuiButton(j1Module* _callback) 
{
	callback = _callback;
	text = new GuiText();
	clickRect = { 0,0,0,0 };
	texture = nullptr;
	currentRect = &normalRect;
	cliking = false;
	to_delete = false;
}

GuiButton::~GuiButton() 
{
	delete text;
	delete texture;
	parent = nullptr;
	texture = nullptr;
	callback = nullptr;
}

void GuiButton::Init(iPoint pos, SDL_Rect normal, SDL_Rect hover, SDL_Rect click, p2SString txt, ButtonAction _action, bool stay) 
{
	screenPos = pos;
	texture = (SDL_Texture*)App->gui->GetAtlas();
	normalRect = normal;
	hoverRect = hover;
	clickRect = click;
	action = _action;
	cliking = stay;

	if (parent != nullptr)
	{
		localPos.x = screenPos.x - parent->screenPos.x;
		localPos.y = screenPos.y - parent->screenPos.y;
	}

	rect = normalRect;

	SDL_Rect text_rect;

	App->font->CalcSize(txt.GetString(), text_rect.w, text_rect.h);
	text_rect.x = screenPos.x + rect.w * 0.5f - text_rect.w * 0.5f;
	text_rect.y = screenPos.y + rect.h * 0.5f - text_rect.h * 0.5f;

	text->parent = this;
	text->Init({ text_rect.x, text_rect.y }, txt);
}

bool GuiButton::CleanUp()
{
	bool ret = true;
	text->CleanUp();
	delete text;
	parent = nullptr;
	text = nullptr;
	texture = nullptr;
	callback = nullptr;
	return ret;
}

bool GuiButton::Input()
{
	if (cliking) 
	{
		if (currentRect == &clickRect) currentRect = &normalRect;
		else currentRect = &clickRect;
	}
	else currentRect = &clickRect;
	if (callback != NULL) callback->OnEvent(this, FocusEvent::CLICKED);
	return true;
}

bool GuiButton::Update(float dt) 
{
	bool ret = true;
	if (!cliking) 
	{
		if (OnHover())
		{			
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) currentRect = &clickRect;			
			currentRect = &hoverRect;
		}
		else currentRect = &normalRect;		
	}

	if (parent != nullptr)
	{
		screenPos.x = parent->screenPos.x + localPos.x;
		screenPos.y = parent->screenPos.y + localPos.y;
	}

	rect.x = screenPos.x - App->render->camera.x;
	rect.y = screenPos.y - App->render->camera.y;

	if (text->text.Length() > 0) text->Update(dt);

	return true;
}

bool GuiButton::Draw() 
{
	App->render->Blit(texture, rect.x, rect.y, currentRect);
	if (text->text.Length() > 0) { text->Draw(); }
	return true;
}

