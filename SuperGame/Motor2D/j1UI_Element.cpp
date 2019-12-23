#include "j1UI_Element.h"
#include "j1App.h"
#include "j1Input.h"

bool j1UI_Element::MouseHovering() {
	iPoint mouse;
	bool ret = false;

	App->input->GetMousePosition(mouse.x, mouse.y);

	if ((mouse.x > rect.x) && (mouse.x < rect.x + rect.w)
		&& (mouse.y > rect.y) && (mouse.y < rect.y + rect.h))
	{
		ret = true;
	}

	return ret;
}

iPoint j1UI_Element::GetScreenPos() const {
	iPoint position;
	position.x = rect.x;
	position.y = rect.y;
	return position;
}

iPoint j1UI_Element::GetLocalPos() const {
	iPoint position;
	if (parent != nullptr)
	{
		position.x = rect.x - parent->GetScreenPos().x;
		position.y = rect.y - parent->GetScreenPos().y;
	}
	else
	{
		position.x = rect.x;
		position.y = rect.y;
	}
	return position;
}

SDL_Rect j1UI_Element::GetScreenRect() const {
	return rect;
}

SDL_Rect j1UI_Element::GetLocalRect() const {
	SDL_Rect local_rect = rect;

	if (parent != nullptr)
	{
		local_rect.x = rect.x - parent->GetScreenPos().x;
		local_rect.y = rect.y - parent->GetScreenPos().y;
	}

	return local_rect;
}