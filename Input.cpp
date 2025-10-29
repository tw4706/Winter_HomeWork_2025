#include "Input.h"
#include<Dxlib.h>

Input::Input() :inputData_{}, lastInputData_{}, inputTable_{}
{
	inputTable_["left"] = { { PeripheralType::keyboard,KEY_INPUT_LEFT },
							{ PeripheralType::pad1,PAD_INPUT_LEFT} };
	inputTable_["right"] = { { PeripheralType::keyboard,KEY_INPUT_RIGHT },
							 { PeripheralType::pad1,PAD_INPUT_RIGHT} };
	inputTable_["jump"] = { {PeripheralType::keyboard,KEY_INPUT_SPACE},
							{PeripheralType::pad1,PAD_INPUT_A} };

	//変な値が入らないように枠を開けておく
	for (const auto& input : inputTable_) {
		inputData_[input.first] = false;
		lastInputData_[input.first] = false;
	}
}

void Input::Update()
{
	//入力情報の取得
	char keyState[256];
	GetHitKeyStateAll(keyState);
	int padState = GetJoypadInputState(DX_INPUT_PAD1);
	lastInputData_ = inputData_;

	//すべての入力イベントをチェック
	for (const auto& inputInfo : inputTable_) {
		auto& input = inputData_[inputInfo.first];
		for (const auto& state : inputInfo.second) {
			switch (state.type)
			{
			case PeripheralType::keyboard:
				input = keyState[state.id];
				break;
			case PeripheralType::pad1:
				input = (padState & state.id);
				break;
			}
			if (input) {//必須!
				break;
			}
		}
	}
}

bool Input::IsPressed(const char* name) const
{
	return inputData_.at(name);
}

bool Input::IsTriggered(const char* name) const
{
	return inputData_.at(name) && !lastInputData_.at(name);
}
