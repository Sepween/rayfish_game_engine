#include "Game.h"
#include "LuaManager.h"
#include "EventBus.h"

Game::Game() :
	new_scene(false),
	exiting(false),
	cur_scene_name(""),
	new_scene_name(""),
	sceneDB(SceneDB::getInstance()),
	renderer(Renderer::getInstance()),
	audioDB(AudioDB::getInstance()),
	luaManager(LuaManager::getInstance()),
	inputManager(Input::getInstance()),
	lua_state(nullptr) {
	// load lua state
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	// load resources
	checkLoadResources();
}

void Game::run() {
	// initialize helpers
	luaManager.init(lua_state);
	sceneDB.init(cur_scene_name);
	renderer.init();
	inputManager.Init();
	
	luaManager.processOnStartQueue();
	
	while (!inputManager.GetExit()) {
		// switch scene
		if (getInstance().new_scene) {
			enterNewScene();
			luaManager.processOnStartQueue();
		}

		// process input
		SDL_Event e;
		while (Helper::SDL_PollEvent(&e)) {
			inputManager.ProcessEvent(e);
		}
		
		// updates
		luaManager.processUpdate(false);
		inputManager.LateUpdate();
		luaManager.processUpdate(true);

		EventBus::getInstance().processSubscriptions();
		Rigidbody::StepWorld();

		// rendering
		renderer.renderGame();
		ImageDB::getInstance().lateUpdate();

		// adding/destroying actors/components
		luaManager.processAddComponent();
		luaManager.processRemoveComponent();
		sceneDB.processActorAdditions();
		sceneDB.processActorDestroy();

		luaManager.processOnStartQueue();
	}

}

void Game::checkLoadResources() {
	if (!EngineUtils::InitializeConfigs()) {
		exit(0);
	}
	rapidjson::Document& gameDoc = EngineUtils::game_config;

	EngineUtils::loadFromDoc(gameDoc, "initial_scene", cur_scene_name);
}

void Game::enterNewScene() {
	// switch to new scene
	getInstance().new_scene = false;
	getInstance().cur_scene_name = getInstance().new_scene_name;
	if (!sceneDB.enterNewScene(getInstance().cur_scene_name)) {
		cout << "error: scene " << getInstance().cur_scene_name << " is missing";
		exit(0);
	}
}

void Game::Load(const string& scene_name) {
	getInstance().new_scene = true;
	getInstance().new_scene_name = scene_name;
}

string Game::GetCurrent() {
	return getInstance().cur_scene_name;
}

void Game::DontDestroy(luabridge::LuaRef actor) {
	if (actor.isUserdata()) {
		Actor* actorPtr = actor.cast<Actor*>();
		if (actorPtr && !actorPtr->dontDestroy) {
			actorPtr->dontDestroy = true;
			getInstance().sceneDB.addDontDestroyActor(actorPtr->id);
		}
	}
}