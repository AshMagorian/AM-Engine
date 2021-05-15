#include "FirstPersonCameraControls.h"
MAKE_PROTOTYPE(FirstPersonCameraControls)

void FirstPersonCameraControls::OnBegin()
{
	m_Transform = GetEntity()->GetTransform();

	m_startPos = m_Transform->GetPos();
	m_startRot = m_Transform->GetRotation();
	GetApplication()->GetCamera()->SetFPSCamera(false);
	GetApplication()->GetCamera()->SetWASD(true);
	GetApplication()->GetCamera()->SetMovementSpeed(3.0f);
	GetApplication()->GetCamera()->SetRotationSensitivity(0.1f);
}

void FirstPersonCameraControls::OnSceneBegin()
{
	GetEntity()->GetTransform()->SetPos(m_startPos);
	GetEntity()->GetTransform()->SetRotation(m_startRot);
}

void FirstPersonCameraControls::OnTick()
{
	if (GetApplication()->GetInput()->IsMouseButtonPressed("right"))
		GetApplication()->GetCamera()->SetFPSCamera(true);
	if (GetApplication()->GetInput()->IsMouseButtonReleased("right"))
		GetApplication()->GetCamera()->SetFPSCamera(false);
}

std::shared_ptr<Component> FirstPersonCameraControls::Clone()
{
	std::shared_ptr<Component> rtn = std::make_shared<FirstPersonCameraControls>(*this);
	return rtn;
}

void FirstPersonCameraControls::OnSave(Json::Value& val)
{
	SAVE_TYPE(FirstPersonCameraControls);
}
void FirstPersonCameraControls::OnLoad(Json::Value& val)
{

}
