#pragma once
#include "memmane.h"
#include "memory.h"
#include "vector.h"
#include <string>
#include "xorstr.hpp"
#include <iostream>


namespace Game
{

	uintptr_t GetWorld()
	{
		return GameMemory->RVM<uint64_t>(globals.Base + OFF_WORLD);
	}

	uint64_t GetEntity(uint64_t PlayerList, uint64_t SelectedPlayer)
	{
		// Sorted Object
		return GameMemory->RVM<uint64_t>(PlayerList + SelectedPlayer * 0x8);
	}
	struct matrix4x4
	{
		float m[12];
	};

	std::string GetEntityTypeName(uint64_t Entity)
	{
		// Render Entity Type + Config Name
		auto EntityType = GameMemory->RVM<uint64_t>(Entity + 0x180);//Human::HumanType
		if (!EntityType)
			return "";
		auto ConfigName = GameMemory->RVM<uint64_t>(EntityType + 0xA8);//HumanType::CategoryName
		if (!ConfigName)
			return "";
		return ReadArmaString(ConfigName).c_str();
	}
	uint64_t GetLocalPlayer(uintptr_t pUWorld)
	{

		auto LocalPlayer = GameMemory->RVM<uint64_t>(pUWorld + 0x2960);
		if (LocalPlayer)
		{
			auto Result = GameMemory->RVM<uint64_t>(LocalPlayer + 0x8);
			if (Result)
			{
				return Result - 0xA8;
			}
		}
		return 0;
	}

	//utilities g_utils;
	//static uintptr_t get_bone_pos_player_sig = g_utils.signature("40 53 48 83 EC 20 48 8B 89 ? ? ? ? 48 8B DA E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC E9 ? ? ? ?");
	//static uintptr_t get_bone_pos_player_sig = g_utils.signature("40 53 48 83 EC 20 48 8B 89 ? ? ? ? 48 8B DA E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC");
	//static uintptr_t get_bone_pos_infected_sig = g_utils.signature("40 53 48 83 EC 20 48 8B 89 ? ? ? ? 48 8B DA E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ?");


	inline uint32_t GetBoneIndexByName(__int64 Entity, const char* boneName)
	{
		typedef uint32_t(__fastcall* fnGetBoneIndexByName)(void*, const char* boneName);
		auto getBoneIndexByName = reinterpret_cast<fnGetBoneIndexByName>((uintptr_t)(globals.Base + Function_GetBoneIndexByName_OFF));


		return getBoneIndexByName((void*)Entity, boneName);
	}

	inline Vector3 GetBonePosition(__int64 Entity, DWORD index)
	{

        //enum bone_id
        //{
        //    righthip_helper = 149,      // �Ҳ��β�����
        //    righthandthumb1 = 145,      // ����Ĵָ1
        //    righthandthumb2 = 146,      // ����Ĵָ2
        //    righthandthumb3 = 147,      // ����Ĵָ3
        //    righthandthumb4 = 148,      // ����Ĵָ4
        //    lefthandpinky1 = 90,        // ����Сָ1
        //    lefthandpinky2 = 91,        // ����Сָ2
        //    lefthandpinky3 = 92,        // ����Сָ3
        //    lefthandpinky4 = 93,        // ����Сָ4
        //    head = 17,                  // ͷ��
        //    rightuplegroll = 31,        // �����ȹ���
        //    rightarmroll = 115,         // �ұ۹���
        //    rblegankle = 34,            // �����׹ؽ�
        //    rightforearmroll = 121,     // ��ǰ�۹���
        //    frontrleg2 = 21,            // ǰ����2
        //    frontrleg3 = 22,            // ǰ����3
        //    frontrleg1 = 20,            // ǰ����1
        //    righthandring = 140,        // ��������ָ
        //    leftforearmroll = 75,       // ��ǰ�۹���
        //    rightupleg = 30,            // ������
        //    rightshoulder = 151,        // �Ҽ�
        //    righthandring2 = 142,       // ��������ָ2
        //    righthandring3 = 143,       // ��������ָ3
        //    righthandring1 = 141,       // ��������ָ1
        //    righthandring4 = 144,       // ��������ָ4
        //    lear = 26,                  // ���
        //    lflegcollarbone = 24,       // ��������
        //    larmcollarbone = 12,        // �������
        //    ludder2 = 15,               // ����2
        //    rightforearm = 117,         // ��ǰ��
        //    lefttoebase = 26,           // ���ֺ����
        //    larmpalm = 13,              // �����
        //    rightkneeextra = 32,        // ��ϥ����
        //    rarm1 = 22,                 // �ұ�1
        //    rarmcollarbone = 23,        // �ұ�����
        //    lefthandindex4 = 83,        // ����ʳָ4
        //    lefthandindex3 = 82,        // ����ʳָ3
        //    lefthandindex2 = 81,        // ����ʳָ2
        //    lefthandindex1 = 80,        // ����ʳָ1
        //    pelvis = 10,                // ����
        //    righttoebase = 36,          // �ҽ�ֺ����
        //    leftshoulder = 105,         // ���
        //    leftlegroll = 24,           // ���ȹ���
        //    righthipextra = 150,        // ���ζ���
        //    lleg1 = 14,                 // ����1
        //    lleg3 = 18,                 // ����3
        //    lleg2 = 15,                 // ����2
        //    lefthandmiddle4 = 87,       // ������ָ4
        //    lefthandmiddle1 = 84,       // ������ָ1
        //    lefthandmiddle2 = 85,       // ������ָ2
        //    lefthandmiddle3 = 86,       // ������ָ3
        //    frontllegankle = 18,        // ǰ�����׹ؽ�
        //    jaw = 25,                   // ���
        //    righthand_dummy = 123,      // ���ּٹ�
        //    reye = 36,                  // ����
        //    rear = 35,                  // ��
        //    leftarmextra = 68,          // ��۶���
        //    rightfoot = 35,             // �ҽ�
        //    lefthandring4 = 98,         // ��������ָ4
        //    lefthandring3 = 97,         // ��������ָ3
        //    lefthandring2 = 96,         // ��������ָ2
        //    lefthandring1 = 95,         // ��������ָ1
        //    lefthipextra = 104,         // ���ζ���
        //    lefthip_helper = 103,       // ����β�����
        //    leftfoot = 25,              // ���
        //    rlegankle = 31,             // �����׹ؽ�
        //    lfleg1 = 20,                // ����1
        //    lfleg3 = 22,                // ����3
        //    lfleg2 = 21,                // ����2
        //    lefthandthumb4 = 102,       // ����Ĵָ4
        //    lefthandthumb3 = 101,       // ����Ĵָ3
        //    lefthandthumb2 = 100,       // ����Ĵָ2
        //    lefthandthumb1 = 99,        // ����Ĵָ1
        //    lefthand = 76,              // ����
        //    frontrlegcollarbone = 24,   // ǰ��������
        //    righthandindex4 = 129,      // ����ʳָ4
        //    righthandindex1 = 126,      // ����ʳָ1
        //    righthandindex2 = 127,      // ����ʳָ2
        //    righthandindex3 = 128,      // ����ʳָ3
        //    rpalm_bone1 = 32,           // ���ƹ�1
        //    rpalm_bone2 = 33,           // ���ƹ�2
        //    eyeright = 18,              // ����
        //    leftarm = 67,               // ���
        //    leftupleg = 20,             // ������
        //    neck1 = 16,                 // ����1
        //    righthandpinky4 = 139,      // ����Сָ4
        //    righthandpinky3 = 138,      // ����Сָ3
        //    righthandpinky2 = 137,      // ����Сָ2
        //    righthandpinky1 = 136,      // ����Сָ1
        //    leftarmroll = 69,           // ��۹���
        //    spine = 11,                 // ����
        //    lflegankle = 23,            // �����׹ؽ�
        //    rightwristextra = 153,      // �������
        //    rightarm = 113,             // �ұ�
        //    lbleg1 = 11,                // ����1
        //    lbleg3 = 18,                // ����3
        //    lbleg2 = 14,                // ����2
        //    leftuplegroll = 21,         // �����ȹ���
        //    rightarmextra = 114,        // �ұ۶���
        //    righthand = 122,            // ����
        //    righthandmiddle4 = 133,     // ������ָ4
        //    righthandmiddle1 = 130,     // ������ָ1
        //    righthandmiddle3 = 132,     // ������ָ3
        //    righthandmiddle2 = 131,     // ������ָ2
        //    frontlleg1 = 11,            // ǰ����1
        //    frontlleg2 = 14,            // ǰ����2
        //    frontlleg3 = 15,            // ǰ����3
        //    lookat = 18,                // Ŀ��ע��
        //    pin_lookat = 30,            // �̶�Ŀ��ע��
        //    lpalm_bone2 = 21,           // ���ƹ�2
        //    lpalm_bone1 = 20,           // ���ƹ�1
        //    lefthand_dummy = 77,        // ���ּٹ�
        //    rightforearmextra = 120,    // ��ǰ�۶���
        //    rightelbowextra = 116,      // �������
        //    rightleg = 33,              // ����
        //    leftwristextra = 107,       // �������
        //    rightlegroll = 34,          // ���ȹ���
        //    rarmpalm = 24,              // �ұ���
        //    rbleg3 = 33,                // ����3
        //    rbleg2 = 32,                // ����2
        //    rbleg1 = 31,                // ����1
        //    scene_root = 154,           // ��������
        //    leftleg = 23,               // ����
        //    leftkneeextra = 22,         // ��ϥ����
        //    lefthandring = 94,          // ��������ָ
        //    rleg2 = 26,                 // ����2
        //    rleg1 = 25,                 // ����1
        //    neck = 15,                  // ����
        //    leftforearm = 71,           // ��ǰ��
        //    leftelbowextra = 70,        // �������
        //    tongue_5 = 15,              // ��ͷ5
        //    tongue_1 = 36,              // ��ͷ1
        //    leftforearmextra = 74,      // ��ǰ�۶���
        //    spine1 = 12,                // ����1
        //    spine2 = 13,                // ����2
        //    spine3 = 14,                // ����3
        //    frontrlegankle = 23         // ǰ�����׹ؽ�
        //};


		Vector3 Result = {};
		typedef void* (__thiscall* fn_bone_position_ws)(void* Entity, Vector3* result, unsigned int index);//    GetBonePositionWS
        auto bone_position_ws = reinterpret_cast<fn_bone_position_ws>((uintptr_t)(globals.Base + Function_GetBonePosition_OFF));
		bone_position_ws((void*)Entity, &Result, index);
		return Result;

	}


    bool IsDead(uintptr_t Entity)
    {
        return GameMemory->RVM<bool>(Entity + 0xE2);
    }


	uint64_t GetCamera()
	{
		auto World = GetWorld();
		if (World)
			return GameMemory->RVM<uint64_t>(World + 0x1B8);
		else
			return NULL;
	}
	Vector3 GetInvertedViewTranslation(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0x2C));
	}
	Vector3 GetInvertedViewRight(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0x8));
	}
	Vector3 GetInvertedViewUp(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0x14));
	}
	Vector3 GetInvertedViewForward(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0x20));
	}

	Vector3 GetViewportSize(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0x58));
	}

	Vector3 GetProjectionD1(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0xD0));
	}

	Vector3 GetProjectionD2(uintptr_t Camera)
	{
		return Vector3(GameMemory->RVM<Vector3>(Camera + 0xDC));
	}

    //float GetPlayerFacingAngle() {
    //    // ���� `GetCamera` ��������� `GetInvertedViewForward` ���ؽ�ɫ����
    //    Vector3 forwardDir = GetInvertedViewForward(GetCamera());
    //    // ֻʹ��X��Y���������ˮƽͶӰ����
    //    float angleRad = std::atan2(forwardDir.x, forwardDir.z);
    //    // ת������Ϊ�Ƕ�
    //    float angleDeg = angleRad * (180.0f / M_PI);
    //    // ���Ƕȹ�һ����0-360��Χ
    //    if (angleDeg < 0) angleDeg += 360.0f;
    //    return angleDeg;
    //    /*
    //        0�� ��ʾ������
    //        90�� ��ʾ������
    //        180�� ��ʾ���ϡ�
    //        270�� ��ʾ������
    //        �м��ֵ������ 45�㣩��ʾ�ڱ�ƫ���ķ���
    //    */
    //}

    struct CameraAngles {
        float yaw;    // ƫ���ǣ�ˮƽ�Ƕȣ�
        float pitch;  // �����ǣ���ֱ�Ƕȣ�
    };

    CameraAngles GetCameraAngles(uintptr_t Camera) {
        Vector3 forwardDir = GetInvertedViewForward(Camera);

        CameraAngles angles;

        // ƫ���ǣ�Yaw�������� X �� Z ����������ˮƽ�Ƕ�
        angles.yaw = std::atan2(forwardDir.x, forwardDir.z) * (180.0f / M_PI);
        if (angles.yaw < 0) angles.yaw += 360.0f;  // ��һ���� 0-360 ��

        // �����ǣ�Pitch�������� Y ��ˮƽ���룬������ֱ�Ƕ�
        float horizontalDist = std::sqrt(forwardDir.x * forwardDir.x + forwardDir.z * forwardDir.z);
        angles.pitch = std::atan2(forwardDir.y, horizontalDist) * (180.0f / M_PI);

        return angles;
    }


	Vector3 GetObjectVisualState(uintptr_t entity)
	{
		uintptr_t renderVisualState = GameMemory->RVM<uintptr_t>(entity + 0x1D0);
		Vector3 pos = GameMemory->RVM<Vector3>(renderVisualState + 0x2C);
		return pos;
	}
	float GetDistanceToMe(Vector3 Entity)
	{
		return Entity.Distance(GetObjectVisualState(GetLocalPlayer(GetWorld())));
	}

	bool WorldToScreen(Vector3 Position, Vector3& output)
	{
		auto LocalCamera = GetCamera();

		if (!LocalCamera) return false;

		Vector3 temp = Position - GetInvertedViewTranslation(LocalCamera);

		float x = temp.Dot(GetInvertedViewRight(LocalCamera));
		float y = temp.Dot(GetInvertedViewUp(LocalCamera));
		float z = temp.Dot(GetInvertedViewForward(LocalCamera));

		if (z < 0.1f)
			return false;

		Vector3 res(
			GetViewportSize(LocalCamera).x * (1 + (x / GetProjectionD1(LocalCamera).x / z)),
			GetViewportSize(LocalCamera).y * (1 - (y / GetProjectionD2(LocalCamera).y / z)),
			z);

		output.x = res.x;
		output.y = res.y;
		output.z = res.z;
		return true;
	}

	bool SetPosition(uint64_t Entity, Vector3 TargetPosition)
	{ 
		if (Entity == Game::GetLocalPlayer(GetWorld())) {
			*reinterpret_cast<Vector3*>(GameMemory->RVM<uint64_t>(Entity + 0xF0) + 0x2C) = TargetPosition;
		}
		else {
			*reinterpret_cast<Vector3*>(GameMemory->RVM<uint64_t>(Entity + 0x1D0) + 0x2C) = TargetPosition;
		}
		return true;
	}

}