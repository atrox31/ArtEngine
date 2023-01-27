#pragma once
#include <vector>

#include "Instance.h"
#include "ArtCore/ShortTypenames.h"
#include "ArtCore/Gui/Gui.h"
#include "ArtCore/System/Core.h"

#include "plf/plf_colony-master/plf_colony.h"

class Instance;
class Scene final
{
public:
	// ReSharper disable once CppInconsistentNaming

	// Maximum of levels count.
	static inline constexpr int SCENE_MAX_LEVEL_COUNT = 128;
private: // private constructors
	Scene();
	~Scene();
	bool LoadFromFile(const std::string& file);
	bool SpawnLevelInstances(Scene* target = nullptr) const;
	inline static Scene* _current_scene = nullptr;

public: // Audio
	// Scene is audio manager, it can play music and sounds
	struct Audio
	{
		/**
		 * \brief Play new music
		 * \param music Music asset
		 * \param loops 0 - play once, -1 - play forever, 1 - play twice, etc.
		 */
		static void PlayMusic(Mix_Music* music, int loops = 0);

		/**
		 * \brief Pause current music
		 */
		static void PauseMusic();

		/**
		 * \brief Resume current music\n
		 * To play new music use: PlayMusic(Mix_Music* music, int loops = 0)
		 */
		static void PlayMusic();

		/**
		 * \brief Stop current music
		 */
		static void StopMusic();

		/**
		 * \brief Check the playing status of the music\n
		 * Paused music is treated as playing
		 * \return Answer to question
		 */
		static bool IsMusicPlaying();

		/**
		 * \brief Play sound effect
		 * \param sound Sound asset
		 * \param loops 0 - play once, -1 - play forever, 1 - play twice, etc.
		 */
		static void PlaySound(Mix_Chunk* sound, int loops = 0);

		/**
		 * \brief Play sound effect at specified position in 2D space
		 * \param sound Sound asset
		 * \param x x-coordinate
		 * \param y y-coordinate
		 */
		static void PlaySoundAt(Mix_Chunk* sound, float x, float y);

		/**
		 * \brief Play sound effect at specified position in 2D space
		 * \param sound Sound asset
		 * \param point Scene point to play sound at
		 */
		static void PlaySoundAt(Mix_Chunk* sound, const SDL_FPoint& point);

		/**
		 * \brief Stop all sounds
		 */
		static void StopAllSounds();
	private:
		friend class Scene;
		/**
		 * \brief Update sound position in audio 3D system
		 */
		static void UpdateSoundPosition();

		/**
		 * \brief Struct contains information about sound position in 2D space
		 */
		struct SoundPosition {
			float X, Y;
			int Channel;
			bool Done;
			SoundPosition(const float x, const float y, Mix_Chunk* chunk) {
				this->X = x;
				this->Y = y;
				this->Channel = Mix_PlayChannel(-1, chunk, 0);
				this->Done = false;
			}
		};
		
	};
private:
	/**
	 * \brief List of sound positions in 2D space
	 */
	std::vector<Audio::SoundPosition*> _sound_position_list{};

public: // common methods
	/// <summary>
	/// Getter for current scene
	/// </summary>
	/// <returns>Current scene reference</returns>
	static Scene* GetCurrentScene() { return _current_scene;  }

	/// <summary>
	/// Create new scene and load it, if success change core->current_scene to new created scene
	/// </summary>
	/// <param name="name">name name of scene file</param>
	/// <returns>success state</returns>
	static bool Create(const std::string& name);

	/// <summary>
	/// Clear all level data, reset current level.
	/// </summary>
	static void Reset();

	/// <summary>
	/// Start new level in current scene
	/// </summary>
	/// <param name="level">level level to load</param>
	/// <returns>success state</returns>
	static bool Start(int level = 0);

	/// <summary>
	/// Load next level
	/// </summary>
	/// <returns>success state, or false if there is no next level</returns>
	static bool StartNextLevel();

	/// <summary>
	/// Check if this level is last
	/// </summary>
	/// <returns>Return answer to question</returns>
	static bool HaveNextLevel();

	// core getters for game loop
	static plf_it<Instance*> ColonyGetBegin() { return _current_scene->_instance_colony.begin();  }
	static plf_it<Instance*> ColonyGetEnd() { return _current_scene->_instance_colony.end();  }
	static plf_it<Instance*> ColonyErase(const plf_it<Instance*>& it);


	// create new instance
	static Instance* CreateInstance(const std::string& name, float x, float y, Scene* target = nullptr);
	// copy instance
	static Instance* CreateInstance(Instance* instance, Scene* target = nullptr);

	// instance functions
	// single
	[[nodiscard]] static Instance*					GetInstanceById(int);
	[[nodiscard]] static Instance*					GetInstanceByTag(const std::string&);
	[[nodiscard]] static Instance*					GetInstanceByName(const std::string&);
	// multi
	[[nodiscard]] static std::vector< Instance* >	GetInstancesByTag(const std::string&);
	[[nodiscard]] static std::vector< Instance* >	GetInstancesByName(const std::string&);
	// counters
	[[nodiscard]] static int						GetInstancesCount()								{ return _current_scene->_instances_size; }
	[[nodiscard]] static int						GetInstancesCountByTag(const std::string& tag)  { return static_cast<int>(GetInstancesByTag(tag).size()); }
	[[nodiscard]] static int						GetInstancesCountByName(const std::string& name){ return static_cast<int>(GetInstancesByName(name).size()); }

	[[nodiscard]] static bool						IsAnyInstances()								{ return _current_scene->_instances_size > 0; }
	// get current scene gui system
	[[nodiscard]] static Gui&						GuiSystem()										{ return _current_scene->_gui_system; }
	// return if point is in camera view
	[[nodiscard]] static bool						InView(const SDL_FPoint& p)						{ return _current_scene->_view_rect.PointInRectWh(p); }

	// non static getters

	// spawn all crated instance
	void SpawnAll();

	[[nodiscard]] std::string GetName()	const		{ return _name;}
	[[nodiscard]] int		  GetWidth()  const { return _width;}
	[[nodiscard]] int		  GetHeight() const { return _height; }
	// get local scene gui system
	[[nodiscard]] Gui&		  GetLocalGuiSystem()			{ return _gui_system; }
	[[nodiscard]] Instance*   GetVariableHolder()	const	{ return _variables_holder;}
				  void		  SetVariableHolder(Instance* i){ _variables_holder = i;}

	void SetTriggerData(const std::string& trigger, const unsigned char* data, Sint64 length);
	std::pair<const unsigned char*, Sint64>* GetTriggerData(const std::string& trigger);
	static void Delete();
private:
	static void ClearListNewInstance(Scene* target);
	static void ClearListColonyInstance(Scene* target);

public: // background data
	struct BackGround {
		GPU_Image* Texture;
		SDL_Color Color;
		enum class BType {
			DrawColor, DrawTexture
		} Type;

		ENUM_WITH_STRING_CONVERSION(BTypeWrap, (Tile)(TileFlipX)(TileFlipY)(TileFlipXY))
			BTypeWrap TypeWrap;
		void SetDefault()
		{
			Type = Scene::BackGround::BType::DrawColor;
			Color = SDL_Color({ 0, 0, 0, 255 });
		}
	} BackGround{};
private:
	// level data
	int _level_current;
	int _level_last;

	Gui _gui_system{};

	int _width;
	int _height;
	Rect _view_rect;
	bool _enable_camera;

	std::string _name;

	bool _is_any_new_instances;
	int _instances_size;
	// list of all active instances
	plf::colony<Instance*> _instance_colony{};
	// list of instances ready to spawn
	std::vector<Instance*> _instances_new{};
	// list of scene instances, there are not deleted on level change
	std::vector<std::pair<std::string, SDL_FPoint>> _instances_scene{};

	// Object that holds all scene variables, must be this way
	// because script system is designed this way
	Instance* _variables_holder = nullptr;

	// triggers
	std::unordered_map<std::string, std::pair<const unsigned char*, Sint64>> _trigger_data{};
	std::string _begin_trigger;
};
