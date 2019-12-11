#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1Scene;
class j1Map;
class j1Particles;
class j1Collision;
class j1UI;
class j1EntityManager;
class j1PathFinding;
class j1WalkingEnemy;
class j1WalkingEnemy2;
class j1FlyingEnemy;
class j1Trap;
class j1FadeToBlack;


class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	bool Awake();
	bool Start();
	bool Update();

	bool CleanUp();

	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Scene*			scene;
	j1Map*				map;
	j1Particles*		particles;
	j1Collision*		collision;
	j1UI*				ui;
	j1EntityManager*	entities;
	j1PathFinding*		pathfinding;
	j1FadeToBlack*		fade_to_black;

	mutable p2SString	checkpoint_save;

	bool pause = false;
	bool vsync = false;
	bool cap_enabled = true;

private:

	p2List<j1Module*>	modules;
	uint				frames;
	float				dt;
	int					argc;
	char**				args;

	p2SString			title;
	p2SString			organization;
	double				framerate_cap;

	//save-load
	mutable bool		want_to_save;
	bool				want_to_load;
	p2SString			load_game;
	mutable p2SString	save_game;

	//framerate
	j1Timer				frame_time;
	j1Timer				startup_time;
	j1PerfTimer			perf_timer;
	j1Timer				last_sec_frame_time;
	uint32				prev_last_sec_frame_count = 0;
	uint32				last_sec_frame_count = 0;
	uint64				frame_count = 0;
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif