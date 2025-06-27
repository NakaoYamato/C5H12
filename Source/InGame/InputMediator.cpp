#include "InputMediator.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

const char* InputMediator::PlayerInputName	= "PlayerInput";
const char* InputMediator::MenuInputName	= "MenuInput";
// ŠJŽnŠÖ”
void InputMediator::OnStart()
{
}
// XV‘Oˆ—
void InputMediator::OnPreUpdate(float elapsedTime)
{
	// Žó‚¯Žæ‚Á‚½ƒRƒ}ƒ“ƒh‚ð‘S‚Äˆ—
	for (const auto& commandData : _commandHistory)
	{
		for (const auto& controller : _inputControllers)
		{
			controller->ReceiveCommandFromOther(commandData.commandType, commandData.command);
		}
	}
	_commandHistory.clear();
}
// GUI•`‰æ
void InputMediator::OnDrawGui()
{
	for (const auto& controller : _inputControllers)
	{
		bool flag = controller->IsActive();
		ImGui::Checkbox(controller->GetName(), &flag);

		ImGui::Separator();
	}
}
// InputControllerBase‚©‚ç‚Ì–½—ß‚ðŽóM
void InputMediator::ReceiveCommand(const InputControllerBase* sender, CommandType commandType, const std::string& command)
{
	CommandData commandData;
	commandData.sender = sender;
	commandData.commandType = commandType;
	commandData.command = command;
	_commandHistory.push_back(commandData);
}

InputControllerBase::~InputControllerBase()
{
	// InputMediator‚©‚çŽ©g‚ðíœ
	if (_inputMediator.lock() != nullptr)
	{
		_inputMediator.lock()->UnregisterInputController(this);
	}
}

// InputMediator‚ð’T‚·
bool InputControllerBase::FindInputMediator()
{
	auto scene = GetActor()->GetScene();
	if (scene == nullptr)
		return false;
	auto mediator = scene->GetActorManager().FindByClass<InputMediator>(ActorTag::DrawContextParameter);
	if (mediator == nullptr)
		return false;
	// Mediator‚ÉŽ©g‚ð“o˜^
	mediator->RegisterInputController(this);
	// Mediator‚ð“o˜^
	_inputMediator = mediator;
	return true;
}
