#include "GuiInputText.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"


GuiInputText::GuiInputText(j1Module* _callback) 
{
	background = new GuiImage(callback);
	text = new GuiText(callback);
	callback = _callback;
	defaultText = { "Input text" };
	mouse = { 0,0,2,30 };
	focused = false;
	usingAtlas = true;
	mousePos = 0;
	to_delete = false;
}

GuiInputText::~GuiInputText() {}

bool GuiInputText::CleanUp()
{
	bool ret = true;
	ret = background->CleanUp();
	delete background;
	background = nullptr;
	ret = text->CleanUp();
	delete text;
	text = nullptr;
	return ret;
}

void GuiInputText::Init(iPoint pos, p2SString _text, SDL_Rect imgSection, bool useAtlas, char* _font) 
{
	screenPos = pos;
	rect = { pos.x, pos.y, imgSection.w, imgSection.h };
	defaultText = _text;

	background->parent = text->parent = (j1UI_Element*)this;
	background->draggable = text->draggable = false;

	rect.x = screenPos.x;
	rect.y = screenPos.y;

	if (parent != nullptr)
	{
		localPos.x = screenPos.x - parent->screenPos.x;
		localPos.y = screenPos.y - parent->screenPos.y;
	}

	if (useAtlas == false) usingAtlas = false;
	if (useAtlas) background->Init(pos, imgSection);

	text->Init({ pos.x + 10,(int)(pos.y + rect.h * 0.25f) }, _text, _font);
}

bool GuiInputText::Input()
{
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		int end;
		end = text->text.Length();
		text->text.Cut(end - 2, end);
		UpdateText();
	}
	return true;
}

bool GuiInputText::Update(float dt) 
{
	bool ret = true;
	int width;

	if (parent != nullptr)
	{
		screenPos.x = parent->screenPos.x + localPos.x;
		screenPos.y = parent->screenPos.y + localPos.y;
	}
	rect.x = screenPos.x - App->render->camera.x;
	rect.y = screenPos.y - App->render->camera.y;

	if (usingAtlas) background->Update(dt);
	text->Update(dt);

	if (text->text.Length() > 0)
	{
		App->font->CalcSize(text->text.GetString(), width, mouse.y, App->font->console_font);
		mouse.x = background->GetScreenRect().x + mouse.w + 20 + width;
		mouse.y = rect.y + rect.h * 0.5f - mouse.h * 0.5f;
	}
	return ret;
}

bool GuiInputText::Draw()
{
	if (usingAtlas) background->Draw();
	text->Draw();
	if (focused)App->render->DrawQuad(mouse, 255, 255, 255, 255);
	return true;
}

void GuiInputText::HandleFocusEvent(FocusEvent event) 
{
	if (event == FocusEvent::FOCUS_IN) {
		if (text->text == defaultText)
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

void GuiInputText::UpdateText()
{
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

GuiText* GuiInputText::GetText() const { return text; }