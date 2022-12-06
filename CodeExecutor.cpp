#include "CodeExecutor.h"
#include "Func.h"
#include "Debug.h"
#include "Convert.h"
#include <algorithm>
#include "Core.h"
#include "main.h"

/*
	static AStack<int> GlobalStack_int;
	static AStack<float> GlobalStack_float;
	static AStack<bool> GlobalStack_bool;
	static AStack<Instance*> GlobalStack_instance;
	static AStack<SDL_FPoint> GlobalStack_SDL_FPoint;
	static AStack<SDL_Color> GlobalStack_SDL_Color;
	static AStack<std::string> GlobalStack_string;
*/

AStack<int> CodeExecutor::GlobalStack_int = AStack<int>();
AStack<float> CodeExecutor::GlobalStack_float = AStack<float>();
AStack<bool> CodeExecutor::GlobalStack_bool = AStack<bool>();
AStack<Instance*> CodeExecutor::GlobalStack_instance = AStack<Instance*>();
AStack<SDL_FPoint> CodeExecutor::GlobalStack_point = AStack<SDL_FPoint>();
AStack<Rect> CodeExecutor::GlobalStack_rect = AStack<Rect>();
AStack<SDL_Color> CodeExecutor::GlobalStack_color = AStack<SDL_Color>();
AStack<std::string> CodeExecutor::GlobalStack_string = AStack<std::string>();
bool CodeExecutor::_break;
CodeExecutor::CodeExecutor()
{
	FunctionsMap = std::map<std::string, void(*)(Instance*)>();
	FunctionsList = std::vector<void(*)(Instance*)>() ;
	InstanceDefinitions = std::vector<InstanceDefinition>();
	_break = false;
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
		if (line.substr(0,2) == "//") continue;
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
	
#ifndef _DEBUG
	Inspector code(_code, c);
#else
	Inspector code(_code, c, "OBJECT_DEFINITION");
#endif
	// first byte
	unsigned char header[8]{ '\0','\0','\0','\0','\0','\0','\0','\0' };
	for (int i = 0; i < 8; i++) {
		header[i] = code.GetBit();
	}

	if (header[0] != 'A' || header[1] != 'C') {
		Debug::ERROR("can not read object data, wrong header");
		return false;
	}

	{
		int ac_main_version = (int)header[2];
		int ac_minor_vesrion = (int)header[3];
		if (ac_main_version < MINIMUM_ACOMPILLER_MAIN && ac_minor_vesrion < MINIMUM_ACOMPILLER_MINOR) {
			Debug::ERROR("Scripts are created in ACompiller " + std::to_string(ac_main_version) + '.' + std::to_string(ac_minor_vesrion) +
				"! Application requires minimum: " + std::to_string(MINIMUM_ACOMPILLER_MAIN) + '.' + std::to_string(MINIMUM_ACOMPILLER_MINOR) + "\nUpdate game files");
			return false;
		}
		//MINIMUM_ACOMPILLER_MAIN
	}


	// object definitions
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
		instance._template = new Instance((int)InstanceDefinitions.size());

		// variables
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
#ifdef _DEBUG
				instance.AddVarible(varible_type, varible_name);
#else
				instance.AddVarible(varible_type);
#endif
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

		// give name if every is ok
		instance._template->Name = o_name;
		// if have at leat one event sort and add to definitions
		if (instance._events.size() > 0) {
			std::sort(instance._events.begin(), instance._events.end());
			InstanceDefinitions.push_back(instance);

			for (auto& event : instance._events) {
				instance._template->EventFlag = (instance._template->EventFlag | EventBitFromEvent(event.event));
			}

			ExecuteScript(instance._template, Event::DEF_VALUES);
		}
	}
	return (InstanceDefinitions.size() > 0);
}

void CodeExecutor::Delete()
{
	for (auto& def : InstanceDefinitions) {
		delete def._template;
	}
	InstanceDefinitions.clear();
}

int CodeExecutor::GetGlobalStackSize()
{
	int size = 0;
	size += GlobalStack_int.Size();
	size += GlobalStack_float.Size();
	size += GlobalStack_bool.Size();
	size += GlobalStack_instance.Size();
	size += GlobalStack_point.Size();
	size += GlobalStack_rect.Size();
	size += GlobalStack_color.Size();
	size += GlobalStack_string.Size();
	return size;
}
int CodeExecutor::GetGlobalStackCapacity()
{
	int size = 0;
	size += GlobalStack_int.Capacity();
	size += GlobalStack_float.Capacity();
	size += GlobalStack_bool.Capacity();
	size += GlobalStack_instance.Capacity();
	size += GlobalStack_point.Capacity();
	size += GlobalStack_rect.Capacity();
	size += GlobalStack_color.Capacity();
	size += GlobalStack_string.Capacity();
	return size;
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
	Instance* instance = new Instance(*InstanceDefinitions[id]._template);
	return instance->GiveId();
}

void CodeExecutor::ExecuteScript(Instance* instance, Event script)
{
	_break = false;
	if (instance == nullptr) return;
	if (script == Event::Invalid) return;
	CodeExecutor::InstanceDefinition::EventData* code_data = CodeExecutor::GetEventData(instance->GetInstanceDefinitionId(), script);
	// no error becouse GetEventData print error
	if (code_data == nullptr) return;
	
#ifndef _DEBUG
	Inspector code(code_data->data, code_data->size);
#else
	Inspector code(code_data->data, code_data->size, Event_toString(script));
#endif
	h_execute_script(&code, instance);
	if (GetGlobalStackSize() > 100){
		__debugbreak();
		int a = GetGlobalStackSize();
	}
}

void CodeExecutor::EraseGlobalStack()
{
	/*
	GlobalStack_int.Erase();
	GlobalStack_float.Erase();
	GlobalStack_point.Erase();
	GlobalStack_string.Erase();
	GlobalStack_struct.Erase();
	*/
}

void CodeExecutor::Break()
{
	_break = true;
#ifdef _DEBUG
	#ifndef DEBUG_EDITOR
	//ASSERT(false, "CodeExecutor::Break");
	#endif // !DEBUG_EDYTOR
#endif // _DEBUG

}

void CodeExecutor::h_execute_script(Inspector* code, Instance* instance)
{
	/*
	*						WriteCommand(Command::OTHER);
							WriteBit(ref->CodeId);
							WriteValue(var->Type, var->index);
	* */
	if (_break) return;
	while (!code->IsEnd()) {
		if (_break) return;
		switch (code->GetNextCommand()) {
		case command::SET: {
			int operation = (int)code->GetBit();
			// varible - type,index
			// varible to set
			ArtCode::varible_type type = (ArtCode::varible_type)code->GetBit();
			ASSERT(type != ArtCode::varible_type::Invalid, "[command::SET] type = varible_type::Invalid")
				int index = (int)code->GetBit();

			h_get_value(code, instance);
			switch (type) {
			case ArtCode::varible_type::INT:		instance->Varibles_int[index] =		h_operation_int(operation, instance->Varibles_int[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::FLOAT:		instance->Varibles_float[index] =	h_operation_float(operation, instance->Varibles_float[index], GlobalStack_float.Get()); break;
			case ArtCode::varible_type::BOOL:		instance->Varibles_bool[index] =	h_operation_bool(operation, instance->Varibles_bool[index], GlobalStack_bool.Get()); break;
			case ArtCode::varible_type::INSTANCE:	instance->Varibles_instance[index] =h_operation_instance(operation, instance->Varibles_instance[index], GlobalStack_instance.Get()); break;
			case ArtCode::varible_type::OBJECT:		instance->Varibles_object[index] =	h_operation_object(operation, instance->Varibles_object[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::SPRITE:		instance->Varibles_sprite[index] =	h_operation_sprite(operation, instance->Varibles_sprite[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::TEXTURE:	instance->Varibles_texture[index] =	h_operation_texture(operation, instance->Varibles_texture[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::SOUND:		instance->Varibles_sound[index] =	h_operation_sound(operation, instance->Varibles_sound[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::MUSIC:		instance->Varibles_music[index] =	h_operation_music(operation, instance->Varibles_music[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::FONT:		instance->Varibles_font[index] =	h_operation_font(operation, instance->Varibles_font[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::POINT:		instance->Varibles_point[index] =	h_operation_point(operation, instance->Varibles_point[index], GlobalStack_point.Get()); break;
			case ArtCode::varible_type::RECT:		instance->Varibles_rect[index] =	h_operation_rect(operation, instance->Varibles_rect[index], GlobalStack_rect.Get()); break;
			case ArtCode::varible_type::COLOR:		instance->Varibles_color[index] =	h_operation_color(operation, instance->Varibles_color[index], GlobalStack_color.Get()); break;
			case ArtCode::varible_type::STRING:		instance->Varibles_string[index] =	h_operation_string(operation, instance->Varibles_string[index],	GlobalStack_string.Get()); break;
			}
			//instance->Varibles[type][index] = h_operation(operation, instance->Varibles[type][index], GlobalStack.Get());
			//instance->Varibles[type][index] = GlobalStack.Get();
		}break;

		case command::OTHER: {
			int instance_type = (int)code->GetBit();
			Instance* other = Core::GetInstance()->_current_scene->CurrentCollisionInstance;
			if (other == nullptr) {
				Break();
				return;
				// error - other is null
			}
			if (instance_type != other->GetInstanceDefinitionId()) {
				Break();
				// error - wrong type
			}
			ArtCode::varible_type type = (ArtCode::varible_type)code->GetBit();
			ASSERT(type != ArtCode::varible_type::Invalid, "command::SET")
			int index = (int)code->GetBit();
			// operator 
			int operation = (int)code->GetBit();
			
			h_get_value(code, instance);
			//other->Varibles[varible_type][varible_index] = h_operation(operation, other->Varibles[varible_type][varible_index], GlobalStack.Get());
			switch (type) {
			case ArtCode::varible_type::INT:		other->Varibles_int[index] = h_operation_int(operation, other->Varibles_int[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::FLOAT:		other->Varibles_float[index] = h_operation_float(operation, other->Varibles_float[index], GlobalStack_float.Get()); break;
			case ArtCode::varible_type::BOOL:		other->Varibles_bool[index] = h_operation_bool(operation, other->Varibles_bool[index], GlobalStack_bool.Get()); break;
			case ArtCode::varible_type::INSTANCE:	other->Varibles_instance[index] = h_operation_instance(operation, other->Varibles_instance[index], GlobalStack_instance.Get()); break;
			case ArtCode::varible_type::OBJECT:		other->Varibles_object[index] = h_operation_object(operation, other->Varibles_object[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::SPRITE:		other->Varibles_sprite[index] = h_operation_sprite(operation, other->Varibles_sprite[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::TEXTURE:	other->Varibles_texture[index] = h_operation_texture(operation, other->Varibles_texture[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::SOUND:		other->Varibles_sound[index] = h_operation_sound(operation, other->Varibles_sound[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::MUSIC:		other->Varibles_music[index] = h_operation_music(operation, other->Varibles_music[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::FONT:		other->Varibles_font[index] = h_operation_font(operation, other->Varibles_font[index], GlobalStack_int.Get()); break;
			case ArtCode::varible_type::POINT:		other->Varibles_point[index] = h_operation_point(operation, other->Varibles_point[index], GlobalStack_point.Get()); break;
			case ArtCode::varible_type::RECT:		other->Varibles_rect[index] = h_operation_rect(operation, other->Varibles_rect[index], GlobalStack_rect.Get()); break;
			case ArtCode::varible_type::COLOR:		other->Varibles_color[index] = h_operation_color(operation, other->Varibles_color[index], GlobalStack_color.Get()); break;
			case ArtCode::varible_type::STRING:		other->Varibles_string[index] = h_operation_string(operation,	other->Varibles_string[index], GlobalStack_string.Get()); break;
			}

		} break;

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

		case command::OTHER: {
			int instance_type = (int)code->GetBit();
			Instance* other = Core::GetInstance()->_current_scene->CurrentCollisionInstance;
			if (other == nullptr) {
				Break();
				// error - other is null
			}
			if (instance_type != other->GetInstanceDefinitionId()) {
				Break();
				// error - wrong type
			}
			h_get_local_value(code, Core::GetInstance()->_current_scene->CurrentCollisionInstance);
		} break;

		case command::FUNCTION:
		{
			h_execute_function(code, instance);
			// add on stack return value
		}break;

		case command::VALUE:
		{
			if (have_operator) {
				// in this cace return to previous command
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
			int type = (int)code->GetBit();
			int skip = (int)code->GetBit();
			if (have_operator) {
				//GlobalStack_int.Add(operator_index);
				if (h_compare(type, operator_index)) {
					return 0;
				}
				else {
					return skip;
				}
			}
			else {
				bool test = GlobalStack_bool.Get();
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
/*
case 0: // "+=",
case 1: //" -=",
case 2: // "*=",
case 3: // "/=",
case 4: // ":="
*/
int CodeExecutor::h_operation_int(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
		return val1 += val2;
			break;
	case 1: //" -=",
		return val1 -= val2;
			break;
	case 2: // "*=",
		return val1 *= val2;
			break;
	case 3: // "/=",
		return val1 /= val2;
			break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
float CodeExecutor::h_operation_float(int _operator, float val1, float val2) {
	switch(_operator){

	case 0: // "+=",
		return val1 += val2;
		break;
	case 1: //" -=",
		return val1 -= val2;
		break;
	case 2: // "*=",
		return val1 *= val2;
		break;
	case 3: // "/=",
		return val1 /= val2;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
bool CodeExecutor::h_operation_bool(int _operator, bool val1, bool val2) {
	switch(_operator){
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
Instance* CodeExecutor::h_operation_instance(int _operator, Instance* val1, Instance* val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return nullptr;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_object(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_sprite(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_texture(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_sound(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_music(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
int CodeExecutor::h_operation_font(int _operator, int val1, int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
SDL_FPoint CodeExecutor::h_operation_point(int _operator, SDL_FPoint val1, SDL_FPoint val2) {
	switch (_operator) {

	case 0: // "+=",
		return val1 += val2;
		break;
	case 1: //" -=",
		return val1 -= val2;
		break;
	case 2: // "*=",
		return val1 *= val2;
		break;
	case 3: // "/=",
		return val1 /= val2;
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
Rect CodeExecutor::h_operation_rect(int _operator, Rect val1, Rect val2) {
	switch (_operator) {

	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return Rect();
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
SDL_Color CodeExecutor::h_operation_color(int _operator, SDL_Color val1, SDL_Color val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return SDL_Color();
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
std::string CodeExecutor::h_operation_string(int _operator, std::string val1, std::string val2) {
	switch (_operator) {

	case 0: // "+=",
		return val1 += val2;
		break;
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return std::string();
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}

bool CodeExecutor::h_compare(int type, int oper)
{
	//	0		1		2		3		4		5		6		7
	//	"||",	"&&",	"<<",	">>",	">=",	"<=",	"!=",	"=="
	//int oper = GlobalStack_int.Get();

	switch (oper) {
	case 0://	"||",
	{
		switch (type) {
		//case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() || GlobalStack_float.Get());  break;
		case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() || GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() || GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() || GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() || GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() || GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() || GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 1://	"&&",
	{
		//bool com2 = Convert::Str2Bool(GlobalStack.Get());
		//bool com1 = Convert::Str2Bool(GlobalStack.Get());
		//return (com2 && com1);
		switch (type) {
		//case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() && GlobalStack_float.Get());  break;
		case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() && GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() && GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() && GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() && GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() && GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() && GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 2://	"<<",
	{
		//float com2 = Func::TryGetFloat(GlobalStack.Get());
		//float com1 = Func::TryGetFloat(GlobalStack.Get());
		//return (com1 < com2);
		switch (type) { // values get are opposite
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() > GlobalStack_float.Get());  break;
		//case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() > GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() > GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() < GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() > GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() > GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() > GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 3://	">>"
	{
		//float com2 = Func::TryGetFloat(GlobalStack.Get());
		//float com1 = Func::TryGetFloat(GlobalStack.Get());
		//return (com1 > com2);
		switch (type) {// values get are opposite
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() < GlobalStack_float.Get());  break;
		//case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() < GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() < GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() > GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() < GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() < GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() < GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 4://	">=",
	{
		//float com2 = Func::TryGetFloat(GlobalStack.Get());
		//float com1 = Func::TryGetFloat(GlobalStack.Get());
		//return (com1 >= com2);
		switch (type) {// values get are opposite
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() <= GlobalStack_float.Get());  break;
		//case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() <= GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() <= GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() <= GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() <= GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() <= GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() <= GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 5://	"<=",
	{
		//float com2 = Func::TryGetFloat(GlobalStack.Get());
		//float com1 = Func::TryGetFloat(GlobalStack.Get());
		//return (com1 <= com2);
		switch (type) {
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() >= GlobalStack_float.Get());  break;
		//case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() >= GlobalStack_bool.Get());  break;
		//case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() >= GlobalStack_instance.Get());  break;
		//case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::Sprite:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() >= GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() >= GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() >= GlobalStack_color.Get());  break;
		//case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() >= GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 6://	"!=",
	{
		//std::string com2 = GlobalStack.Get();
		//std::string com1 = GlobalStack.Get();
		//return (com1 != com2);
		switch (type) {
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() != GlobalStack_float.Get());  break;
		case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() != GlobalStack_bool.Get());  break;
		case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() != GlobalStack_instance.Get());  break;
		case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::SPRITE:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() != GlobalStack_point.Get());  break;
		//case ArtCode::varible_type::Rect:		return (GlobalStack_rect.Get() != GlobalStack_rect.Get());  break;
		//case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() != GlobalStack_color.Get());  break;
		case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() != GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	case 7://	"=="
	{
		//std::string com2 = GlobalStack.Get();
		//std::string com1 = GlobalStack.Get();
		//return (com1 == com2);
		switch (type) {
		case ArtCode::varible_type::INT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FLOAT:		return (GlobalStack_float.Get() == GlobalStack_float.Get());  break;
		case ArtCode::varible_type::BOOL:		return (GlobalStack_bool.Get() == GlobalStack_bool.Get());  break;
		case ArtCode::varible_type::INSTANCE:	return (GlobalStack_instance.Get() == GlobalStack_instance.Get());  break;
		case ArtCode::varible_type::OBJECT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::SPRITE:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::TEXTURE:	return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::SOUND:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::MUSIC:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::FONT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::varible_type::POINT:		return (GlobalStack_point.Get() == GlobalStack_point.Get());  break;
		case ArtCode::varible_type::RECT:		return (GlobalStack_rect.Get() == GlobalStack_rect.Get());  break;
		case ArtCode::varible_type::COLOR:		return (GlobalStack_color.Get() == GlobalStack_color.Get());  break;
		case ArtCode::varible_type::STRING:		return (GlobalStack_string.Get() == GlobalStack_string.Get());  break;
		default: return false; break; // wrong operator
		}
	}
		break;
	default://	wrong operator
		Break();
		return false;
		break;
	}
}
/*
std::string CodeExecutor::h_operation(int operation, std::string value1, std::string value2)
{
	// if point
	if (value1.find(':') == std::string::npos && value2.find(':') == std::string::npos) {
		//TODO: maby care about types
		float val1 = Func::TryGetFloat(value1);
		float val2 = Func::TryGetFloat(value2);
		switch (operation) {
		case 0: // "+=",
		{
			val1 += val2;
		}break;
		case 1: //" -=",
		{
			val1 -= val2;
		}break;
		case 2: // "*=",
		{val1 *= val2;

		}break;
		case 3: // "/=",
		{
			val1 /= val2;
		}break;
		case 4: // "="
		{
			val1 = val2;
		}break;
		default:
			Break(); break;
		}
		return std::to_string(val1);
	}
	else {
		SDL_FPoint val1 = Convert::Str2FPoint(value1);
		SDL_FPoint val2 = Convert::Str2FPoint(value2);
		switch (operation) {
		case 0: // "+=",
		{
			val1.x += val2.x;
			val1.y += val2.y;
		}break;
		case 1: //" -=",
		{
			val1.x -= val2.x;
			val1.y -= val2.y;
		}break;
		case 2: // "*=",
		{
			val1.x *= val2.x;
			val1.y *= val2.y;
		}break;
		case 3: // "/=",
		{
			val1.x /= val2.x;
			val1.y /= val2.y;
		}break;
		case 4: // "="
		{
			val1 = val2;
		}break;
		default:
			Break(); break;
		}
		return Convert::FPoint2String(val1);
	}
}
*/
void CodeExecutor::h_get_local_value(Inspector* code, Instance* instance)
{
	ArtCode::varible_type type = (ArtCode::varible_type)code->GetBit();
	ASSERT(type != ArtCode::varible_type::Invalid, "varible_type::Invalid");
	int index = (int)code->GetBit();
	//GlobalStack.Add(instance->Varibles[type][index]);
	switch (type) {
	case ArtCode::varible_type::INT:		GlobalStack_int.Add(instance->Varibles_int[index]); break;
	case ArtCode::varible_type::FLOAT:		GlobalStack_float.Add(instance->Varibles_float[index]); break;
	case ArtCode::varible_type::BOOL:		GlobalStack_bool.Add(instance->Varibles_bool[index]); break;
	case ArtCode::varible_type::INSTANCE:	GlobalStack_instance.Add(instance->Varibles_instance[index]); break;
	case ArtCode::varible_type::OBJECT:		GlobalStack_int.Add(instance->Varibles_object[index]); break;
	case ArtCode::varible_type::SPRITE:		GlobalStack_int.Add(instance->Varibles_sprite[index]); break;
	case ArtCode::varible_type::TEXTURE:	GlobalStack_int.Add(instance->Varibles_texture[index]); break;
	case ArtCode::varible_type::SOUND:		GlobalStack_int.Add(instance->Varibles_sound[index]); break;
	case ArtCode::varible_type::MUSIC:		GlobalStack_int.Add(instance->Varibles_music[index]); break;
	case ArtCode::varible_type::FONT:		GlobalStack_int.Add(instance->Varibles_font[index]); break;
	case ArtCode::varible_type::POINT:		GlobalStack_point.Add(instance->Varibles_point[index]); break;
	case ArtCode::varible_type::RECT:		GlobalStack_rect.Add(instance->Varibles_rect[index]); break;
	case ArtCode::varible_type::COLOR:		GlobalStack_color.Add(instance->Varibles_color[index]); break;
	case ArtCode::varible_type::STRING:		GlobalStack_string.Add(instance->Varibles_string[index]); break;
	}

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
		//GlobalStack.Add(code->GetString());
		switch (type) {
		case ArtCode::varible_type::INT:		GlobalStack_int.Add(Func::TryGetInt( code->GetString() )); break;
		case ArtCode::varible_type::FLOAT:		GlobalStack_float.Add(Func::TryGetFloat(code->GetString())); break;
		case ArtCode::varible_type::BOOL:		GlobalStack_bool.Add( Convert::Str2Bool( code->GetString()) ); break;
		case ArtCode::varible_type::INSTANCE:	GlobalStack_instance.Add( Core::GetInstance()->_current_scene->GetInstanceById(Func::TryGetInt(code->GetString())) ); break;
		case ArtCode::varible_type::OBJECT:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::SPRITE:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::TEXTURE:	GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::SOUND:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::MUSIC:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::FONT:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::varible_type::POINT:		GlobalStack_point.Add( Convert::Str2FPoint(code->GetString()) ); break;
		case ArtCode::varible_type::RECT:		GlobalStack_rect.Add(Convert::Str2Rect(code->GetString())); break;
		case ArtCode::varible_type::COLOR:		GlobalStack_color.Add(Convert::Str2Color(code->GetString())); break;
		case ArtCode::varible_type::STRING:		GlobalStack_string.Add(code->GetString()); break;
		}
	}
		break;
	case command::NULL_VALUE:
		// do nothing
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
		if (FunctionsList[function_index] != nullptr) {
			FunctionsList[function_index](instance);
		}
		else {
#ifdef _DEBUG
			Debug::WARNING(instance->Name + " script error, function not found, event name:'" + code->DebugGetFName() + "' token pos:" + std::to_string(code->DebugGetCurrentPos()) + " function index: " + std::to_string(function_index));
#endif
		}
		break;
	}

}


#undef command
CodeExecutor::InstanceDefinition::EventData* CodeExecutor::GetEventData(const int id, const Event event)
{
	for (auto& e : InstanceDefinitions[id]._events) {

		if (event == e.event) {
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
