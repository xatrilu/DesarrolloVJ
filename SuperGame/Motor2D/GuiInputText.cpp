#include "GuiInputText.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"

GuiInputText::GuiInputText(j1Module* g_callback, bool Static) {
	background = new GuiImage(g_callback, Static);
	text = new GuiText(g_callback,Static);
	callback = g_callback;
	default_text = { "Input text" };
	mouse = { 0,0,2,30 };
	focused = false;
	usingAtlas = true;
	mouse_pos = 0;
	isStatic = Static;
}

GuiInputText::~GuiInputText() {
	delete background;
	delete text;
}

void GuiInputText::Init(iPoint pos, p2SString g_text, SDL_Rect image_section, bool useAtlas) {

	screen_pos = pos;
	rect = { pos.x, pos.y, image_section.w, image_section.h };
	default_text = g_text;

	background->parent = text->parent = (j1UI_Element*)this;
	background->draggable = text->draggable = false;

	rect.x = screen_pos.x;
	rect.y = screen_pos.y;

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}

	if (useAtlas = false)
		usingAtlas = false;

	if (useAtlas) background->Init(pos, image_section);

	text->Init({ pos.x + 10,(int)(pos.y + rect.h * 0.25f) }, g_text);
}

bool GuiInputText::Input() {
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		int end;
		end = text->text.Length();
		text->text.Cut(end - 2, end);
	}
	return true;
}

bool GuiInputText::Update(float dt) {
	bool ret = true;
	int width;

	if (parent != nullptr)
	{
		screen_pos.x = parent->screen_pos.x + local_pos.x;
		screen_pos.y = parent->screen_pos.y + local_pos.y;
	}

	rect.x = screen_pos.x;
	rect.y = screen_pos.y;

	if (usingAtlas) background->Update(dt);
	text->Update(dt);

	if (text->text.GetCapacity() > 0)
	{
		App->font->CalcSize(text->text.GetString(), width, mouse.y);
		mouse.x = background->GetScreenRect().x + mouse.w + 10 + width;
		mouse.y = rect.y + rect.h * 0.5f - mouse.h * 0.5f;
	}


	return ret;
}

bool GuiInputText::Draw() {

	if (usingAtlas) background->Draw();

	text->Draw();

	if (focused)App->render->DrawQuad(mouse, 255, 255, 255, 255);

	return true;
}

void GuiInputText::HandleFocusEvent(FocusEvent event) {
	if (event == FocusEvent::FOCUS_IN) {
		if (text->text == default_text)
		{
			text->text.Clear();
		}
		App->input->EnableTxtInput(true);
		App->input->text = (char*)text->text.GetString();
		focused = true;
	}

	if (event == FocusEvent::FOCUS_OUT)
	{
		focused = false;
		App->input->EnableTxtInput(false);
	}
}

GuiText* GuiInputText::GetText() const {
	return text;
}