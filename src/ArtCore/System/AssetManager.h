#pragma once
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/Sprite.h"
#include "FC_Fontcache/SDL_FontCache.h"
#include "ArtCore/main.h" // for debug flags

// all macros to define asset manager
#pragma region All macros to define asset manager

#if AC_ENABLE_DEBUG_MODE														
	#define CREATE_DEBUG_LIST(getter_name)	std::vector<std::string> Debug_List_##getter_name##_name;
#else
	#define CREATE_DEBUG_LIST(getter_name)
#endif
#define GENERATE_GETTER_FUNCTION_DEFINITION(type, getter_name)					\
	public:																		\
	type* Get##getter_name(int id) const;										\
	type* Get##getter_name(const std::string& name);							\
	int Get##getter_name##Id(const std::string& name) const;					\
	CREATE_DEBUG_LIST(getter_name)					\
	private:																	\
	std::vector<type*> _list_##getter_name##_id;								\
	std::map< std::string, type*> _list_##getter_name##_name;
																	
/// ////////////////////////////////////////////////////////////////////// ///
// This macro is used to create getters functions for each asset type
#define GENERATE_GETTER_FUNCTION(type, asset_Name)								\
type* AssetManager::Get##asset_Name(const int id) const							\
{																				\
	if (static_cast<size_t>(id) < _list_##asset_Name##_id.size()) {				\
		return _list_##asset_Name##_id[id];										\
	}																			\
	return nullptr;																\
}																				\
type* AssetManager::Get##asset_Name(const std::string& name)					\
{																				\
	if (_list_##asset_Name##_name.contains(name)) {								\
		return _list_##asset_Name##_name[name];									\
	}																			\
	return nullptr;																\
}																				\
int AssetManager::Get##asset_Name##Id(const std::string& name) const			\
{																				\
	int i = -1;																	\
	for (const auto& key : _list_##asset_Name##_name | std::views::keys) {		\
		if (key == name) {														\
			return ++i;															\
		}																		\
		++i;																	\
	}																			\
	return -1;																	\
}
/// ///////////////////////////////////////////////////////////////////////// ///
#if AC_ENABLE_DEBUG_MODE
	#define WRITE_DEBUG_LIST(asset_Name)										\
		Console::WriteLine("AssetManager::Clear(): delete "+					\
		Debug_List_##asset_Name##_name[i++]);									
	#define CLEAN_DEBUG_LIST(asset_Name) Debug_List_##asset_Name##_name.clear();									
#else
	#define WRITE_DEBUG_LIST(asset_Name)
	#define CLEAN_DEBUG_LIST(asset_Name)
#endif
#define ADD_CLEAR_ASSET(asset_Name, delete_method)								\
for (int i=0;auto* ref : _list_##asset_Name##_id) {								\
delete_method;																	\
WRITE_DEBUG_LIST(asset_Name)													\
}																				\
_list_##asset_Name##_id.clear();												\
_list_##asset_Name##_name.clear();												\
CLEAN_DEBUG_LIST(asset_Name)
/// ///////////////////////////////////////////////////////////////////////// ///
#if AC_ENABLE_DEBUG_MODE
#define DEBUG_OUTPUT(asset_Name)												\
	Console::WriteLine("AssetManager::LoadData - loaded: " +					\
	std::string(#asset_Name) + " " + asset_data.second);						
#else
#define DEBUG_OUTPUT(asset_Name)
#endif
#define ADD_LOAD_ASSET(type, asset_Name, method)								\
	if(asset_type == #asset_Name ) {											\
		type* tmp = method;														\
		if (tmp == nullptr) return false;										\
		_list_##asset_Name##_name.insert({ asset_data.second, tmp });			\
		DEBUG_OUTPUT(asset_Name)												\
		continue;																\
	}
/// ///////////////////////////////////////////////////////////////////////// ///
///	This fave 2 versions, with debug asset names and not
#if AC_ENABLE_DEBUG_MODE
	#define FILL_ASSET_ID_LIST(asset_Name)										\
	for (auto& val : _list_##asset_Name##_name) {								\
		_list_##asset_Name##_id.push_back(val.second);							\
		Debug_List_##asset_Name##_name.push_back(val.first);					\
	}
#else
	#define FILL_ASSET_ID_LIST(asset_Name)										\
		for (auto* val : _list_##asset_Name##_name | std::views::values) {		\
			_list_##asset_Name##_id.push_back(val);								\
		}
#endif
#pragma endregion macros

class BackGroundRenderer;
/**
 * \brief Asset manager is full of macros, but it's very useful. Many lines of code is doubled.\n
 * Contains all assets, and can load them from filelist.txt\n
 * All data are stored in two containers: one for id and one for name.\n
 */
class AssetManager final
{
public:
	AssetManager() = default;
	~AssetManager()	{ ClearData(); }

	bool LoadData(const BackGroundRenderer* bgr, int p_min, int p_max);
	void ClearData();

	GENERATE_GETTER_FUNCTION_DEFINITION(Sprite,		Sprite)
	GENERATE_GETTER_FUNCTION_DEFINITION(GPU_Image,	Texture)
	GENERATE_GETTER_FUNCTION_DEFINITION(Mix_Music,	Music)
	GENERATE_GETTER_FUNCTION_DEFINITION(Mix_Chunk,	Sound)
	GENERATE_GETTER_FUNCTION_DEFINITION(FC_Font,	Font)
		
#undef GENERATE_GETTER_FUNCTION_DEFINITION
};

