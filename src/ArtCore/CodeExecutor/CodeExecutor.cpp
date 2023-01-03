// ReSharper disable CppUseAuto
#include "CodeExecutor.h"

#include <algorithm>
#include <ranges>

#include "ArtCore/main.h"
#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/Gui/Gui.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/Scene/Scene.h"

AStack<int> CodeExecutor::GlobalStack_int = AStack<int>();
AStack<float> CodeExecutor::GlobalStack_float = AStack<float>();
AStack<bool> CodeExecutor::GlobalStack_bool = AStack<bool>();
AStack<Instance*> CodeExecutor::GlobalStack_instance = AStack<Instance*>();
AStack<SDL_FPoint> CodeExecutor::GlobalStack_point = AStack<SDL_FPoint>();
AStack<Rect> CodeExecutor::GlobalStack_rect = AStack<Rect>();
AStack<SDL_Color> CodeExecutor::GlobalStack_color = AStack<SDL_Color>();
AStack<std::string> CodeExecutor::GlobalStack_string = AStack<std::string>();

bool CodeExecutor::_break;

std::vector<CodeExecutor::SuspendCodeStruct> CodeExecutor::_suspended_code;
bool CodeExecutor::_have_suspended_code;
CodeExecutor::CodeExecutor()
{
	FunctionsMap = std::map<std::string, void(*)(Instance*)>();
	FunctionsList = std::vector<void(*)(Instance*)>() ;
	_instance_definitions = std::vector<InstanceDefinition>();
	_break = false;
	_suspended_code = std::vector<CodeExecutor::SuspendCodeStruct>();
	_have_suspended_code = false;
}

bool CodeExecutor::LoadArtLib()
{
	MapFunctions();
	const char* buffer = Func::GetFileBuf("files/AScript.lib", nullptr);
	if (buffer == nullptr) {
		return false;
	}
	const std::string data(buffer);
	std::vector<std::string> lines = Func::Explode(data, '\n');
	
	if (lines.empty()) {
		return false;
	}

	FunctionsList.clear();
	FunctionsList.reserve(FunctionsMap.size());

	for (std::string& line : lines) {
		if (line.substr(0,2) == "//") continue;
		// tokenize
		int phase = 0;
		std::string tmp;
		for (const char c : line)
		{
			switch (phase) {
			case 0:
				if (c == ' ') {
					phase++;
					tmp = "";
				}
				else {
					tmp += c;
				}
				break;
			case 1:
				if (c == '(') {
					phase++;
					if (FunctionsMap.contains(tmp)) {
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
					tmp += c;
				}
				break;
			}
		}
	}
	if (!FunctionsMap.empty()) {
		for (const auto& key : FunctionsMap | std::views::keys) {
			Debug::LOG("Function: '" + key + "' not found");
		}
	}
	FunctionsList.shrink_to_fit();
	// free memory
	FunctionsMap.clear();
	return true;
}

#define COMMAND ArtCode::Command

// create new inspector, remember to delete it
Inspector* CodeExecutor::CreateInspector(const std::string& code_file) const
{
	Sint64 c = 0;
	const unsigned char* code_buffer = Func::GetFileBytes(code_file, &c);
	if (code_buffer == nullptr) return nullptr;
#ifndef _DEBUG
	Inspector* inspector = new Inspector(code_buffer, c);
#else
	Inspector* inspector = new Inspector(code_buffer, c, Func::GetFileName(code_file));
#endif

	// first byte
	unsigned char header[8]{ '\0','\0','\0','\0','\0','\0','\0','\0' };
	for (int i = 0; i < 8; i++) {  // NOLINT(modernize-loop-convert) <- ;loop not valid because content have null terminated chars
		header[i] = inspector->GetBit();
	}

	if (header[0] != 'A' || header[1] != 'C') {
		Debug::ERROR("can not read object data, wrong header");
		return nullptr;
	}

	const int ac_main_version = (int)header[2];
	const int ac_minor_version = (int)header[3];
	if (ac_main_version < MINIMUM_ART_COMPILER_MAIN && ac_minor_version < MINIMUM_ART_COMPILER_MINOR) {
		Debug::ERROR("Scripts are created in ACompiler " + std::to_string(ac_main_version) + '.' + std::to_string(ac_minor_version) +
			"! Application requires minimum: " + std::to_string(MINIMUM_ART_COMPILER_MAIN) + '.' + std::to_string(MINIMUM_ART_COMPILER_MINOR) + "\nUpdate game files");
		return nullptr;
	}
	//MINIMUM_ART_COMPILER_MAIN
	return inspector;
}

bool CodeExecutor::LoadObjectDefinitions(const BackGroundRenderer* bgr, const int p_min, const int p_max)
{
	const std::unique_ptr<Inspector> code = std::make_unique<Inspector>( *CreateInspector("object_compile.acp"));
	// object definitions
	while (!code->IsEnd()) {
		// first is OBJECT_DEFINITION command
		if (code->GetNextCommand() != COMMAND::OBJECT_DEFINITION) {
			// last is end
			if (code->GetCurrentCommand() == COMMAND::END && code->IsEnd()) {
				break;
			}
			Debug::ERROR("expected 'OBJECT_DEFINITION' but " + std::to_string(code->Current()) + " is given"); SDL_assert(false); return false;
		}

		InstanceDefinition instance;
		// second is string object name
		const std::string o_name = code->GetString();
		instance.Name = o_name;
		instance.Template = new Instance((int)_instance_definitions.size());

		// variables
		while (code->GetNextCommand() != COMMAND::END || code->IsEnd()) {
			/* from ArtCompiler code
			 WriteCommand(Command::LOCAL_VARIABLE_DEFINITION);
			 WriteBit(getVariableIndex(local.Type));
			 WriteString(local.Name);
			* */
			if (code->GetCurrentCommand() == COMMAND::LOCAL_VARIABLE_DEFINITION) {
				const int variable_type = code->GetBit();
				const std::string variable_name = code->GetString();
				instance.AddVariable(variable_type, variable_name);
			}
			else {

				Debug::ERROR("instance: '" + o_name + "' - expected 'LOCAL_VARIABLE_DEFINITION' but " + std::to_string(code->Current()) + " is given"); SDL_assert(false); return false;
			}
		} // variables

		// events
		while (code->GetNextCommand() != COMMAND::END || code->IsEnd()) {
			if (code->GetCurrentCommand() == COMMAND::FUNCTION_DEFINITION) {
				std::string e_name = code->GetString();
				const int f_size = code->GetBit();
				const unsigned char* f_code = code->GetChunk(f_size);
				if ((ArtCode::Command)f_code[f_size - 1] != COMMAND::END) {

					Debug::ERROR("instance: '" + o_name + "' - expected 'END' but " + std::to_string(f_code[f_size - 1]) + " is given"); SDL_assert(false); return false;
				}

				//instance._events
				if (Event_fromString(e_name) == Event::EventInvalid) {
					Debug::ERROR("instance: '" + o_name + "' - wrong event '" + e_name + "'"); SDL_assert(false); return false;
				}

				instance._events.push_back(InstanceDefinition::EventData{ Event_fromString(e_name), f_size, f_code });
			}
			else {
				Debug::ERROR("instance: '" + o_name + "' - expected 'FUNCTION_DEFINITION' but " + std::to_string(code->Current()) + " is given"); SDL_assert(false); return false;
			}
		} //events

		// expected end
		if (code->GetCurrentCommand() != COMMAND::END) {

			Debug::ERROR("expected 'END' but " + std::to_string(code->Current()) + " is given"); SDL_assert(false); return false;
		}

		// give name if every is ok
		instance.Template->Name = o_name;
		// if have at least one event sort and add to definitions
		if (!instance._events.empty()) {
			std::sort(instance._events.begin(), instance._events.end());
			_instance_definitions.push_back(instance);

			for (const InstanceDefinition::EventData& event : instance._events) {
				instance.Template->EventFlag = (instance.Template->EventFlag | EventBitFromEvent(event.event));
			}

			ExecuteScript(instance.Template, Event::DEF_VALUES);
		}

		bgr->SetProgress(
			static_cast<int>(Func::LinearScale(
				static_cast<float>(code->GetPosition()),
				0.0f,
				static_cast<float>(code->GetLength()),
				static_cast<float>(p_min),
				static_cast<float>(p_max)
			))
		);

	}
	return (!_instance_definitions.empty());
}

bool CodeExecutor::LoadSceneTriggers()
{
	const std::unique_ptr<Inspector> code = std::make_unique<Inspector>(*CreateInspector("scene/" + Core::GetInstance()->_current_scene->GetName() + "/scene_triggers.acp"));
	// object definitions
	while (!code->IsEnd()) {
		// first is OBJECT_DEFINITION command
		if (code->GetNextCommand() != COMMAND::OBJECT_DEFINITION) {
			// last is end
			if (code->GetCurrentCommand() == COMMAND::END && code->IsEnd()) {
				break;
			}
			Debug::ERROR("expected 'OBJECT_DEFINITION' but " + std::to_string(code->Current()) + " is given");SDL_assert(false); return false;
		}
		const std::string o_name = code->GetString();
		InstanceDefinition instance;
		instance.Template = new Instance(-1);
		instance.Name = o_name;
		instance.Template->Name = o_name;
		// variables
		while (code->GetNextCommand() != COMMAND::END || code->IsEnd()) {
			/* from ArtCompiler code
			 WriteCommand(Command::LOCAL_VARIABLE_DEFINITION);
			 WriteBit(getVariableIndex(local.Type));
			 WriteString(local.Name);
			* */
			if (code->GetCurrentCommand() == COMMAND::LOCAL_VARIABLE_DEFINITION) {
				const int variable_type = code->GetBit();
				instance.AddVariable(variable_type, code->GetString());
			}
			else {
				Debug::ERROR("LoadSceneTriggers: '"+ o_name+ "' - expected 'LOCAL_VARIABLE_DEFINITION' but " + std::to_string(code->Current()) + " is given");
				ASSERT(false, "x01"); return false;
			}
		} // variables

		// events
		while (code->GetNextCommand() != COMMAND::END || code->IsEnd()) {
			if (code->GetCurrentCommand() != COMMAND::FUNCTION_DEFINITION) {
				Debug::WARNING("LoadSceneTriggers: '" + o_name + "' - expected 'FUNCTION_DEFINITION' but " + std::to_string(code->Current()) + " is given");
				ASSERT(false, "x02"); return false;
			}
			else {
				std::string e_name = code->GetString();
				const int f_size = code->GetBit();
				const unsigned char* f_code = code->GetChunk(f_size);
				if ((ArtCode::Command)f_code[f_size - 1] != COMMAND::END) {

					Debug::ERROR("LoadSceneTriggers: '" + o_name + "' - expected 'END' but " + std::to_string(f_code[f_size - 1]) + " is given");
					ASSERT(false, "x03"); return false;
				}
				// scene accept only def values event, every other is trigger
				if (Event_fromString(e_name) == Event::DEF_VALUES) {
					std::pair def_values_code = { f_code, (Sint64)f_size };
					ExecuteCode(instance.Template, &def_values_code);
				}else
				{
					if(const std::vector<std::string> trigger_type = Func::Split(e_name, '&'); trigger_type.size() != 2)
					{
						//error
						Debug::WARNING("LoadSceneTriggers: '" + o_name + "' - expected 'trigger_type.size() == 2' but " + std::to_string(trigger_type.size()) + " is given");
						ASSERT(false, "x04"); return false;
					}else
					{
						if(trigger_type[0] == "scene")
						{
							// triggers
							Core::GetInstance()->_current_scene->SetTriggerData(e_name, f_code, f_size);
						}else
						{
							// gui element action
							if (const std::vector<std::string> gui_element_type = Func::Split(trigger_type[0], '#'); gui_element_type.size() != 2)
							{//error
								Debug::WARNING("LoadSceneTriggers: '" + o_name + "' - expected 'gui_element_type.size() == 2' but " + std::to_string(gui_element_type.size()) + " is given");
								ASSERT(false, "x05"); return false;
							}else
							{
								Gui::GuiElementTemplate* element = Core::GetInstance()->_current_scene->GuiSystem.GetElementById(gui_element_type[1]);
								if (element != nullptr)
								{
									element->SetCallback(Gui::GuiElementTemplate::EvCallback_fromString(trigger_type[1]),
									                     std::pair<const unsigned char*, Uint64>(f_code, f_size)
									);
								}else
								{//error
									Debug::WARNING("LoadSceneTriggers: '" + o_name + "' - 'GuiSystem.GetElementById("+ gui_element_type[0]+"' == nullptr");
									ASSERT(false, "x06"); return false;
								}
							}
						}
					}
				}
			}
		} //events

		// expected end
		if (code->GetCurrentCommand() != COMMAND::END) {
			
			Debug::WARNING("LoadSceneTriggers 'END' but " + std::to_string(code->Current()) + " is given");
			ASSERT(false, "x07"); return false;
		}

		// execute only starting event
		Core::GetInstance()->_current_scene->SetVariableHolder( new Instance(*instance.Template) );
	}
	return true;
}

void CodeExecutor::Delete()
{
	for (const auto& def : _instance_definitions) {
		delete def.Template;
	}
	_instance_definitions.clear();
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


Instance* CodeExecutor::SpawnInstance(const std::string& name) const
{
	int id = -1;
	for (auto& ins : _instance_definitions) {
		id++;
		if (ins.Name == name) {
			return SpawnInstance(id);
		}
	}
	return nullptr;
}

Instance* CodeExecutor::SpawnInstance(const int id) const
{
	Instance* instance = new Instance(*_instance_definitions[id].Template);
	return instance->GiveId();
}

void CodeExecutor::ExecuteCode(Instance* instance, std::pair<const unsigned char*, Sint64>* code_data)
{
	if (code_data == nullptr) return;

#ifndef _DEBUG
	Inspector code(code_data->first, code_data->second);
#else
	Inspector code(code_data->first, code_data->second, "Execute script");
#endif
	h_execute_script(&code, instance);
}

void CodeExecutor::ExecuteScript(Instance* instance, const Event script)
{
	_break = false;
	if (instance == nullptr) return;
	if (script == Event::EventInvalid) return;
	CodeExecutor::InstanceDefinition::EventData* code_data = CodeExecutor::GetEventData(instance->GetInstanceDefinitionId(), script);
	// no error because GetEventData print error
	if (code_data == nullptr) return;
	
#ifndef _DEBUG
	Inspector code(code_data->data, code_data->size);
#else
	Inspector code(code_data->data, code_data->size, Event_toString(script));
#endif
	h_execute_script(&code, instance);
}



void CodeExecutor::EraseGlobalStack()
{
	GlobalStack_int.Erase();
	GlobalStack_float.Erase();
	GlobalStack_bool.Erase();
	GlobalStack_instance.Erase();
	GlobalStack_point.Erase();
	GlobalStack_rect.Erase();
	GlobalStack_color.Erase(); 
	GlobalStack_string.Erase();
}


void CodeExecutor::Break()
{
	_break = true;
}

void CodeExecutor::SuspendedCodeStop()
{
	_suspended_code.clear();
	_have_suspended_code = false;
}

void CodeExecutor::SuspendedCodeAdd(const double time, const Inspector* code_data, Instance* sender)
{
	if (sender->SuspendedCodeState(true)) {
		_suspended_code.emplace_back(time, code_data, sender);
	}
}

void CodeExecutor::SuspendedCodeExecute()
{
	if (!_have_suspended_code) return;

	for (auto it = _suspended_code.begin(); it != _suspended_code.end(); )
	{
		(*it).Time -= 1000.0 * Core::GetInstance()->DeltaTime;
		if((*it).Time <= 0.0)
		{
			if ((*it).Sender->SuspendedCodeState(false)) {
				_break = false;
				Core::GetInstance()->Executor->h_execute_script(&(*it).CodeData, (*it).Sender);
			}
			it = _suspended_code.erase(it);
		}
		else {
			++it;
		}
	}
}

void CodeExecutor::SuspendedCodeDeleteInstance(const Instance* sender)
{
	if (!_have_suspended_code) return;
	for (auto it = _suspended_code.begin(); it != _suspended_code.end(); )
	{
		if((*it).Sender->GetId() == sender->GetId())
		{
			it = _suspended_code.erase(it);
		}
		else {
			++it;
		}
	}
}

void CodeExecutor::h_execute_script(Inspector* code, Instance* instance)
{
	/*
	*						WriteCommand(Command::OTHER);
							WriteBit(ref->CodeId);
							WriteValue(var->Type, var->index);
	* */
	_current_inspector = code;
	if (_break) return;
	while (!code->IsEnd()) {
		if (_break) return;
		switch (code->GetNextCommand()) {
		case COMMAND::SET: {
			const int operation = (int)code->GetBit();
			// variable - type,index
			// variable to set
			const ArtCode::variable_type type = (ArtCode::variable_type)code->GetBit();
			ASSERT(type != ArtCode::variable_type::variable_typeInvalid, "[command::SET] type = variable_type::Invalid")
			const int index = (int)code->GetBit();

			h_get_value(code, instance);
			switch (type) {
			case ArtCode::variable_type::INT:		instance->Variables_int[index] =	h_operation_int(operation, instance->Variables_int[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::FLOAT:		instance->Variables_float[index] =	h_operation_float(operation, instance->Variables_float[index], GlobalStack_float.Get()); break;
			case ArtCode::variable_type::BOOL:		instance->Variables_bool[index] =	h_operation_bool(operation, instance->Variables_bool[index], GlobalStack_bool.Get()); break;
			case ArtCode::variable_type::INSTANCE:	instance->Variables_instance[index]=h_operation_instance(operation, instance->Variables_instance[index], GlobalStack_instance.Get()); break;
			case ArtCode::variable_type::OBJECT:	instance->Variables_object[index] =	h_operation_object(operation, instance->Variables_object[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::SPRITE:	instance->Variables_sprite[index] =	h_operation_sprite(operation, instance->Variables_sprite[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::TEXTURE:	instance->Variables_texture[index] =h_operation_texture(operation, instance->Variables_texture[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::SOUND:		instance->Variables_sound[index] =	h_operation_sound(operation, instance->Variables_sound[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::MUSIC:		instance->Variables_music[index] =	h_operation_music(operation, instance->Variables_music[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::FONT:		instance->Variables_font[index] =	h_operation_font(operation, instance->Variables_font[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::POINT:		instance->Variables_point[index] =	h_operation_point(operation, instance->Variables_point[index], GlobalStack_point.Get()); break;
			case ArtCode::variable_type::RECT:		instance->Variables_rect[index] =	h_operation_rect(operation, instance->Variables_rect[index], GlobalStack_rect.Get()); break;
			case ArtCode::variable_type::COLOR:		instance->Variables_color[index] =	h_operation_color(operation, instance->Variables_color[index], GlobalStack_color.Get()); break;
			case ArtCode::variable_type::STRING:	instance->Variables_string[index] =	h_operation_string(operation, instance->Variables_string[index],	GlobalStack_string.Get()); break;
			}
			//instance->variables[type][index] = h_operation(operation, instance->variables[type][index], GlobalStack.Get());
			//instance->variables[type][index] = GlobalStack.Get();
		}break;

		case COMMAND::OTHER: {
			const int instance_type = (int)code->GetBit();
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
			const ArtCode::variable_type type = (ArtCode::variable_type)code->GetBit();
			ASSERT(type != ArtCode::variable_type::variable_typeInvalid, "command::SET")
			const int index = (int)code->GetBit();
			// operator 
			const int operation = (int)code->GetBit();
			
			h_get_value(code, instance);
			//other->variables[variable_type][variable_index] = h_operation(operation, other->variables[variable_type][variable_index], GlobalStack.Get());
			switch (type) {
			case ArtCode::variable_type::INT:		other->Variables_int[index] = h_operation_int(operation, other->Variables_int[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::FLOAT:		other->Variables_float[index] = h_operation_float(operation, other->Variables_float[index], GlobalStack_float.Get()); break;
			case ArtCode::variable_type::BOOL:		other->Variables_bool[index] = h_operation_bool(operation, other->Variables_bool[index], GlobalStack_bool.Get()); break;
			case ArtCode::variable_type::INSTANCE:	other->Variables_instance[index] = h_operation_instance(operation, other->Variables_instance[index], GlobalStack_instance.Get()); break;
			case ArtCode::variable_type::OBJECT:		other->Variables_object[index] = h_operation_object(operation, other->Variables_object[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::SPRITE:		other->Variables_sprite[index] = h_operation_sprite(operation, other->Variables_sprite[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::TEXTURE:	other->Variables_texture[index] = h_operation_texture(operation, other->Variables_texture[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::SOUND:		other->Variables_sound[index] = h_operation_sound(operation, other->Variables_sound[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::MUSIC:		other->Variables_music[index] = h_operation_music(operation, other->Variables_music[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::FONT:		other->Variables_font[index] = h_operation_font(operation, other->Variables_font[index], GlobalStack_int.Get()); break;
			case ArtCode::variable_type::POINT:		other->Variables_point[index] = h_operation_point(operation, other->Variables_point[index], GlobalStack_point.Get()); break;
			case ArtCode::variable_type::RECT:		other->Variables_rect[index] = h_operation_rect(operation, other->Variables_rect[index], GlobalStack_rect.Get()); break;
			case ArtCode::variable_type::COLOR:		other->Variables_color[index] = h_operation_color(operation, other->Variables_color[index], GlobalStack_color.Get()); break;
			case ArtCode::variable_type::STRING:		other->Variables_string[index] = h_operation_string(operation,	other->Variables_string[index], GlobalStack_string.Get()); break;
			}

		} break;

		case COMMAND::FUNCTION: {
			h_execute_function(code, instance);
		} break;

		case COMMAND::IF_TEST: {
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
		case COMMAND::LOCAL_VARIABLE:
		{
			h_get_local_value(code, instance);
		}break;

		case COMMAND::OTHER: {
			const int instance_type = (int)code->GetBit();
			const Instance* other = Core::GetInstance()->_current_scene->CurrentCollisionInstance;
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

		case COMMAND::FUNCTION:
		{
			h_execute_function(code, instance);
			// add on stack return value
		}break;

		case COMMAND::VALUE:
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

		case COMMAND::OPERATOR:
		{
			const int index = (int)code->GetBit();
			if (index > 7) {
				Break();
			}
			operator_index = index;
			have_operator = true;
		}break;

		case COMMAND::IF_BODY: {
			const int type = (int)code->GetBit();
			const int skip = (int)code->GetBit();
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
				const bool test = GlobalStack_bool.Get();
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
int CodeExecutor::h_operation_int(const int _operator, int val1, const int val2) {
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
float CodeExecutor::h_operation_float(const int _operator, float val1, const float val2) {
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
bool CodeExecutor::h_operation_bool(const int _operator, bool val1, const bool val2) {
	switch(_operator){
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
Instance* CodeExecutor::h_operation_instance(const int _operator, Instance* val1, Instance* val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return nullptr;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_object(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_sprite(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_texture(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_sound(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_music(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
int CodeExecutor::h_operation_font(const int _operator, int val1, const int val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return false;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
SDL_FPoint CodeExecutor::h_operation_point(const int _operator, const SDL_FPoint val1, const SDL_FPoint val2) {
	switch (_operator) {

	case 0: // "+=",
		return val1 += val2;
	case 1: //" -=",
		return val1 -= val2;
	case 2: // "*=",
		return val1 *= val2;
	case 3: // "/=",
		return val1 /= val2;
	case 4: // ":=",
		return val2;
	}
	return val2;
}
Rect CodeExecutor::h_operation_rect(const int _operator, Rect val1, const Rect val2) {
	switch (_operator) {

	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return {};
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
SDL_Color CodeExecutor::h_operation_color(const int _operator, SDL_Color val1, const SDL_Color val2) {
	switch (_operator) {
	case 0: // "+=",
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return {};
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}
std::string CodeExecutor::h_operation_string(const int _operator, std::string val1, std::string val2) {
	switch (_operator) {

	case 0: // "+=",
		return val1 += val2;
		break;
	case 1: //" -=",
	case 2: // "*=",
	case 3: // "/=",
		return {};
		break;
	case 4: // ":=",
		return val2;
		break;
	}
	return val2;
}

bool CodeExecutor::h_compare(const int type, const int operation)
{
	//	0		1		2		3		4		5		6		7
	//	"||",	"&&",	"<<",	">>",	">=",	"<=",	"!=",	"=="
	//int operation = GlobalStack_int.Get();

	switch (operation) {
	case 0://	"||",
	{
		switch (type) {
		//case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() || GlobalStack_float.Get());  break;
		case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() || GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() || GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() || GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() || GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() || GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() || GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() || GlobalStack_string.Get());  break;
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
		//case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() && GlobalStack_float.Get());  break;
		case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() && GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() && GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() && GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() && GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() && GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() && GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() && GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() > GlobalStack_float.Get());  break;
		//case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() > GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() > GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() > GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() > GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() > GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() > GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() > GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() < GlobalStack_float.Get());  break;
		//case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() < GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() < GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() < GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() < GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() < GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() < GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() < GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() <= GlobalStack_float.Get());  break;
		//case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() <= GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() <= GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() <= GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() <= GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() <= GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() <= GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() <= GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() >= GlobalStack_float.Get());  break;
		//case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() >= GlobalStack_bool.Get());  break;
		//case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() >= GlobalStack_instance.Get());  break;
		//case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::Sprite:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		//case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() >= GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() >= GlobalStack_point.Get());  break;
		//case ArtCode::variable_type::Rect:		return (GlobalStack_rect.Get() >= GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() >= GlobalStack_color.Get());  break;
		//case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() >= GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() != GlobalStack_float.Get());  break;
		case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() != GlobalStack_bool.Get());  break;
		case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() != GlobalStack_instance.Get());  break;
		case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::SPRITE:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() != GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() != GlobalStack_point.Get());  break;
		case ArtCode::variable_type::RECT:		return (GlobalStack_rect.Get() != GlobalStack_rect.Get());  break;
		//case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() != GlobalStack_color.Get());  break;
		case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() != GlobalStack_string.Get());  break;
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
		case ArtCode::variable_type::INT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FLOAT:		return (GlobalStack_float.Get() == GlobalStack_float.Get());  break;
		case ArtCode::variable_type::BOOL:		return (GlobalStack_bool.Get() == GlobalStack_bool.Get());  break;
		case ArtCode::variable_type::INSTANCE:	return (GlobalStack_instance.Get() == GlobalStack_instance.Get());  break;
		case ArtCode::variable_type::OBJECT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::SPRITE:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::TEXTURE:	return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::SOUND:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::MUSIC:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::FONT:		return (GlobalStack_int.Get() == GlobalStack_int.Get());  break;
		case ArtCode::variable_type::POINT:		return (GlobalStack_point.Get() == GlobalStack_point.Get());  break;
		case ArtCode::variable_type::RECT:		return (GlobalStack_rect.Get() == GlobalStack_rect.Get());  break;
		case ArtCode::variable_type::COLOR:		return (GlobalStack_color.Get() == GlobalStack_color.Get());  break;
		case ArtCode::variable_type::STRING:		return (GlobalStack_string.Get() == GlobalStack_string.Get());  break;
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

void CodeExecutor::h_get_local_value(Inspector* code, Instance* instance)
{
	const ArtCode::variable_type type = (ArtCode::variable_type)code->GetBit();
	ASSERT(type != ArtCode::variable_type::variable_typeInvalid, "variable_type::Invalid");
	const int index = (int)code->GetBit();
	//GlobalStack.Add(instance->variables[type][index]);
	switch (type) {
	case ArtCode::variable_type::INT:		GlobalStack_int.Add(instance->Variables_int[index]); break;
	case ArtCode::variable_type::FLOAT:		GlobalStack_float.Add(instance->Variables_float[index]); break;
	case ArtCode::variable_type::BOOL:		GlobalStack_bool.Add(instance->Variables_bool[index]); break;
	case ArtCode::variable_type::INSTANCE:	GlobalStack_instance.Add(instance->Variables_instance[index]); break;
	case ArtCode::variable_type::OBJECT:		GlobalStack_int.Add(instance->Variables_object[index]); break;
	case ArtCode::variable_type::SPRITE:		GlobalStack_int.Add(instance->Variables_sprite[index]); break;
	case ArtCode::variable_type::TEXTURE:	GlobalStack_int.Add(instance->Variables_texture[index]); break;
	case ArtCode::variable_type::SOUND:		GlobalStack_int.Add(instance->Variables_sound[index]); break;
	case ArtCode::variable_type::MUSIC:		GlobalStack_int.Add(instance->Variables_music[index]); break;
	case ArtCode::variable_type::FONT:		GlobalStack_int.Add(instance->Variables_font[index]); break;
	case ArtCode::variable_type::POINT:		GlobalStack_point.Add(instance->Variables_point[index]); break;
	case ArtCode::variable_type::RECT:		GlobalStack_rect.Add(instance->Variables_rect[index]); break;
	case ArtCode::variable_type::COLOR:		GlobalStack_color.Add(instance->Variables_color[index]); break;
	case ArtCode::variable_type::STRING:		GlobalStack_string.Add(instance->Variables_string[index]); break;
	}

}

void CodeExecutor::h_get_value(Inspector* code, Instance* instance) {
	if (_break) return;
	switch (code->GetNextCommand()) {
	case COMMAND::FUNCTION:
		h_execute_function(code, instance);
		break;
	case COMMAND::LOCAL_VARIABLE:
		h_get_local_value(code, instance);
		break;
	case COMMAND::VALUE: {
		const int type = (int)code->GetBit(); // ignore for now
		//GlobalStack.Add(code->GetString());
		switch (type) {
		case ArtCode::variable_type::INT:		GlobalStack_int.Add(Func::TryGetInt( code->GetString() )); break;
		case ArtCode::variable_type::FLOAT:		GlobalStack_float.Add(Func::TryGetFloat(code->GetString())); break;
		case ArtCode::variable_type::BOOL:		GlobalStack_bool.Add( Convert::Str2Bool( code->GetString()) ); break;
		case ArtCode::variable_type::INSTANCE:	GlobalStack_instance.Add( Core::GetInstance()->_current_scene->GetInstanceById(Func::TryGetInt(code->GetString())) ); break;
		case ArtCode::variable_type::OBJECT:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::SPRITE:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::TEXTURE:	GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::SOUND:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::MUSIC:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::FONT:		GlobalStack_int.Add(Func::TryGetInt(code->GetString())); break;
		case ArtCode::variable_type::POINT:		GlobalStack_point.Add( Convert::Str2FPoint(code->GetString()) ); break;
		case ArtCode::variable_type::RECT:		GlobalStack_rect.Add(Convert::Str2Rect(code->GetString())); break;
		case ArtCode::variable_type::COLOR:		GlobalStack_color.Add(Convert::Hex2Color(code->GetString())); break;
		case ArtCode::variable_type::STRING:		GlobalStack_string.Add(code->GetString()); break;
		}
	}
		break;
	case COMMAND::NULL_VALUE:
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
		const int function_index = (int)code->GetBit();
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


#undef COMMAND
CodeExecutor::InstanceDefinition::EventData* CodeExecutor::GetEventData(const int id, const Event event)
{
	for (auto& e : _instance_definitions[id]._events) {

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
