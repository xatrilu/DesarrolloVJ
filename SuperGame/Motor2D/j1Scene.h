#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Timer.h"

#define CAMERA_SPEED 200
#define CAMERA_CENTER_MARGIN 30

struct SDL_Texture;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputText;

enum class Menu {
	MAIN_MENU,
	SETTINGS,
	PAUSE,
	CREDITS,
	SCREEN_UI,
	NO_MENU
};

enum Map
{
	NO_MAP,
	LEVEL_1,
	LEVEL_2,
};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);
	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();
	// Called each loop iteration
	bool Update(float dt);
	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LevelChange(Map unloading_map, Map loading_map);
	void CreatePauseMenu();
	void DeletePauseMenu();
	void DestroySpecificMenu(Menu menu);

	void OnEvent(j1UI_Element*, FocusEvent);
	void Commands(p2SString command);

	void CreateScreenUI();

	bool Save(pugi::xml_node& data) const;
	bool Load(pugi::xml_node& data);
	void UpdateScreenUI();

	void GameOver();

public:
	//edges
	SDL_Rect camera_frame;
	int left_edge;
	int right_edge;
	int top_edge;
	int bottom_edge;

	int camera_frame_x_margin;
	int camera_frame_y_margin;

	bool blocked_camera;
	bool showing_menu;
	bool fullscreen;
	int winWidth;
	int winHeight;

	//UI Info

	//int score;
	GuiText* score;
	p2List<GuiImage*> lives;
	GuiImage* stars;

	//levels
	Map currentLevel;
	Menu openMenu;

	p2SString song;
	p2SString folder;

	int camera_margin;
	iPoint startCameraPos;

	//pause menu
	p2List<j1UI_Element*> pause_menu;
	p2List<j1UI_Element*> ScreenUI;

private:
	int uiLives;
	int uiScore;
};

#endif // __j1SCENE_H__