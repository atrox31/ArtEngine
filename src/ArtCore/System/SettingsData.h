#pragma once
#include <map>
#include <string>
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Gui/Console.h"

class SettingsData {
private:
	// settings contain all settings data
	inline static std::map<std::string, std::string> _settings_data = std::map<std::string, std::string>();
	inline static std::map<std::string, std::string> _settings_data_user = std::map<std::string, std::string>();
	inline static std::string _fl_platform_file = "setup_user.ini";
public:
    	// Get value from Settings Data in int format
	static int GetInt(const std::string& field, const int _default) {
        if (_settings_data.contains(field)) {
            return Func::TryGetInt(_settings_data[field]);
        }
        if (_settings_data_user.contains(field)) {
            return Func::TryGetInt(_settings_data_user[field]);
        }
#ifdef _DEBUG
        Console::WriteLine("SettingsData::GetInt: can not find '" + field + "'");
#endif
        return _default;
    }

	// Get value from Settings Data in float format
	static float GetFloat(const std::string& field, const float _default) {
        if (_settings_data.contains(field)) {
            return Func::TryGetFloat(_settings_data[field]);
        }
        if (_settings_data_user.contains(field)) {
            return Func::TryGetFloat(_settings_data_user[field]);
        }
#ifdef _DEBUG
        Console::WriteLine("SettingsData::GetInt: can not find '" + field + "'");
#endif
        return _default;
    }

	// Get value from Settings Data in string format
	static std::string GetString(const std::string& field, const std::string& _default) {
        if (_settings_data.contains(field)) {
            return _settings_data[field];
        }
        if (_settings_data_user.contains(field)) {
            return _settings_data_user[field];
        }
#ifdef _DEBUG
        Console::WriteLine("SettingsData::GetInt: can not find '" + field + "'");
#endif
        return _default;
    }

	// set setting data value, 
	static void SetValue(const std::string& field, const std::string& value) {
		if(field.length() > 0 && field.starts_with("AC"))
        {
            bool new_value = false;
            if (_settings_data_user.contains(field)) {
                _settings_data_user[field] = value;
            }
            else {
                _settings_data_user.emplace(field, value);
            }
            return;
        }
        // can not rewrite core settings
        if (_settings_data.contains(field)) {
            Console::WriteLine("Can not set property of: '" + field + "'. Its read only.");
        }else
        {
            _settings_data.emplace(field, value);
        }
    }

    // read all user settings data
    static bool LoadUserData(const std::string& settings_file)
	{
        Console::WriteLine("SettingsData::LoadUserData() - not implemented");
        return true;

        if(!Func::FileExists(settings_file))
        {
            return false;
        }
        _fl_platform_file = settings_file;

        // load file
        // parse file
        // close file
        //_settings_data.emplace(field, value);

        return true;
	}

	// write all values to file
	static void WriteValues()
	{
        Console::WriteLine("SettingsData::WriteValues() - not implemented");
	}
private:
};
