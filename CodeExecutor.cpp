#include "CodeExecutor.h"
#include "Func.h"
#include "Debug.h"
#include <algorithm>

AStack<std::string> CodeExecutor::GlobalStack = AStack<std::string>();

CodeExecutor::CodeExecutor()
{
	FunctionsMap = std::map<std::string, void(*)(Instance*)>();
	FunctionsList = std::vector<void(*)(Instance*)>() ;
	InstanceDefinitions = std::vector<InstanceDefinition>();
	_break = false;
}

void CodeExecutor::MapFunctions()
{
	// map all functions to string
	FunctionsMap.clear();
	FunctionsMap["new_point"] = &CodeExecutor::new_point;
	FunctionsMap["new_direction"] = &CodeExecutor::new_direction;
	FunctionsMap["new_rectangle"] = &CodeExecutor::new_rectangle;
	FunctionsMap["new_rectangle_f"] = &CodeExecutor::new_rectangle_f;
	FunctionsMap["new_mask_from_sprite"] = &CodeExecutor::new_mask_from_sprite;
	FunctionsMap["get_sprite"] = &CodeExecutor::get_sprite;
	FunctionsMap["get_texture"] = &CodeExecutor::get_texture;
	FunctionsMap["get_music"] = &CodeExecutor::get_music;
	FunctionsMap["get_sound"] = &CodeExecutor::get_sound;
	FunctionsMap["get_font"] = &CodeExecutor::get_font;
	FunctionsMap["sprite_get_width"] = &CodeExecutor::sprite_get_width;
	FunctionsMap["sprite_get_height"] = &CodeExecutor::sprite_get_height;
	FunctionsMap["sprite_get_frames"] = &CodeExecutor::sprite_get_frames;
	FunctionsMap["sprite_set_animation_speed"] = &CodeExecutor::sprite_set_animation_speed;
	FunctionsMap["sprite_set_animation_loop"] = &CodeExecutor::sprite_set_animation_loop;
	FunctionsMap["move_to_point"] = &CodeExecutor::move_to_point;
	FunctionsMap["move_forward"] = &CodeExecutor::move_forward;
	FunctionsMap["move_instant"] = &CodeExecutor::move_instant;
	FunctionsMap["move_to_direction"] = &CodeExecutor::move_to_direction;
	FunctionsMap["distance_to_point"] = &CodeExecutor::distance_to_point;
	FunctionsMap["distance_beetwen_point"] = &CodeExecutor::distance_beetwen_point;
	FunctionsMap["distance_to_instance"] = &CodeExecutor::distance_to_instance;
	FunctionsMap["draw_sprite"] = &CodeExecutor::draw_sprite;
	FunctionsMap["draw_sprite_ex"] = &CodeExecutor::draw_sprite_ex;
	FunctionsMap["draw_texture"] = &CodeExecutor::draw_texture;
	FunctionsMap["draw_texture_ex"] = &CodeExecutor::draw_texture_ex;
	FunctionsMap["draw_sprite_self"] = &CodeExecutor::draw_sprite_self;
	FunctionsMap["draw_shape_rectangle"] = &CodeExecutor::draw_shape_rectangle;
	FunctionsMap["draw_shape_rectangle_r"] = &CodeExecutor::draw_shape_rectangle_r;
	FunctionsMap["draw_shape_rectangle_filled"] = &CodeExecutor::draw_shape_rectangle_filled;
	FunctionsMap["draw_shape_rectangle_filled_r"] = &CodeExecutor::draw_shape_rectangle_filled_r;
	FunctionsMap["draw_shape_circle"] = &CodeExecutor::draw_shape_circle;
	FunctionsMap["draw_shape_circle_p"] = &CodeExecutor::draw_shape_circle_p;
	FunctionsMap["draw_shape_circle_filled"] = &CodeExecutor::draw_shape_circle_filled;
	FunctionsMap["draw_shape_circle_filled_p"] = &CodeExecutor::draw_shape_circle_filled_p;
	FunctionsMap["math_min_i"] = &CodeExecutor::math_min_i;
	FunctionsMap["math_max_i"] = &CodeExecutor::math_max_i;
	FunctionsMap["math_min"] = &CodeExecutor::math_min;
	FunctionsMap["math_max"] = &CodeExecutor::math_max;
	FunctionsMap["global_get_mouse"] = &CodeExecutor::global_get_mouse;
	FunctionsMap["set_self_sprite"] = &CodeExecutor::set_self_sprite;
	FunctionsMap["get_pos_x"] = &CodeExecutor::get_pos_x;
	FunctionsMap["get_pos_y"] = &CodeExecutor::get_pos_y;
	FunctionsMap["direction_to_point"] = &CodeExecutor::direction_to_point;
	FunctionsMap["direction_beetwen_point"] = &CodeExecutor::direction_beetwen_point;
	FunctionsMap["direction_to_instance"] = &CodeExecutor::direction_to_instance;
	FunctionsMap["sound_play"] = &CodeExecutor::sound_play;
	FunctionsMap["music_play"] = &CodeExecutor::music_play;

}

bool CodeExecutor::LoadArtLib()
{
	MapFunctions();
	char* buffer = Func::GetFileBuf("files/AScript.lib", nullptr);
	if (buffer == NULL) {
		return false;
	}
	std::string data = std::string(buffer);
	std::vector<std::string> lines = Func::Explode(data, '\n');
	
	if (lines.size() == 0) {
		return false;
	}

	FunctionsList.clear();
	FunctionsList.reserve(FunctionsMap.size());

	for (std::string& line : lines) {
		if (line.starts_with("//")) continue;
		//point new_point(float x,float y)
		
		// tokenize
		int phase = 0;
		std::string tmp = "";
		for (int i = 0; i < line.length(); i++) {
			switch (phase) {
			case 0:
				if (line[i] == ' ') {
					phase++;
					//ArtLib.back()._return = tmp;
					tmp = "";
				}
				else {
					tmp += line[i];
				}
				break;
			case 1:
				if (line[i] == '(') {
					phase++;
					//ArtLib.back()._name = tmp;  FunctionsList

					if (FunctionsMap.find(tmp) != FunctionsMap.end()) {
						FunctionsList.push_back(FunctionsMap[tmp]);
						FunctionsMap.erase(tmp);
					}
					else {
						FunctionsList.push_back(nullptr);
						Debug::WARNING("function '"+ tmp+ "' not found");
					}

					tmp = "";
					break;
				}
				else {
					tmp += line[i];
				}
				break;
			}
		}
	}
	if (FunctionsMap.size() > 0) {
		for (auto& f : FunctionsMap) {
			Debug::LOG("Function: '" + f.first + "' not found");
		}
	}
	FunctionsList.shrink_to_fit();
	// free memory
	FunctionsMap.clear();
	return true;
}

#define command ArtCode::Command
#define err(x,y) Debug::ERROR("expected " + x + " but " + std::to_string(y) + " is given"); return false;



bool CodeExecutor::LoadObjectDefinitions()
{
	//CodeExecutor::ArtCode ac;
	Sint64 c = 0;
	const unsigned char* _code = Func::GetFileBytes("object_compile.acp", &c);
	if (_code == nullptr) return false;
	Inspector code(_code, c);

	// definicje objektów
	while (!code.IsEnd()) {
		// first is OBJECT_DEFINITION command
		if (code.GetNextCommand() != command::OBJECT_DEFINITION) {
			// last is end
			if (code.GetCurrentCommand() == command::END && code.IsEnd()) {
				break;
			}
			Debug::ERROR("expected 'OBJECT_DEFINITION' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
		}

		InstanceDefinition instance;
		// seccond is string object name
		std::string o_name = code.GetString();
		instance._name = o_name;

		// varibles
		while (code.GetNextCommand() != command::END || code.IsEnd()) {
			/*
			 WriteCommand(Command::LOCAL_VARIBLE_DEFINITION);
			 WriteBit(getVaribleIndex(local.Type));
			 WriteBit(local.index);
			 WriteString(local.Name);
			 WriteBit(local.ReadOnly);
			* */
			if (code.GetCurrentCommand() == command::LOCAL_VARIBLE_DEFINITION) {
				int varible_type = code.GetBit();
				// not used now, maby later for debug
				int varible_index = code.GetBit();
				std::string varible_name = code.GetString();
				bool varible_read_only = (bool)code.GetBit();
				//
				instance.AddVarible(varible_type);
			}
			else {
				
				Debug::ERROR("instance: '"+ o_name+ "' - expected 'LOCAL_VARIBLE_DEFINITION' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
			}
		} // varibles

		// events
		while (code.GetNextCommand() != command::END || code.IsEnd()) {
			if (code.GetCurrentCommand() == command::FUNCTION_DEFINITION) {
				std::string e_name = code.GetString();
				int f_size = code.GetBit();
				const unsigned char* f_code = code.GetChunk(f_size);
				if ((ArtCode::Command)f_code[f_size - 1] != command::END) {

					Debug::ERROR("instance: '" + o_name + "' - expected 'END' but " + std::to_string(f_code[f_size - 1]) + " is given"); SDL_assert(false); return false;
				}
				
				//instance._events
				if (Event_fromString(e_name) == Event::Invalid) {
					Debug::ERROR("instance: '" + o_name + "' - wrong event '" + e_name + "'"); SDL_assert(false); return false;
				}

				instance._events.push_back(InstanceDefinition::EventData{ Event_fromString(e_name), f_size, f_code });
			}
			else {
				Debug::ERROR("instance: '" + o_name + "' - expected 'FUNCTION_DEFINITION' but " + std::to_string(code.Current()) + " is given"); SDL_assert(false); return false;
			}
		} //ebvents

		// expected end
		if (code.GetCurrentCommand() != command::END) {
			
			Debug::ERROR("expected 'END' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
		}

		// sort events
		std::sort(instance._events.begin(), instance._events.end());

		// all ok, populate
		InstanceDefinitions.push_back(instance);
	}

	return (InstanceDefinitions.size() > 0);
}

Instance* CodeExecutor::SpawnInstance(std::string name)
{
	int id = -1;
	for (auto& ins : InstanceDefinitions) {
		id++;
		if (ins._name == name) {
			return SpawnInstance(id);
		}
	}
	return nullptr;
}

Instance* CodeExecutor::SpawnInstance(int id)
{
	Instance* instance = new Instance(id);
	instance->Name = InstanceDefinitions[id]._name;
	for (auto& var : InstanceDefinitions[id]._varibles) {
		// inicialize all varibles
		instance->Varibles[var.first].insert(instance->Varibles[var.first].end(), var.second, "nul");//Inserting "nul",  var.second times to the vector
	}
	ExecuteScript(instance, Event::DEF_VALUES);
	return instance;
}

void CodeExecutor::ExecuteScript(Instance* instance, Event script)
{
	_break = false;
	if (instance == nullptr) return;
	if (script == Event::Invalid) return;
	CodeExecutor::InstanceDefinition::EventData* code_data = CodeExecutor::GetEventData(instance->GetInstanceDefinitionId(), script);
	// no error becouse GetEventData print error
	if (code_data == nullptr) return;
	GlobalStack.Erase();
	Inspector code(code_data->data, code_data->size);
	h_execute_script(&code, instance);
}

void CodeExecutor::Break()
{
	_break = true;
	ASSERT(false);
}

void CodeExecutor::h_execute_script(Inspector* code, Instance* instance)
{
	if (_break) return;
	while (!code->IsEnd()) {
		if (_break) return;
		switch (code->GetNextCommand()) {
		case command::SET: {
			// varible - type,index
			// varible to set
			ArtCode::varible_type type = (ArtCode::varible_type)code->GetBit();
			ASSERT(type != ArtCode::varible_type::Invalid)
				int index = (int)code->GetBit();
			h_get_value(code, instance);
			instance->Varibles[type][index] = GlobalStack.Get();
		}break;

		case command::FUNCTION: {
			h_execute_function(code, instance);
		} break;

		case command::IF_TEST: {
			code->Skip(h_if_test(code, instance));
		} break;

		}
	}
}

int CodeExecutor::h_if_test(Inspector* code, Instance* instance) {
	if (_break) return 0;
	bool have_operator = false;
	int operator_index = -1;
	// get value to compare
	while (!code->IsEnd()) {
		switch (code->GetNextCommand()) {
		case command::LOCAL_VARIBLE:
		{
			h_get_local_value(code, instance);
		}break;

		case command::FUNCTION:
		{
			h_execute_function(code, instance);
			// add on stack return value
		}break;

		case command::VALUE:
		{
			if (have_operator) {
				// in thic sace return to previous command
				code->Skip(-1);
				h_get_value(code, instance);
			}
			else {
				Break();
			}
		}break;

		case command::OPERATOR:
		{
			int index = (int)code->GetBit();
			if (index > 7) {
				Break();
			}
			operator_index = index;
			have_operator = true;
		}break;

		case command::IF_BODY: {
			int skip = (int)code->GetBit();
			if (have_operator) {
				GlobalStack.Add(std::to_string(operator_index));
				if (h_compare(code, instance)) {
					return 0;
				}
				else {
					return skip;
				}
			}
			else {
				bool test = Func::Str2Bool(GlobalStack.Get());
				if (test) {
					return 0;
				}
				else {
					return skip;
				}
			}
		}break;

		}
	}
	// coœ posz³o nie tak, lepiej olaæ skrypt
	Break();
	return 0;
}

bool CodeExecutor::h_compare(Inspector*, Instance*)
{
	//	0		1		2		3		4		5		6		7
	//	"||",	"&&",	"<<",	">>",	">=",	"<=",	"!=",	"=="
	int oper = std::stoi(GlobalStack.Get());
	switch (oper) {
	case 0://	"||",
	{
		bool com2 = Func::Str2Bool(GlobalStack.Get());
		bool com1 = Func::Str2Bool(GlobalStack.Get());
		return (com2 || com1);
	}
		break;
	case 1://	"&&",
	{
		bool com2 = Func::Str2Bool(GlobalStack.Get());
		bool com1 = Func::Str2Bool(GlobalStack.Get());
		return (com2 && com1);
	}
		break;
	case 2://	"<<",
	{
		float com2 = std::stof(GlobalStack.Get());
		float com1 = std::stof(GlobalStack.Get());
		return (com1 < com2);
	}
		break;
	case 3://	">>"
	{
		float com2 = std::stof(GlobalStack.Get());
		float com1 = std::stof(GlobalStack.Get());
		return (com1 > com2);
	}
		break;
	case 4://	">=",
	{
		float com2 = std::stof(GlobalStack.Get());
		float com1 = std::stof(GlobalStack.Get());
		return (com1 >= com2);
	}
		break;
	case 5://	"<=",
	{
		float com2 = std::stof(GlobalStack.Get());
		float com1 = std::stof(GlobalStack.Get());
		return (com1 <= com2);
	}
		break;
	case 6://	"!=",
	{
		std::string com2 = GlobalStack.Get();
		std::string com1 = GlobalStack.Get();
		return (com1 != com2);
	}
		break;
	case 7://	"=="
	{
		std::string com2 = GlobalStack.Get();
		std::string com1 = GlobalStack.Get();
		return (com1 == com2);
	}
		break;
	default://	
		Break();
		return false;
		break;
	}
}

void CodeExecutor::h_get_local_value(Inspector* code, Instance* instance)
{
	ArtCode::varible_type type = (ArtCode::varible_type)code->GetBit();
	ASSERT(type != ArtCode::varible_type::Invalid);
	int index = (int)code->GetBit();
	GlobalStack.Add(instance->Varibles[type][index]);
}

void CodeExecutor::h_get_value(Inspector* code, Instance* instance) {
	if (_break) return;
	switch (code->GetNextCommand()) {
	case command::FUNCTION:
		h_execute_function(code, instance);
		break;
	case command::LOCAL_VARIBLE:
		h_get_local_value(code, instance);
		break;
	case command::VALUE: {
		int type = (int)code->GetBit(); // ignore for now
		GlobalStack.Add(code->GetString());
	}
		break;
	case command::NULL_VALUE:
		GlobalStack.Add("nul");
		break;
	default:

		break;
	}

}


void CodeExecutor::h_execute_function(Inspector* code, Instance* instance)
{
	if (_break) return;
	while (!code->IsEnd()) {
		if (_break) return;
		int function_index = (int)code->GetBit();
		int args = (int)code->GetBit();
		while (args-- > 0) {
			h_get_value(code, instance);
		}
		FunctionsList[function_index](instance);
		break;
	}

}


#undef command
CodeExecutor::InstanceDefinition::EventData* CodeExecutor::GetEventData(int _id, Event _Event)
{
	for (auto& e : InstanceDefinitions[_id]._events) {

		if (_Event == e.event) {
			return &e;
		}
		else {
			/* TODO: cos z tym jest nie tak
			if (_Event > e.event) {
				if (_Event == Event::DEF_VALUES) return nullptr;
				Debug::WARNING("Event '" + Event_toString(_Event) + "' not found for instance '" + InstanceDefinitions[_id]._name + "'");
				InstanceDefinitions[_id]._events.push_back({ _Event, 0, nullptr });
				// sort events
				std::sort(InstanceDefinitions[_id]._events.begin(), InstanceDefinitions[_id]._events.end());
				return nullptr;
			}
			*/
		}
		
	}
	return nullptr;
}
