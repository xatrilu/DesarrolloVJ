#include "j1UI_Element.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"

void j1UI_Element::SetLocalPos(iPoint pos)
{
	local_pos = pos;
}

bool j1UI_Element::OnHover()
{
	iPoint mouse;
	iPoint camera;
	camera.x = App->render->camera.x;
	camera.y = App->render->camera.y;

	bool ret = false;

	App->input->GetMousePosition(mouse.x, mouse.y);
	mouse.x -= camera.x;
	mouse.y -= camera.y;

	if ((mouse.x > rect.x) && (mouse.x < rect.x + rect.w) && (mouse.y > rect.y) && (mouse.y < rect.y + rect.h)) //Check mouse positions
		ret = true;

	return ret;
}

iPoint j1UI_Element::GetScreenPos() const 
{
	iPoint pos;
	pos.x = rect.x;
	pos.y = rect.y;
	return pos;
}

iPoint j1UI_Element::GetLocalPos() const 
{
	iPoint pos;
	if (parent != nullptr)
	{
		pos.x = rect.x - parent->GetScreenPos().x;
		pos.y = rect.y - parent->GetScreenPos().y;
	}
	else
	{
		pos.x = rect.x;
		pos.y = rect.y;
	}
	return pos;
}

SDL_Rect j1UI_Element::GetScreenRect() const { return rect; }

SDL_Rect j1UI_Element::GetLocalRect() const 
{
	SDL_Rect local_rect = rect;
	if (parent != nullptr)
	{
		local_rect.x = rect.x - parent->GetScreenPos().x;
		local_rect.y = rect.y - parent->GetScreenPos().y;
	}
	return local_rect;
}