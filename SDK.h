#pragma once
#include "memmane.h"
#include "memory.h"
#include "vector.h"
#include <string>
//#include "xorstr.h"
#include "includes/modules/security/xorstr.h"
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
        //    righthip_helper = 149,      // 右侧臀部助手
        //    righthandthumb1 = 145,      // 右手拇指1
        //    righthandthumb2 = 146,      // 右手拇指2
        //    righthandthumb3 = 147,      // 右手拇指3
        //    righthandthumb4 = 148,      // 右手拇指4
        //    lefthandpinky1 = 90,        // 左手小指1
        //    lefthandpinky2 = 91,        // 左手小指2
        //    lefthandpinky3 = 92,        // 左手小指3
        //    lefthandpinky4 = 93,        // 左手小指4
        //    head = 17,                  // 头部
        //    rightuplegroll = 31,        // 右上腿滚动
        //    rightarmroll = 115,         // 右臂滚动
        //    rblegankle = 34,            // 右腿踝关节
        //    rightforearmroll = 121,     // 右前臂滚动
        //    frontrleg2 = 21,            // 前右腿2
        //    frontrleg3 = 22,            // 前右腿3
        //    frontrleg1 = 20,            // 前右腿1
        //    righthandring = 140,        // 右手无名指
        //    leftforearmroll = 75,       // 左前臂滚动
        //    rightupleg = 30,            // 右上腿
        //    rightshoulder = 151,        // 右肩
        //    righthandring2 = 142,       // 右手无名指2
        //    righthandring3 = 143,       // 右手无名指3
        //    righthandring1 = 141,       // 右手无名指1
        //    righthandring4 = 144,       // 右手无名指4
        //    lear = 26,                  // 左耳
        //    lflegcollarbone = 24,       // 左腿锁骨
        //    larmcollarbone = 12,        // 左臂锁骨
        //    ludder2 = 15,               // 腰部2
        //    rightforearm = 117,         // 右前臂
        //    lefttoebase = 26,           // 左脚趾基部
        //    larmpalm = 13,              // 左臂掌
        //    rightkneeextra = 32,        // 右膝额外
        //    rarm1 = 22,                 // 右臂1
        //    rarmcollarbone = 23,        // 右臂锁骨
        //    lefthandindex4 = 83,        // 左手食指4
        //    lefthandindex3 = 82,        // 左手食指3
        //    lefthandindex2 = 81,        // 左手食指2
        //    lefthandindex1 = 80,        // 左手食指1
        //    pelvis = 10,                // 骨盆
        //    righttoebase = 36,          // 右脚趾基部
        //    leftshoulder = 105,         // 左肩
        //    leftlegroll = 24,           // 左腿滚动
        //    righthipextra = 150,        // 右臀额外
        //    lleg1 = 14,                 // 左腿1
        //    lleg3 = 18,                 // 左腿3
        //    lleg2 = 15,                 // 左腿2
        //    lefthandmiddle4 = 87,       // 左手中指4
        //    lefthandmiddle1 = 84,       // 左手中指1
        //    lefthandmiddle2 = 85,       // 左手中指2
        //    lefthandmiddle3 = 86,       // 左手中指3
        //    frontllegankle = 18,        // 前左腿踝关节
        //    jaw = 25,                   // 下颚
        //    righthand_dummy = 123,      // 右手假骨
        //    reye = 36,                  // 右眼
        //    rear = 35,                  // 后部
        //    leftarmextra = 68,          // 左臂额外
        //    rightfoot = 35,             // 右脚
        //    lefthandring4 = 98,         // 左手无名指4
        //    lefthandring3 = 97,         // 左手无名指3
        //    lefthandring2 = 96,         // 左手无名指2
        //    lefthandring1 = 95,         // 左手无名指1
        //    lefthipextra = 104,         // 左臀额外
        //    lefthip_helper = 103,       // 左侧臀部助手
        //    leftfoot = 25,              // 左脚
        //    rlegankle = 31,             // 右腿踝关节
        //    lfleg1 = 20,                // 左腿1
        //    lfleg3 = 22,                // 左腿3
        //    lfleg2 = 21,                // 左腿2
        //    lefthandthumb4 = 102,       // 左手拇指4
        //    lefthandthumb3 = 101,       // 左手拇指3
        //    lefthandthumb2 = 100,       // 左手拇指2
        //    lefthandthumb1 = 99,        // 左手拇指1
        //    lefthand = 76,              // 左手
        //    frontrlegcollarbone = 24,   // 前右腿锁骨
        //    righthandindex4 = 129,      // 右手食指4
        //    righthandindex1 = 126,      // 右手食指1
        //    righthandindex2 = 127,      // 右手食指2
        //    righthandindex3 = 128,      // 右手食指3
        //    rpalm_bone1 = 32,           // 右掌骨1
        //    rpalm_bone2 = 33,           // 右掌骨2
        //    eyeright = 18,              // 右眼
        //    leftarm = 67,               // 左臂
        //    leftupleg = 20,             // 左上腿
        //    neck1 = 16,                 // 颈部1
        //    righthandpinky4 = 139,      // 右手小指4
        //    righthandpinky3 = 138,      // 右手小指3
        //    righthandpinky2 = 137,      // 右手小指2
        //    righthandpinky1 = 136,      // 右手小指1
        //    leftarmroll = 69,           // 左臂滚动
        //    spine = 11,                 // 脊柱
        //    lflegankle = 23,            // 左腿踝关节
        //    rightwristextra = 153,      // 右腕额外
        //    rightarm = 113,             // 右臂
        //    lbleg1 = 11,                // 左腿1
        //    lbleg3 = 18,                // 左腿3
        //    lbleg2 = 14,                // 左腿2
        //    leftuplegroll = 21,         // 左上腿滚动
        //    rightarmextra = 114,        // 右臂额外
        //    righthand = 122,            // 右手
        //    righthandmiddle4 = 133,     // 右手中指4
        //    righthandmiddle1 = 130,     // 右手中指1
        //    righthandmiddle3 = 132,     // 右手中指3
        //    righthandmiddle2 = 131,     // 右手中指2
        //    frontlleg1 = 11,            // 前左腿1
        //    frontlleg2 = 14,            // 前左腿2
        //    frontlleg3 = 15,            // 前左腿3
        //    lookat = 18,                // 目光注视
        //    pin_lookat = 30,            // 固定目光注视
        //    lpalm_bone2 = 21,           // 左掌骨2
        //    lpalm_bone1 = 20,           // 左掌骨1
        //    lefthand_dummy = 77,        // 左手假骨
        //    rightforearmextra = 120,    // 右前臂额外
        //    rightelbowextra = 116,      // 右肘额外
        //    rightleg = 33,              // 右腿
        //    leftwristextra = 107,       // 左腕额外
        //    rightlegroll = 34,          // 右腿滚动
        //    rarmpalm = 24,              // 右臂掌
        //    rbleg3 = 33,                // 右腿3
        //    rbleg2 = 32,                // 右腿2
        //    rbleg1 = 31,                // 右腿1
        //    scene_root = 154,           // 场景根部
        //    leftleg = 23,               // 左腿
        //    leftkneeextra = 22,         // 左膝额外
        //    lefthandring = 94,          // 左手无名指
        //    rleg2 = 26,                 // 右腿2
        //    rleg1 = 25,                 // 右腿1
        //    neck = 15,                  // 颈部
        //    leftforearm = 71,           // 左前臂
        //    leftelbowextra = 70,        // 左肘额外
        //    tongue_5 = 15,              // 舌头5
        //    tongue_1 = 36,              // 舌头1
        //    leftforearmextra = 74,      // 左前臂额外
        //    spine1 = 12,                // 脊柱1
        //    spine2 = 13,                // 脊柱2
        //    spine3 = 14,                // 脊柱3
        //    frontrlegankle = 23         // 前右腿踝关节
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
    //    // 假设 `GetCamera` 返回相机， `GetInvertedViewForward` 返回角色朝向
    //    Vector3 forwardDir = GetInvertedViewForward(GetCamera());
    //    // 只使用X和Y分量来获得水平投影方向
    //    float angleRad = std::atan2(forwardDir.x, forwardDir.z);
    //    // 转换弧度为角度
    //    float angleDeg = angleRad * (180.0f / M_PI);
    //    // 将角度归一化到0-360范围
    //    if (angleDeg < 0) angleDeg += 360.0f;
    //    return angleDeg;
    //    /*
    //        0° 表示正北。
    //        90° 表示正东。
    //        180° 表示正南。
    //        270° 表示正西。
    //        中间的值（比如 45°）表示在北偏东的方向。
    //    */
    //}

    struct CameraAngles {
        float yaw;    // 偏航角（水平角度）
        float pitch;  // 俯仰角（垂直角度）
    };

    CameraAngles GetCameraAngles(uintptr_t Camera) {
        Vector3 forwardDir = GetInvertedViewForward(Camera);

        CameraAngles angles;

        // 偏航角（Yaw）：基于 X 和 Z 分量，描述水平角度
        angles.yaw = std::atan2(forwardDir.x, forwardDir.z) * (180.0f / M_PI);
        if (angles.yaw < 0) angles.yaw += 360.0f;  // 归一化到 0-360 度

        // 俯仰角（Pitch）：基于 Y 和水平距离，描述垂直角度
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