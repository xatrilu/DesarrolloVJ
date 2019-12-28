#include "GuiInputText.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"


GuiInputText::GuiInputText(j1Module* g_callback) {
	background = new GuiImage(g_callback);
	text = new GuiText(g_callback);
	callback = g_callback;
	default_text = { "Input text" };
	cursor = { 0,0,2,30 };
	focused = false;
	usingAtlas = true;
	cursor_position = 0;
}

GuiInputText::~GuiInputText() {}

bool GuiInputText::CleanUp() {
	bool ret = true;
	ret = background->CleanUp();
	delete background;
	background = nullptr;
	ret = text->CleanUp();
	delete text;
	text = nullptr;
	return ret;
}

void GuiInputText::Init(iPoint position, p2SString g_text, SDL_Rect image_section, bool useAtlas, char* g_font) {

	screen_pos = position;
	rect = { position.x, position.y, image_section.w, image_section.h };
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

	if (useAtlas) background->Init(position, image_section);

	text->Init({ position.x + 10,(int)(position.y + rect.h * 0.25f) }, g_text, g_font);
}

bool GuiInputText::Input() {
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		int end;
		end = text->text.Length();
		text->text.Cut(end - 2, end);
		UpdateText();
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

	rect.x = screen_pos.x - App->render->camera.x;
	rect.y = screen_pos.y - App->render->camera.y;

	if (usingAtlas) background->Update(dt);
	text->Update(dt);

	if (text->text.GetCapacity() > 0)
	{
		App->font->CalcSize(text->text.GetString(), width, cursor.y, App->font->console_font);
		cursor.x = background->GetScreenRect().x + cursor.w + 20 + width;
		cursor.y = rect.y + rect.h * 0.5f - cursor.h * 0.5f;
	}


	return ret;
}

bool GuiInputText::Draw() {

	if (usingAtlas) background->Draw();

	text->Draw();

	if (focused)App->render->DrawQuad(cursor, 255, 255, 255, 255);

	return true;
}

void GuiInputText::HandleFocusEvent(FocusEvent event) {
	if (event == FocusEvent::FOCUS_IN) {
		if (text->text == default_text)
		{
			text->text.Clear();
		}
		App->input->EnableTextInput(true);
		App->input->text = (char*)text->text.GetString();
		focused = true;
	}

	if (event == FocusEvent::FOCUS_OUT)
	{
		focused = false;
		App->input->EnableTextInput(false);
	}
}

GuiText* GuiInputText::GetText() const {
	return text;
}

void GuiInputText::UpdateText() {
	if (text != nullptr)
	{
		if (text->font == DEFAULT_FONT)
		{
			text->texture = App->font->Print(text->text.GetString());
		}
		else
		{
			text->texture = App->font->Print(text->text.GetString(), { (255),(255),(255),(255) }, App->font->console_font);
		}
	}
}