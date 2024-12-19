//#include "GLFW/glfw3.h"

#include "memmane.h"
#include "overlay.h"
#include "SDK.h"
#include "xorstr.hpp"
#include <DirectXMath.h>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include "safecall.h"

#define UENGINE __stdcall

std::vector<player_t> entities = {};
std::vector<item_t> items = {};


namespace Config
{
	namespace ESP
	{
		namespace Player
		{
			bool ShowPlayer = true;
			bool ShowPlayerSkeleton = true;
			bool ShowPlayerHealth = true;
			bool ShowPlayerBox = false;
			bool ShowPlayerName = true;
			bool ShowPlayerHands = true;
			bool ShowPlayerSlaplines = false;
			bool ShowPlayerDistance = true;
			int MaxPlayerDistance = 1500;

		}
		namespace Zombie
		{
			bool ShowZombieSkeleton = false;
			bool ShowZombieBox = false;
			bool ShowZombie = true;

			bool ShowZombieHealth = true;
			bool ShowZombieName = false;
			bool ShowZombieDistance = true;
			int MaxZombieDistance = 400;
		}
		namespace Item
		{
			bool ShowItems = true;
			bool ShowWeapon = true;//武器
			bool ShowProxyMagazines = true;//弹药
			bool ShowItemOptics;//瞄准镜
			bool ShowRodas;//汽车配件
			bool ShowContainers = true;//容器
			bool ShowFood = true;//食物
			bool ShowBackPack = true;//背包
			bool ShowMedicine = true;//药品
			bool ShowDrink = true;//饮料
			bool ShowClothing = true;//衣物
			bool ShowOthers = true;//其他
			bool ShowItemQualidade = true;
			bool ShowItemInventory = true;
			bool ShowAllItems;//全部物品
			int MaxLootDistance = 1000;
		}


		bool ShowHouse = true;
		int MaxHouseDistance = 1500;

		bool ShowAnimal = true;//动物
		int MaxAnimalDistance = 1500;


		bool ShowCar = true;//车
		int MaxCarDistance = 1500;

	}
	namespace Aim
	{

		bool SilentAim = true;
		bool AmmoSpeedChange = false;
		bool ShowAimFov = 1;
		bool ShowCrossHair = 1;
		float AimFov = 100;
		int MaxAimDistance = 1500;
		float CrossHairSize = 2;
		int AimBonePosition = 17;//头
		int AimKey = VK_RBUTTON;

	}
	namespace Feature
	{

		bool LatitudeCheat = false;			
		float Latitude = 1.f;

		bool WorldLightCheat = false;		
		float WorldLight = 1.f;

		bool LootTeleport = 0;				
		int LootTeleportKey = VK_F6;

		bool WorldCompass = true;			

		bool ShowLocalPlayerPosition = true;

		bool ClearGrass = false;			

		bool PerfectWeather = false;		

		bool FreeCamera = true;				
		bool FreeCameraRemoteDrop = false;	
		float FreeCameraSpeed = 0.1f;
		int FreeCameraKey = VK_F7;


		bool ChangeFov = false;				
		float GameFov = 1.f;

		bool FightMode = false;				
		int FightModeKey = VK_OEM_3;

		//Vector3 RemoteDropVec = {  };
		
		bool SpeedHackCheat = false;
		float SpeedHack = 1;

		bool ShowWorldTime = {  };
		bool ShowPlayerCount = {  };


		bool EnglishMode = true;

		bool DebugMode = false;
	}
}




std::vector<uintptr_t> EntityListVector;


uintptr_t AimTargetEntity = {  };










struct nameid
{
	UINT64 pt1;
	UINT64 pt2;
};
std::map<UINT64, std::string> nameCache;


std::string getNameFromId(uintptr_t namePointer)
{
	nameid ID = GameMemory->RVM<nameid>(namePointer + 0x10);

	std::map<UINT64, std::string>::iterator it = nameCache.find(ID.pt1 + ID.pt2);

	if (it == nameCache.end())
	{

		int size = GameMemory->RVM<int>(namePointer + 0x8);
		if (size < 1)
			return "";
		char* name = new char[size];
		if (!ZwCopyMemory(namePointer + 0x10, name, size, false))
			return "";
		std::string text = std::string(name);
		delete name;
		nameCache.insert(std::pair<UINT64, std::string>(ID.pt1 + ID.pt2, text));
		return text;
	}
	else
	{
		return it->second;
	}
}




template <typename QualityType>
QualityType GetEntityQuality(uintptr_t Entity)
{
	auto Quality = GameMemory->RVM<int>(Entity + 0x194);
	QualityType Result{};

	if constexpr (std::is_same<QualityType, int>::value)
	{
		switch (Quality)
		{
		case 1: Result = 75; break;
		case 2: Result = 50; break;
		case 3: Result = 25; break;
		case 4: Result = 0; break;
		default: Result = 100; break;
		}
	}
	else if constexpr (std::is_same<QualityType, std::string>::value)
	{
		switch (Quality)
		{
		case 1:  Result  = Config::Feature::EnglishMode ? __("Worn") : (const char*)__(u8"破旧"); break;
		case 2:  Result  = Config::Feature::EnglishMode ? __("Damaged") : (const char*)__(u8"损坏"); break;
		case 3:  Result  = Config::Feature::EnglishMode ? __("Badly Damaged") : (const char*)__(u8"严重损坏"); break;
		case 4:  Result  = Config::Feature::EnglishMode ? __("Ruined") : (const char*)__(u8"毁坏"); break;
		default: Result  = Config::Feature::EnglishMode ? __("Pristine") : (const char*)__(u8"干净"); break;
		}
	}
	return Result;
}




std::string GetPlayerName(uintptr_t entity) {



	std::string playerName = __("IA");
	uintptr_t network_client = GameMemory->RVM<uintptr_t>(globals.Base + OFF_NETWORK_MANAGER + OFF_NETWROK_CLIENT);
	if (!network_client)
		return playerName;
	uint32_t entity_network_id = GameMemory->RVM<uint32_t>(entity + OFF_NETWORK_ID);
	if (entity_network_id < 1)
		return playerName;
	uintptr_t scoreboard = GameMemory->RVM<uintptr_t>(network_client + OFF_NETWORK_SCOREBOARD);
	uint32_t scoreboard_size = GameMemory->RVM<uint32_t>(network_client + OFF_NETWORK_SCOREBOARD_SIZE);
	if (scoreboard_size < 1)
		return playerName;
	for (uint32_t i = 0; i < scoreboard_size; i++) {
		uintptr_t current_identity = GameMemory->RVM<uintptr_t>(scoreboard + (i * sizeof(uintptr_t)));
		if (!current_identity)
			return __("Dead");
		uint32_t current_id = GameMemory->RVM<uint32_t>(current_identity + OFF_NETWORK_SCOREBOARD_ID);
		if (current_id == entity_network_id)
		{
			uintptr_t namePtr = GameMemory->RVM<uintptr_t>(current_identity + OFF_NETWORK_SCOREBOARD_NAME);
			playerName = ReadArmaString(namePtr);
			return playerName;
		}
	}
	return playerName;
}





uintptr_t GetClosestItem(std::vector<uintptr_t> list, float field_of_view)
{
	ImGuiIO& io = ImGui::GetIO();
	
	uintptr_t ResultTargetItemTemp = {};
	float closestTocenter = FLT_MAX;

	for (auto curr_entity : list)
	{
		Vector3 currentworld;
		Vector3 Pos = Game::GetObjectVisualState(curr_entity);
		if (Game::WorldToScreen(Pos, currentworld)) {
			auto dx = currentworld.x - (io.DisplaySize.x / 2);
			auto dy = currentworld.y - (io.DisplaySize.y / 2);
			auto dist = sqrtf(dx * dx + dy * dy);
			if (dist < field_of_view && dist < closestTocenter) {
				closestTocenter = dist;
				ResultTargetItemTemp = curr_entity;
			}
		}
	}

	return ResultTargetItemTemp;
}

uintptr_t GetClosestPlayer(std::vector<uintptr_t> list, float field_of_view)
{
	ImGuiIO& io = ImGui::GetIO();


	uintptr_t resultant_target_entity_temp = {  };
	float closestTocenter = FLT_MAX;

	for (auto curr_entity : list)
	{

		auto EntityTypeName = Game::GetEntityTypeName(curr_entity);
	
		if (EntityTypeName != __("dayzinfected") && EntityTypeName != __("dayzplayer"))
			continue;

		Vector3 current;
		Vector3 currentworld;


		current = Game::GetBonePosition(curr_entity, Game::GetBoneIndexByName(curr_entity, __("head")));


		if (Game::WorldToScreen(current, currentworld))
		{
			auto dx = currentworld.x - (io.DisplaySize.x / 2);
			auto dy = currentworld.y - (io.DisplaySize.y / 2);
			auto dist = sqrtf(dx * dx + dy * dy);
			if (dist < field_of_view && dist < closestTocenter) {
				closestTocenter = dist;
				resultant_target_entity_temp = curr_entity;
			}
		}

	}

	return resultant_target_entity_temp;
}



void SilentAimCallBack()
{
	auto* window = ImGui::GetBackgroundDrawList();
	ImGuiIO& io = ImGui::GetIO();

	Vector3 outPos;
	Vector3 Pos = Game::GetObjectVisualState(AimTargetEntity);


	int distance = Game::GetDistanceToMe(Pos);

	Game::WorldToScreen(Pos, outPos);



	Vector3 current;
	Vector3 currentworld;

	current = Game::GetBonePosition(AimTargetEntity, Config::Aim::AimBonePosition);


	Game::WorldToScreen(current, currentworld);

	window->AddLine(ImVec2{ (float)io.DisplaySize.x / 2, (float)io.DisplaySize.y / 2 }, ImVec2{ currentworld.x, currentworld.y }, 0xff0000ff, 1.5f);

	const auto pUWorld = Game::GetWorld();
	if (!pUWorld)
		return;


	uintptr_t entityTable = GameMemory->RVM<uintptr_t>(pUWorld + OFF_BULLET);
	if (!entityTable)
		return;

	int objectTableSz = GameMemory->RVM<int>(pUWorld + OFF_BULLETSIZE);
	if (objectTableSz < 1)
		return;


	for (uintptr_t i = NULL; i < objectTableSz; i++)
	{
		uintptr_t entity = GameMemory->RVM<uintptr_t>(entityTable + (i * 0x8));
		if (!entity)
			continue;


		if (Config::Aim::SilentAim)
			Game::SetPosition(entity, current);

	}



}

inline void __stdcall SilentAimFunction(uintptr_t World) {


	if (safe_call(GetAsyncKeyState)(Config::Aim::AimKey) & 0x8000)
	{
		if (!AimTargetEntity)
			AimTargetEntity = GetClosestPlayer(EntityListVector, Config::Aim::AimFov);
		if (!AimTargetEntity || Game::IsDead(AimTargetEntity))
		{
			AimTargetEntity = {  };
			return;
		}
		Vector3 worldPosition = Game::GetObjectVisualState(AimTargetEntity);
		int dist = Game::GetDistanceToMe(worldPosition);
		if (dist > Config::Aim::MaxAimDistance) {

			AimTargetEntity = {  };
			return;
		}
		if (Config::Aim::AmmoSpeedChange)
		{
			auto DayZPlayer = GameMemory->RVM<uintptr_t>(globals.Base + OFF_DAYZPLAYER);
			if (DayZPlayer)
			{
				auto PlayerInventory = GameMemory->RVM<uintptr_t>(DayZPlayer + OFF_PLAYER_INVENTORY);
				if (PlayerInventory)
				{
					auto PlayerInventoryHands = GameMemory->RVM<uintptr_t>(PlayerInventory + OFF_PLAYER_INVENTORY_HANDS);
					if (PlayerInventoryHands)
					{
						auto AmmoType1 = GameMemory->RVM<uintptr_t>(PlayerInventoryHands + OFF_AMMO_TYPE1);
						if (AmmoType1)
						{
							auto AmmoType2 = GameMemory->RVM<uintptr_t>(AmmoType1 + OFF_AMMO_TYPE2);
							if (GameMemory->isValidPointer((void*)(AmmoType2 + OFF_INIT_SPEED)))
								*(float*)(AmmoType2 + OFF_INIT_SPEED) = (float)(dist * 100);
						}
					}
				}
			}
		}

		SilentAimCallBack();
	}
	else
	{
		AimTargetEntity = {  };
	}


}


void LootTeleportFunction() {

	ImGuiIO& io = ImGui::GetIO();
	auto* window = ImGui::GetBackgroundDrawList();

	uintptr_t ff = GetClosestItem(EntityListVector, Config::Aim::AimFov);
	if (!ff)
		return;
	Vector3 currentworld;
	Vector3 Pos = Game::GetObjectVisualState(ff);
	if (Game::GetDistanceToMe(Pos) > 6.5)
		return;


	Game::WorldToScreen(Pos, currentworld);
	window->AddLine(ImVec2{ (float)io.DisplaySize.x / 2, (float)io.DisplaySize.y / 2 }, ImVec2{ currentworld.x, currentworld.y }, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 255, 0, 255)));
	if (safe_call(GetAsyncKeyState)(Config::Feature::LootTeleportKey) & 0x8000)
	{
		auto MyPlayer = Game::GetLocalPlayer(Game::GetWorld());
		Vector3 worldPosition2 = Game::GetObjectVisualState(MyPlayer);
		Game::SetPosition(ff, worldPosition2);

	}
}



namespace DrawColors
{
	//ImU32 WeaponItemColor = 0xff0000ff;//纯红
	//ImU32 BackPackColor = 0xff4cba23;//39 173 76 255 嫩绿
	//ImU32 ContainersColor = 0xff742104;//4 33 116 255 靛蓝
	//ImU32 FoodColor = 0xff00b88c;//140 184 0 255 绿黄
	//ImU32 MedicineColor = 0xffff0000;//纯蓝
	//ImU32 DrinkColor = 0xff7b5fb8;//184 95 123 255 淡粉
	//ImU32 ClothingColor = 0xffffffff;//纯白
	//ImU32 OthersColor = 0xffff0549;//73 5 255 255蓝紫
	//ImU32 AmmoColor = 0xff120e83;//131 14 18 255淡红
	//ImU32 ItemOpticsColor = 0xff306010;//16 96 48 255深绿
	//ImU32 RodasColor = 0xff00ffff;//纯黄
	ImU32 WeaponItemColor = 0xff0000ff;  // 纯红
	ImU32 BackPackColor = 0xff3cb44b;    // 亮绿
	ImU32 ContainersColor = 0xff4363d8;  // 亮蓝
	ImU32 FoodColor = 0xfff58231;        // 橙色
	ImU32 MedicineColor = 0xffcbc0ff;    // 粉色
	ImU32 DrinkColor = 0xff42d4f4;       // 青色
	ImU32 ClothingColor = 0xfff032e6;    // 亮紫色
	ImU32 OthersColor = 0xfffabebe;      // 淡粉色
	ImU32 AmmoColor = 0xffffd700;        // 金色
	ImU32 ItemOpticsColor = 0xff469990;  // 海绿
	ImU32 RodasColor = 0xff9a6324;       // 棕色

}

void IterateAll(uintptr_t worldptr, std::vector<short> offset)
{
	auto* window = ImGui::GetBackgroundDrawList();
	ImGuiIO& io = ImGui::GetIO();

	
	for (auto EntityOffset : offset)
	{



		uintptr_t entityTable = GameMemory->RVM<uintptr_t>(worldptr + EntityOffset);
		if (!entityTable)
			return;
		int objectTableSz = GameMemory->RVM<int>(worldptr + EntityOffset + 0x8);
		if (objectTableSz < 1)
			return;
		Vector3 outPos;
		for (size_t i = 0; i < objectTableSz; i++)
		{
			if (i > GameMemory->RVM<int>(worldptr + EntityOffset + 0x8))
				break;
			uintptr_t entity = GameMemory->RVM<uintptr_t>(entityTable + i * 0x8);

			if (!entity) 
			{
				continue;
			}
			else
			{

				auto EntityTypeName = Game::GetEntityTypeName(entity);				     if (EntityTypeName.empty()) continue;
				
				Vector3 EntityPosition = Game::GetObjectVisualState(entity);		  	 if (EntityPosition.x == -1 && EntityPosition.y == -1 && EntityPosition.z == -1) continue;
																						
				int distance = Game::GetDistanceToMe(EntityPosition);					 if (!Game::WorldToScreen(EntityPosition, outPos)) continue;
																						
				uintptr_t objectBase = GameMemory->RVM<uintptr_t>(entity + 0x180);			 if (!objectBase) continue;
																						
				uintptr_t CleanNamePtr = GameMemory->RVM<uintptr_t>(objectBase + OFF_CLEANNAME_1);	 if (!CleanNamePtr)	continue;
																						
				std::string CleanName = getNameFromId(CleanNamePtr);					 if (CleanName.empty()) continue;

				if (Config::Feature::DebugMode)
				{
					if ( /* EntityTypeName != __("dayzplayer")&&*/ EntityTypeName != __("dayzinfected")  && EntityTypeName != __("dayzanimal") && EntityTypeName != __("car")/* && EntityTypeName != __("house")*/)
					{
						window->AddText(ImVec2(outPos.x, outPos.y + 20.f), 0xffffffff, (EntityTypeName + __(" | ") + CleanName + std::to_string(entity)).c_str());//白色
					}
				}


				auto CleanAndDistanceName = CleanName + __("[") + std::to_string(distance) + __("]");


				if (Config::ESP::Zombie::ShowZombie) {
					if (EntityTypeName == __("dayzinfected")) {
						if (distance > Config::ESP::Zombie::MaxZombieDistance)
							continue;
						if (distance < 2)
							continue;
						if (Game::IsDead(entity))
							continue;

						EntityListVector.push_back(entity);

						if (Config::ESP::Zombie::ShowZombieSkeleton)
						{
							std::string bones_names[19][2] = {
							{ __("head"), __("neck")},
							{ __("neck"), __("spine3")},
							{ __("spine3"), __("pelvis")},
							{ __("neck"), __("leftarm")},
							{ __("leftarm"), __("leftforearm")},
							{ __("leftforearm"), __("lefthand")},
							{ __("lefthand"), __("lefthandmiddle4")},
							{ __("pelvis"), __("leftupleg")},
							{ __("leftupleg"), __("leftleg")},
							{ __("leftleg"), __("leftfoot")},
							{ __("leftfoot"), __("lefttoebase")},
							{ __("neck"), __("rightarm") },
							{ __("rightarm"), __("rightforearm")},
							{ __("rightforearm"), __("righthand")},
							{ __("righthand"), __("righthandmiddle4")},
							{ __("pelvis"), __("rightupleg")},
							{ __("rightupleg"), __("rightleg")},
							{ __("rightleg"), __("rightfoot")},
							{ __("rightfoot"), __("righttoebase") }
							};

							for (std::string* bone : bones_names)
							{
								uint32_t fromIdx = Game::GetBoneIndexByName(entity, bone[0].c_str());
								uint32_t toIdx = Game::GetBoneIndexByName(entity, bone[1].c_str());

								Vector3 fromPos = Game::GetBonePosition(entity, fromIdx);
								Vector3 toPos = Game::GetBonePosition(entity, toIdx);

								Vector3 sFromPos, sToPos;
								if (!Game::WorldToScreen(fromPos, sFromPos) || !Game::WorldToScreen(toPos, sToPos))
									continue;
								window->AddLine(ImVec2(sFromPos.x, sFromPos.y), ImVec2(sToPos.x, sToPos.y), 0xffffffff);
							}
						}
						if (Config::ESP::Zombie::ShowZombieBox || Config::ESP::Zombie::ShowZombieHealth || Config::ESP::Zombie::ShowZombieDistance || Config::ESP::Zombie::ShowZombieName)
						{
						
							auto head_pos = Game::GetBonePosition(entity, 17);
							Vector3 screen_head_pos;
							if (!Game::WorldToScreen(head_pos + 0.2f, screen_head_pos)) continue;

							float box_h = fabs(screen_head_pos.y - outPos.y);
							float box_w = box_h / 1.65f;

							float left = outPos.x - box_w * 0.5f;
							float right = left + box_w;
							float bottom = outPos.y;
							float top = screen_head_pos.y;

							
							if (Config::ESP::Zombie::ShowZombieBox)
							{
								window->AddRect(ImVec2(left, top), ImVec2(right, bottom), 0xff00ff00, 0);
							}

					
							if (Config::ESP::Zombie::ShowZombieHealth)
							{

								int health = GetEntityQuality<int>(entity);  
								float health_box_h = fabs(bottom - top);
								float offset = 6.0f;

								
								float health_bar_height = (health_box_h * health) / 100.0f;

							
								int green = static_cast<int>(health * 2.55f);
								int red = 255 - green;

								
								float health_bar_x = left - offset;
								float health_bar_y_start = bottom;
								float health_bar_y_end = bottom - health_bar_height;
								float health_bar_width = 4.0f;

								
								window->AddRectFilled(ImVec2(health_bar_x, top), ImVec2(health_bar_x + health_bar_width, health_bar_y_start),
									ImColor(0, 0, 0, 255), 1.0f);
								window->AddRectFilled(ImVec2(health_bar_x + 1, health_bar_y_end + 1), ImVec2(health_bar_x + health_bar_width - 1, health_bar_y_start - 1),
									ImColor(red, green, 0, 255), 1.0f);
							}

					
							if (Config::ESP::Zombie::ShowZombieName) {
								float name_x = (left + right) * 0.5f - ImGui::CalcTextSize(CleanName.c_str()).x * 0.5f;
								window->AddText(ImVec2(name_x, top - ImGui::GetFontSize()), 0xff00ff00, CleanName.c_str());
							}
							if (Config::ESP::Zombie::ShowZombieDistance) {
								std::string zombieDistance = __("[") + std::to_string(distance) + __("]");
								float distance_x = (left + right) * 0.5f - ImGui::CalcTextSize(zombieDistance.c_str()).x * 0.5f;
								window->AddText(ImVec2(distance_x, bottom + 2), 0xff00ff00, zombieDistance.c_str());
							}
						}


	

						//window->AddText(ImVec2(outPos.x, outPos.y), 0xff0000ff, (std::to_string(distance)).c_str());


					}
				}
				if (Config::ESP::Player::ShowPlayer) {

					if (EntityTypeName == __("dayzplayer")) {
						if (distance > Config::ESP::Player::MaxPlayerDistance)
							continue;
						if (distance < 2)
							continue;
						EntityListVector.push_back(entity);

	

						if (Config::ESP::Player::ShowPlayerSlaplines)
							window->AddLine(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y), ImVec2(outPos.x, outPos.y), 0xff0000ff);
						
						if (Config::ESP::Player::ShowPlayerSkeleton)
						{
							std::string bones_names[19][2] = {
							{ __("head"), __("neck")},
							{ __("neck"), __("spine3")},
							{ __("spine3"), __("pelvis")},
							{ __("neck"), __("leftarm")},
							{ __("leftarm"), __("leftforearm")},
							{ __("leftforearm"), __("lefthand")},
							{ __("lefthand"), __("lefthandmiddle4")},
							{ __("pelvis"), __("leftupleg")},
							{ __("leftupleg"), __("leftleg")},
							{ __("leftleg"), __("leftfoot")},
							{ __("leftfoot"), __("lefttoebase")},
							{ __("neck"), __("rightarm") },
							{ __("rightarm"), __("rightforearm")},
							{ __("rightforearm"), __("righthand")},
							{ __("righthand"), __("righthandmiddle4")},
							{ __("pelvis"), __("rightupleg")},
							{ __("rightupleg"), __("rightleg")},
							{ __("rightleg"), __("rightfoot")},
							{ __("rightfoot"), __("righttoebase") }
							};

							for (std::string* bone : bones_names)
							{
								uint32_t fromIdx = Game::GetBoneIndexByName(entity, bone[0].c_str());
								uint32_t toIdx = Game::GetBoneIndexByName(entity, bone[1].c_str());

								Vector3 fromPos = Game::GetBonePosition(entity, fromIdx);
								Vector3 toPos = Game::GetBonePosition(entity, toIdx);

								Vector3 sFromPos, sToPos;
								if (!Game::WorldToScreen(fromPos, sFromPos) || !Game::WorldToScreen(toPos, sToPos))
									continue;
								window->AddLine(ImVec2(sFromPos.x, sFromPos.y), ImVec2(sToPos.x, sToPos.y), 0xff0000ff, 1.f);
							}


						}
						if (Config::ESP::Player::ShowPlayerBox || Config::ESP::Player::ShowPlayerHealth || Config::ESP::Player::ShowPlayerDistance || Config::ESP::Player::ShowPlayerName || Config::ESP::Player::ShowPlayerHands)
						{
			
							auto head_pos = Game::GetBonePosition(entity, 17);
							Vector3 screen_head_pos;
							if (!Game::WorldToScreen(head_pos + 0.2f, screen_head_pos)) continue;

							float box_h = fabs(screen_head_pos.y - outPos.y);
							float box_w = box_h / 1.65f;

							float left = outPos.x - box_w * 0.5f;
							float right = left + box_w;
							float bottom = outPos.y;
							float top = screen_head_pos.y;

						
							if (Config::ESP::Player::ShowPlayerBox)
							{
								window->AddRect(ImVec2(left, top), ImVec2(right, bottom), 0xff0000ff, 0);
							}

					
							if (Config::ESP::Player::ShowPlayerHealth)
							{

								int health = GetEntityQuality<int>(entity);  
								float health_box_h = fabs(bottom - top);
								float offset = 6.0f;

								
								float health_bar_height = (health_box_h * health) / 100.0f;

								
								int green = static_cast<int>(health * 2.55f);
								int red = 255 - green;

						
								float health_bar_x = left - offset;
								float health_bar_y_start = bottom;
								float health_bar_y_end = bottom - health_bar_height;
								float health_bar_width = 4.0f;

								
								window->AddRectFilled(ImVec2(health_bar_x, top), ImVec2(health_bar_x + health_bar_width, health_bar_y_start),
									ImColor(0, 0, 0, 255), 1.0f);
								window->AddRectFilled(ImVec2(health_bar_x + 1, health_bar_y_end + 1), ImVec2(health_bar_x + health_bar_width - 1, health_bar_y_start - 1),
									ImColor(red, green, 0, 255), 1.0f);
							}
						
							if (Config::ESP::Player::ShowPlayerName) {
								std::string playerName = GetPlayerName(entity);
								float name_x = (left + right) * 0.5f - ImGui::CalcTextSize(playerName.c_str()).x * 0.5f;
								window->AddText(ImVec2(name_x, top - ImGui::GetFontSize()), 0xff0000ff, playerName.c_str());
							}
							if (Config::ESP::Player::ShowPlayerDistance) {
								std::string playerDistance = __("[") + std::to_string(distance) + __("]");
								float distance_x = (left + right) * 0.5f - ImGui::CalcTextSize(playerDistance.c_str()).x * 0.5f;

								window->AddText(ImVec2(distance_x, bottom + 2), 0xff0000ff, playerDistance.c_str());

								if (Config::ESP::Player::ShowPlayerHands) {
									auto ItemInventory = GameMemory->RVM<uintptr_t>(entity + OFF_PLAYER_INVENTORY);
									if (ItemInventory) {
										auto PlayerHands = GameMemory->RVM<uintptr_t>(ItemInventory + OFF_PLAYER_INVENTORY_HANDS);
										if (PlayerHands) {
											auto PlayerHandEntityType = GameMemory->RVM<uintptr_t>(PlayerHands + OFF_PLAYER_HUMAN_TYPE);
											auto HandsCleanName = getNameFromId(GameMemory->RVM<uintptr_t>(PlayerHandEntityType + OFF_CLEANNAME_1));

				
											float hands_y = bottom + 2 + ImGui::CalcTextSize(playerDistance.c_str()).y + 5;
											window->AddText(ImVec2(distance_x, hands_y), 0xff0000ff, HandsCleanName.c_str());
										}
									}
								}
							}
							else { 
								if (Config::ESP::Player::ShowPlayerHands) {
									auto ItemInventory = GameMemory->RVM<uintptr_t>(entity + OFF_PLAYER_INVENTORY);
									if (ItemInventory) {
										auto PlayerHands = GameMemory->RVM<uintptr_t>(ItemInventory + OFF_PLAYER_INVENTORY_HANDS);
										if (PlayerHands) {
											auto PlayerHandEntityType = GameMemory->RVM<uintptr_t>(PlayerHands + OFF_PLAYER_HUMAN_TYPE);
											auto HandsCleanName = getNameFromId(GameMemory->RVM<uintptr_t>(PlayerHandEntityType + OFF_CLEANNAME_1));
										
											float distance_x = (left + right) * 0.5f - ImGui::CalcTextSize(HandsCleanName.c_str()).x * 0.5f;
											
											float hands_y = bottom + 2; 
											window->AddText(ImVec2(distance_x, hands_y), 0xff0000ff, HandsCleanName.c_str());
										}
									}
								}
							}


							
						}


					}


				}

				if (!Config::Feature::FightMode)
				{
					if (Config::ESP::ShowAnimal) {
						if (EntityTypeName == __("dayzanimal")) {
							if (distance > Config::ESP::MaxAnimalDistance)
								continue;
							EntityListVector.push_back(entity);
							window->AddText(ImVec2(outPos.x, outPos.y), 0xffffffff, CleanAndDistanceName.c_str());//白色
						}
					}
					if (Config::ESP::ShowCar) {
						if (EntityTypeName == __("car")) {

							if (distance > Config::ESP::MaxCarDistance)
								continue;
							EntityListVector.push_back(entity);

							window->AddText(ImVec2(outPos.x, outPos.y), 0xffffff00, CleanAndDistanceName.c_str());//蓝色

						}

					}
					if (Config::ESP::ShowHouse)
					{
						if (EntityTypeName == __("house")) {
							if (distance > Config::ESP::MaxAnimalDistance)
								continue;
							EntityListVector.push_back(entity);
							std::string HouseDrawText = Config::Feature::EnglishMode ? __("HouseBase") : (const char*)__(u8"地基");
							window->AddText(ImVec2(outPos.x, outPos.y), 0xffffffff, ((HouseDrawText)+__("[") + std::to_string(distance) + __("]")).c_str());//白色
						}

					}
				}

			}
		}
	}





}


std::vector<ImVec2> TempItemOverlayArr;
inline bool IsRange(int CurrentValue, int Min, int Max)
{
	return CurrentValue > Min && CurrentValue < Max;
}
inline void IsOverlay(ImVec2& ScreenVec)
{
	for (int i = 0; i < TempItemOverlayArr.size(); i++)
	{
		if (IsRange(ScreenVec.x, TempItemOverlayArr[i].x - 50, TempItemOverlayArr[i].x + 50) && IsRange(ScreenVec.y, TempItemOverlayArr[i].y - 50, TempItemOverlayArr[i].y + 50))
		{
			ScreenVec = TempItemOverlayArr[i];
			ScreenVec.y = ScreenVec.y - 13.f;
		}
	}
}
void AddDrawItem(uintptr_t Entity, ImVec2 D2D, ImU32 ItemColor, std::string ItemName) {

	auto* window = ImGui::GetBackgroundDrawList();

	IsOverlay(D2D);
	TempItemOverlayArr.push_back(D2D);
	if (Config::ESP::Item::ShowItemQualidade) {
		ItemName.append("-").append(GetEntityQuality<std::string>(Entity));
	}
	window->AddText(D2D, ItemColor, ItemName.c_str());
	//if (ShowItemQualidade)
	//	window->AddText(ImVec2(D2D.x, D2D.y + 15), ColoroU32(Entity), get_quality(Entity).c_str());
}


void DrawItemInventoryList(uintptr_t ItemEntity, ImVec2 D2D, ImU32 ItemColor, std::string ItemName)
{
	AddDrawItem(ItemEntity, D2D, ItemColor, ItemName);
	if (Config::ESP::Item::ShowItemInventory)
	{
		auto ItemInventory = GameMemory->RVM<uintptr_t>(ItemEntity + OFF_ITEM_INVENTORY);
		auto CargoGrid = GameMemory->RVM<uintptr_t>(ItemInventory + OFF_ITEM_CARGOGRID);
		auto InventoryList = GameMemory->RVM<uintptr_t>(CargoGrid + OFF_ITEM_LIST);
		auto InventoryListSize = GameMemory->RVM<int>(CargoGrid + OFF_ITEM_LIST + 0x8);
		for (size_t i = 0; i < InventoryListSize; i++)
		{
			if (i > GameMemory->RVM<int>(CargoGrid + OFF_ITEM_LIST + 0x8))
				break;
			uintptr_t entity = GameMemory->RVM<uintptr_t>(InventoryList + i * 0x8);
			if (!entity) continue;

			uintptr_t objectBase = GameMemory->RVM<uintptr_t>(entity + 0x180);
			if (!objectBase) continue;

	
			uintptr_t cleanNamePtr = GameMemory->RVM<uintptr_t>(objectBase + OFF_CLEANNAME_1);//OFF_CleanName
			if (!cleanNamePtr)continue;

			std::string text = getNameFromId(cleanNamePtr);
			if (text.empty()) continue;
			AddDrawItem(entity, D2D, ItemColor, text);

		}
	}


}

void IterateItems(uintptr_t pUWorld)
{
	auto* window = ImGui::GetBackgroundDrawList();

	int objectTableSz = GameMemory->RVM<int>(pUWorld + OFF_WORLD_ITEMTABLESIZE);
	uintptr_t entityTable = GameMemory->RVM<uintptr_t>(pUWorld + OFF_WORLD_ITEMTABLE);

	if (!entityTable) return;

	TempItemOverlayArr.clear();

	for (size_t i = 0; i < objectTableSz; i++) {
		
		if (i > GameMemory->RVM<int>(pUWorld + OFF_WORLD_ITEMTABLESIZE))
			break;

		uintptr_t entity = GameMemory->RVM<uintptr_t>(entityTable + i * 0x18 + 0x8);
		uint32_t flag = GameMemory->RVM<uint32_t>(entityTable + i * 0x18);

		if (!entity || flag != 1) continue;

	
		Vector3 EntityPosition = Game::GetObjectVisualState(entity);
		Vector3 outPos;

		if (!Game::WorldToScreen(EntityPosition, outPos)) continue;

		ImVec2 textPosition(outPos.x, outPos.y);
		int dist = Game::GetDistanceToMe(EntityPosition);


		if (dist < 1 || dist > Config::ESP::Item::MaxLootDistance) continue;

		uintptr_t objectBase = GameMemory->RVM<uintptr_t>(entity + 0x180);
		if (!objectBase) continue;


		uintptr_t cleanNamePtr  = GameMemory->RVM<uintptr_t>(objectBase + OFF_CLEANNAME_1);//OFF_CleanName
		uintptr_t cleanNamePtr2 = GameMemory->RVM<uintptr_t>(objectBase + OFF_CLEANNAME_2);
		uintptr_t cleanNamePtr5 = GameMemory->RVM<uintptr_t>(objectBase + OFF_CLEANNAME_3);


		if (!cleanNamePtr || !cleanNamePtr2 || !cleanNamePtr5) continue;

		std::string text = getNameFromId(cleanNamePtr);
		std::string text2 = getNameFromId(cleanNamePtr2);
		std::string text3 = getNameFromId(cleanNamePtr5);
		if (text.empty() || text2.empty() || text3.empty()) continue;

		EntityListVector.push_back(entity);

		std::string displayText = text + __(" [") + std::to_string(dist) + __("]");
		if (Config::Feature::DebugMode)
		{
			window->AddText(ImVec2(outPos.x, outPos.y + 20.f), 0xffffffff, (text + __(" | ") + text2 + __(" | ") + text3 + __(" | ") + std::to_string(entity)).c_str());//白色
		}
	
		if (Config::ESP::Item::ShowWeapon || Config::ESP::Item::ShowAllItems) {
			if (text2.find(__("Weapon")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::WeaponItemColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowBackPack || Config::ESP::Item::ShowAllItems) {
			if (text3.find(__("backpacks")) != std::string::npos) {
				DrawItemInventoryList(entity, textPosition, DrawColors::BackPackColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowContainers || Config::ESP::Item::ShowAllItems) {
			if (text3.find(__("containers")) != std::string::npos || text3.find(__("cooking")) != std::string::npos) {
				DrawItemInventoryList(entity, textPosition, DrawColors::ContainersColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowFood || Config::ESP::Item::ShowAllItems) {
			if (text3.find(__("food")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::FoodColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowMedicine || Config::ESP::Item::ShowAllItems) {
			if (text3.find(__("medical")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::MedicineColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowDrink || Config::ESP::Item::ShowAllItems) {
			if (text3.find(__("drinks")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::DrinkColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowClothing || Config::ESP::Item::ShowAllItems) {
			if (text2.find(__("clothing")) != std::string::npos) {
				DrawItemInventoryList(entity, textPosition, DrawColors::ClothingColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowOthers || Config::ESP::Item::ShowAllItems) {
		
			if (text.find(__("Cerca")) != std::string::npos || text3.find(__("Fogueira")) != std::string::npos || text2.find(__("inventoryItem")) != std::string::npos) {
				DrawItemInventoryList(entity, textPosition, DrawColors::OthersColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowProxyMagazines || Config::ESP::Item::ShowAllItems) {
			if (text2.find(__("ProxyMagazines")) != std::string::npos || text3.find(__("ammunition")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::AmmoColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowItemOptics || Config::ESP::Item::ShowAllItems) {
			if (text2.find(__("itemoptics")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::ItemOpticsColor, displayText);
				continue;
			}
		}
		if (Config::ESP::Item::ShowRodas || Config::ESP::Item::ShowAllItems) {
			if (text2.find(__("carwheel")) != std::string::npos) {
				AddDrawItem(entity, textPosition, DrawColors::ItemOpticsColor, displayText);
				continue;
			}
		}

	}
}

namespace CameraControler
{

	void* GetFreeDebugCamera()
	{

		typedef void* (__fastcall* tFunction)();
		auto function = reinterpret_cast<tFunction>(uintptr_t(globals.Base + 0x483850));//4C 8B DC 48 83 EC 48
		return function();
	}
	int IsFreeDebugCameraActive()
	{
		typedef bool(__thiscall* tFunction)(void*);
		auto function = reinterpret_cast<tFunction>(uintptr_t(globals.Base + 0x47CDF0));//48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8D 05 ? ? ? ? 48 3B C2  函数名:IsActive
		auto c = GetFreeDebugCamera();
		if (!c) return -1;

		return function(c);
	}
	void SetFreeCamActive(bool active)
	{
		typedef __int64(__thiscall* tFunction)(void*, bool);
		auto function = reinterpret_cast<tFunction>(uintptr_t(globals.Base + 0x47CC3C));//48 8B 01 FF A0 20 09 00 00  函数名:SetActive
		auto c = GetFreeDebugCamera();
		if (!c) return;
		function(c, active);
	}


};

void DrawCompass(ImDrawList* drawList, const ImVec2& screenCenter, float playerAngle) {
	
	float compassWidth = 300.0f;			
	float compassHeight = 30.0f;			
	float textSpacing = compassWidth / 4.0f;

	ImVec2 arrowCenter = ImVec2(screenCenter.x, screenCenter.y);


	float angleOffsets[] = { 0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f };



	ImGui::PushFont(globals.CompassFont);


	for (int i = 0; i < 8; ++i) {
		
		float offsetAngle = angleOffsets[i] - playerAngle;

	
		if (offsetAngle > 180.0f) offsetAngle -= 360.0f;
		if (offsetAngle < -180.0f) offsetAngle += 360.0f;


		float posX = arrowCenter.x + (offsetAngle / 45.0f) * textSpacing;
		ImVec2 textPos(posX, compassHeight);  

	
		std::string DirectionsText;
		switch (i)
		{
		case 0:
		{
			DirectionsText = Config::Feature::EnglishMode ? __("N") : (const char*)__(u8"北");
			break;
		}
		case 1:
		{
			DirectionsText = Config::Feature::EnglishMode ? __("NE") :(const char*)__(u8"东北");
			break;		
		}				
		case 2:			
		{				
			DirectionsText = Config::Feature::EnglishMode ? __("E") :(const char*)__(u8"东");
			break;
		}			
		case 3:		
		{			
			DirectionsText = Config::Feature::EnglishMode ? __("SE") :(const char*)__(u8"东南");
			break;		
		}				
		case 4:			
		{				
			DirectionsText = Config::Feature::EnglishMode ? __("S") :(const char*)__(u8"南");
			break;
		}			
		case 5:		
		{			
			DirectionsText = Config::Feature::EnglishMode ? __("SW") :(const char*)__(u8"西南");
			break;		
		}				
		case 6:			
		{				
			DirectionsText = Config::Feature::EnglishMode ? __("W") :(const char*)__(u8"西");
			break;	
		}			
		case 7:		
		{			
			DirectionsText = Config::Feature::EnglishMode ? __("NW") :(const char*)__(u8"西北");
			break;
		}
		default:
			break;
		}
		drawList->AddText(ImVec2(textPos.x - ImGui::CalcTextSize(DirectionsText.c_str()).x / 2, textPos.y),
			IM_COL32(255, 255, 255, 255), DirectionsText.c_str());

	}


	std::string angleText = std::to_string(static_cast<int>(playerAngle));
	ImVec2 anglePos(arrowCenter.x - ImGui::CalcTextSize(angleText.c_str()).x / 2,
		compassHeight + 20.f); 


	drawList->AddText(anglePos, IM_COL32(255, 255, 0, 255), angleText.c_str());
	ImGui::PopFont();

}



Vector3 CalculateMovementOffset(float yawAngle, float pitchAngle, float speed) {
	Vector3 offset;
	float yawRadians = yawAngle * (3.14159265f / 180.0f);
	float pitchRadians = pitchAngle * (3.14159265f / 180.0f);


	offset.x = std::sin(yawRadians) * std::cos(pitchRadians) * speed;  // 左右方向
	offset.y = std::sin(pitchRadians) * speed;                         // 上下方向
	offset.z = std::cos(yawRadians) * std::cos(pitchRadians) * speed;  // 前后方向

	return offset;
}

void HandleCameraMovement(Vector3& playerPosition, float yawAngle, float pitchAngle, float speed) {
	Vector3 offset = CalculateMovementOffset(yawAngle, pitchAngle, speed);

	if (safe_call(GetAsyncKeyState)('W') & 0x8000) {
		playerPosition.x += offset.x;  
		playerPosition.y += offset.y;  
		playerPosition.z += offset.z;
	}
	if (safe_call(GetAsyncKeyState)('S') & 0x8000) {
		playerPosition.x -= offset.x;  
		playerPosition.y -= offset.y;
		playerPosition.z -= offset.z;
	}

	Vector3 strafeOffset = CalculateMovementOffset(yawAngle + 90.0f, 0.0f, speed);

	if (safe_call(GetAsyncKeyState)('A') & 0x8000) {
		playerPosition.x -= strafeOffset.x;  
		playerPosition.z -= strafeOffset.z;
	}
	if (safe_call(GetAsyncKeyState)('D') & 0x8000) {
		playerPosition.x += strafeOffset.x;  
		playerPosition.z += strafeOffset.z;
	}
}


std::vector<short> EntityTable = { OFF_NEAR_ENTITY_TABLE,OFF_SLOW_ENTITY_TABLE,OFF_FAR_ENTITY_TABLE };

uintptr_t Camera_VTableFunction[2] = {  };
uintptr_t ManVisualState_VTableFunction[2] = {  };


void __stdcall main_cheat_handler()
{
	//SPOOF_FUNC;

	auto* window = ImGui::GetBackgroundDrawList();
	ImGuiIO& io = ImGui::GetIO();
	if (Config::Aim::ShowAimFov)
		window->AddCircle({ io.DisplaySize.x / 2, io.DisplaySize.y / 2 }, Config::Aim::AimFov, 0xffffffff, 50);

	if (Config::Aim::ShowCrossHair)
		window->AddCircleFilled(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), Config::Aim::CrossHairSize, 0xffffffff, 30);

	const auto pUWorld = GameMemory->RVM<uintptr_t>(globals.Base + OFF_WORLD);
	if (!pUWorld) return;
	auto LocalPlayer = Game::GetLocalPlayer(pUWorld);
	if (!LocalPlayer) return;


	

	if (Config::Feature::WorldCompass)
		DrawCompass(window, ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), Game::GetCameraAngles(Game::GetCamera()).yaw);

	if (Config::Feature::ShowLocalPlayerPosition)
	{
		auto LocalPlayerPosition = Game::GetObjectVisualState(LocalPlayer);
		std::string LocalPositionText = __("X->") + std::to_string(LocalPlayerPosition.x) + __(" Y->") + std::to_string(LocalPlayerPosition.y) + __(" Z->") + std::to_string(LocalPlayerPosition.z);
		ImGui::PushFont(globals.CompassFont);
		window->AddText(ImVec2((io.DisplaySize.x- ImGui::CalcTextSize(LocalPositionText.c_str()).x) / 2, 0), 0xffffffff, LocalPositionText.c_str());
		ImGui::PopFont();
	}



	if (Config::Feature::WorldLightCheat)
	{
		if (GameMemory->RVM<float>(pUWorld + OFF_EYE_ACCOM) != Config::Feature::WorldLight)
		{
			GameMemory->WVM<float>(pUWorld + OFF_EYE_ACCOM, Config::Feature::WorldLight);
		}
	}


	if (Config::Feature::LatitudeCheat)
	{
		if (GameMemory->RVM<float>(pUWorld + OFF_LATITUDE) != Config::Feature::Latitude)
		{
			GameMemory->WVM<float>(pUWorld + OFF_LATITUDE, Config::Feature::Latitude);
		}
	}

	if (Config::Feature::PerfectWeather)
	{
		auto Landscape = GameMemory->RVM<uintptr_t>(globals.Base + OFF_LANDSCAPE);
		if (Landscape)
		{
			auto WorldWeather = GameMemory->RVM<uintptr_t>(Landscape + OFF_WEATHER);
			if (WorldWeather)
			{
				auto Overcast = GameMemory->RVM<uintptr_t>(WorldWeather + OFF_WEATHER_OVERCAST);
				GameMemory->WVM<float>(Overcast + 0x10, 0);

				auto Fog = GameMemory->RVM<uintptr_t>(WorldWeather + OFF_WEATHER_FOG);
				GameMemory->WVM<float>(Fog + 0x10, 0.f);

				auto Rain = GameMemory->RVM<uintptr_t>(WorldWeather + OFF_WEATHER_RAIN);
				GameMemory->WVM<float>(Rain + 0x10, 0.f);

				auto Snowfall = GameMemory->RVM<uintptr_t>(WorldWeather + OFF_WEATHER_SNOWFALL);
				GameMemory->WVM<float>(Snowfall + 0x10, 0.f);
			}
		}
	}

	if (Config::Feature::FreeCamera)
	{

		auto Camera_VTable = reinterpret_cast<void**>(*(void**)(Game::GetCamera()));
		auto ManVisualState_VTable = reinterpret_cast<void**>(*(void**)(GameMemory->RVM<uintptr_t>(Game::GetLocalPlayer(pUWorld) + 0x1D0)));

		if (!Camera_VTableFunction[0])
		{
			Camera_VTableFunction[0] = (uintptr_t)Camera_VTable[2];
			Camera_VTableFunction[1] = (uintptr_t)Camera_VTable[6];

			ManVisualState_VTableFunction[0] = (uintptr_t)ManVisualState_VTable[2];
			ManVisualState_VTableFunction[1] = (uintptr_t)ManVisualState_VTable[6];
		
		}

		if (safe_call(GetAsyncKeyState)(Config::Feature::FreeCameraKey) & 0x8000)
		{



			DWORD OldProtection;
			safe_call(VirtualProtect)(&Camera_VTable[2], 8, PAGE_EXECUTE_READWRITE, &OldProtection);
			safe_call(VirtualProtect)(&Camera_VTable[6], 8, PAGE_EXECUTE_READWRITE, &OldProtection);
			safe_call(VirtualProtect)(&ManVisualState_VTable[2], 8, PAGE_EXECUTE_READWRITE, &OldProtection);
			safe_call(VirtualProtect)(&ManVisualState_VTable[6], 8, PAGE_EXECUTE_READWRITE, &OldProtection);

			if ((uintptr_t)Camera_VTable[2] == Camera_VTableFunction[0])
			{
				Camera_VTable[2] = (void*)Camera_VTableFunction[1];
				Camera_VTable[6] = (void*)Camera_VTableFunction[0];

				ManVisualState_VTable[2] = (void*)ManVisualState_VTableFunction[1];
				ManVisualState_VTable[6] = (void*)ManVisualState_VTableFunction[0];
			}
			else
			{
				Camera_VTable[2] = (void*)Camera_VTableFunction[0];
				Camera_VTable[6] = (void*)Camera_VTableFunction[1];

				ManVisualState_VTable[2] = (void*)ManVisualState_VTableFunction[0];
				ManVisualState_VTable[6] = (void*)ManVisualState_VTableFunction[1];
				Config::Feature::ClearGrass = false;
			}
			safe_call(VirtualProtect)(&Camera_VTable[2], 8, OldProtection, &OldProtection);
			safe_call(VirtualProtect)(&Camera_VTable[6], 8, OldProtection, &OldProtection);
			safe_call(VirtualProtect)(&ManVisualState_VTable[2], 8, OldProtection, &OldProtection);
			safe_call(VirtualProtect)(&ManVisualState_VTable[6], 8, OldProtection, &OldProtection);
			safe_call(Sleep)(300);
		}
		if ((uintptr_t)Camera_VTable[2] == Camera_VTableFunction[1])
		{
			Config::Feature::ClearGrass = true;
			auto CameraVec = Game::GetInvertedViewTranslation(Game::GetCamera());
			auto Angle = Game::GetCameraAngles(Game::GetCamera());
			HandleCameraMovement(CameraVec, Angle.yaw, Angle.pitch, Config::Feature::FreeCameraSpeed);
			GameMemory->WVM<Vector3>(Game::GetCamera() + 0x2C, CameraVec);
			GameMemory->WVM<Vector3>(GameMemory->RVM<uintptr_t>(Game::GetLocalPlayer(pUWorld) + 0x1d0) + 0x2C, CameraVec);
		}

	}

	//if (Config::Feature::FreeCamera)
	//{
	//	byte nop[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
	//	if (safe_call(RtlCompareMemory)((void*)(globals.Base + OFF_REMOTE_DROP), &nop, 18) == 18)
	//	{
	//		//GameMemory->WVM<Vector3>(GameMemory->RVM<uintptr_t>(Game::GetLocalPlayer(pUWorld) + 0x1d0) + 0x2C, Config::Feature::RemoteDropVec);
	//		auto CameraVec = Game::GetInvertedViewTranslation(Game::GetCamera());
	//		auto Angle = Game::GetCameraAngles(Game::GetCamera());
	//		HandleCameraMovement(CameraVec, Angle.yaw, Angle.pitch, Config::Feature::FreeCameraSpeed);
	//		GameMemory->WVM<Vector3>(Game::GetCamera() + 0x2C, CameraVec);
	//		GameMemory->WVM<Vector3>(GameMemory->RVM<uintptr_t>(Game::GetLocalPlayer(pUWorld) + 0x1d0) + 0x2C, CameraVec);
	//	}
	//	else
	//	{
	//		DWORD old = {  };
	//		safe_call(VirtualProtect)((void*)(globals.Base + OFF_REMOTE_DROP), 18, 64, &old);
	//		safe_call(memcpy)((void*)(globals.Base + OFF_REMOTE_DROP), &nop, sizeof(nop));
	//		safe_call(VirtualProtect)((void*)(globals.Base + OFF_REMOTE_DROP), 18, old, NULL);
	//	}
	//}
	//else
	//{
	//	byte Original[] = { 0x8B,0x42,0x24,0x89,0x41,0x24,0x8B,0x42,0x28,0x89,0x41,0x28,0x8B,0x42,0x2C,0x89,0x41,0x2C };
	//	if (safe_call(RtlCompareMemory)((void*)(globals.Base + OFF_REMOTE_DROP), &Original, 18) != 18)
	//	{
	//		DWORD old = {  };
	//		safe_call(VirtualProtect)((void*)(globals.Base + OFF_REMOTE_DROP), 18, 64, &old);
	//		safe_call(memcpy)((void*)(globals.Base + OFF_REMOTE_DROP), &Original, sizeof(Original));
	//		safe_call(VirtualProtect)((void*)(globals.Base + OFF_REMOTE_DROP), 18, old, NULL);
	//	}
	//}




	if (Config::Feature::ClearGrass)
	{
		if (GameMemory->RVM<float>(pUWorld + OFF_NOGRASS) != 0)
		{
			GameMemory->WVM<float>(pUWorld + OFF_NOGRASS, 0);
		}
	}
	else
	{
		if (GameMemory->RVM<float>(pUWorld + OFF_NOGRASS) != 12.5)
		{
			GameMemory->WVM<float>(pUWorld + OFF_NOGRASS, 12.5);
		}
	}
	if (Config::Feature::ChangeFov)
	{
		GameMemory->WVM<float>(
			GameMemory->RVM<uintptr_t>(
				GameMemory->RVM<uintptr_t>(
					GameMemory->RVM<uintptr_t>(
						GameMemory->RVM<uintptr_t>(
							GameMemory->RVM<uintptr_t>(pUWorld + 0x28) + 0x298) + 0xA8) + 0x1E8) + 0x78) + 0x628, Config::Feature::GameFov);
	}

	if (Config::Feature::ShowWorldTime)
	{
		auto dayProgress = GameMemory->RVM<float>(globals.Base + OFF_WORLD_TIME1);
		float elapsedHours = dayProgress * 24;
		int hours = static_cast<int>(elapsedHours);
		int minutes = static_cast<int>((elapsedHours - hours) * 60);
		std::string NowTime = (Config::Feature::EnglishMode ? (const char*)__(u8"WorldTime->") : (const char*)__(u8"世界时间->")) + std::to_string(hours) + (const char*)__(u8":") + std::to_string(minutes);
		ImGui::PushFont(globals.CompassFont);
		window->AddText({ 0,0 }, 0xffffffff, NowTime.c_str());
		ImGui::PopFont();

	}
	if (Config::Feature::SpeedHackCheat)
	{
		auto NowSpeed = GameMemory->RVM<int>(globals.Base + OFF_SPEED_HACK);
		if (NowSpeed != int(10000000 - ((Config::Feature::SpeedHack - 1) * 10000000)))
		{
			GameMemory->WVM<int>(globals.Base + OFF_SPEED_HACK, 10000000 - (Config::Feature::SpeedHack - 1) * 10000000);
		}
	}
	
	if (safe_call(GetAsyncKeyState)(Config::Feature::FightModeKey) & 0x8000)
	{
		Config::Feature::FightMode = !Config::Feature::FightMode;
		safe_call(Sleep)(500);
	}



	if (Config::ESP::Item::ShowItems && !Config::Feature::FightMode)
		IterateItems(pUWorld);



	IterateAll(pUWorld, EntityTable);



	if (Config::Feature::LootTeleport)
		LootTeleportFunction();

	if (Config::Aim::SilentAim)
		SilentAimFunction(pUWorld);
	

	EntityListVector.clear();

}


class Setting {
public:
	std::string EnglishName;
	std::string ChineseName;
	bool isBool;
	bool* boolValuePtr;
	float* floatValuePtr;
	int* intValuePtr;

	int intStrength = {};
	float floatStrength = {};
	float minValue = {}; 
	float maxValue = {}; 
	std::vector<Setting> subSettings;
	bool isExpanded;
	
	int SleepStrength = {  };


	Setting(std::string English, std::string Chinese, bool* externalBool)
		: EnglishName(English), ChineseName(Chinese), isBool(true), boolValuePtr(externalBool), floatValuePtr(nullptr), intValuePtr(nullptr), isExpanded(*externalBool) {
		if (*externalBool) {
			isExpanded = true;
		}
	}


	Setting(std::string English, std::string Chinese, float* externalFloat, float SliderStrength, float minVal, float maxVal,int SleepVal)
		: EnglishName(English), ChineseName(Chinese), isBool(false), boolValuePtr(nullptr), floatValuePtr(externalFloat), intValuePtr(nullptr),
		intStrength(0), floatStrength(SliderStrength), minValue(minVal), maxValue(maxVal), isExpanded(false),SleepStrength(SleepVal) {}

	
	Setting(std::string English, std::string Chinese, int* externalInt, int SliderStrength, int minVal, int maxVal)
		: EnglishName(English), ChineseName(Chinese), isBool(false), boolValuePtr(nullptr), floatValuePtr(nullptr), intValuePtr(externalInt),
		intStrength(SliderStrength), floatStrength(0), minValue(static_cast<float>(minVal)), maxValue(static_cast<float>(maxVal)), isExpanded(false) {}


	void addSubSetting(const Setting& setting) {
		subSettings.push_back(setting);
		if (isBool && boolValuePtr && *boolValuePtr) {
			isExpanded = true;
		}
	}


	std::string getValueAsString() const {
		if (isBool && boolValuePtr) {
			return *boolValuePtr ? getLocalizedString(__("Enable"), (const char*)__(u8"开启")) : getLocalizedString(__("Disable"), (const char*)__(u8"关闭"));
		}
		else if (floatValuePtr) {
			return std::to_string(*floatValuePtr);
		}
		else if (intValuePtr) {
			return std::to_string(*intValuePtr);
		}
		return "";
	}

	// 切换布尔值
	void toggleBoolValue(bool value) {
		if (isBool && boolValuePtr) {
			*boolValuePtr = value;
			if (value) {
				isExpanded = true;
			}
		}
	}


	void modifyIntValue(int delta) {
		if (intValuePtr) {
			*intValuePtr = std::clamp(*intValuePtr + delta, static_cast<int>(minValue), static_cast<int>(maxValue));
		}
	}

	
	void modifyFloatValue(float delta) {
		if (floatValuePtr) {
			*floatValuePtr = std::clamp(*floatValuePtr + delta, minValue, maxValue);
		}
	}


	std::string getLocalizedName() const {
		return getLocalizedString(EnglishName, ChineseName);
	}

private:

	std::string getLocalizedString(const std::string& english, const std::string& chinese) const {
		return Config::Feature::EnglishMode ? english : chinese;
	}
};


class Category {
public:
	std::string EnglishName;
	std::string ChineseName;
	std::vector<Setting> settings;
	bool isExpanded;

	Category(std::string English, std::string Chinese) : EnglishName(English), ChineseName(Chinese), isExpanded(false) {}

	void addSetting(const Setting& setting) {
		settings.push_back(setting);
	}


	std::string getLocalizedName() const {
		return getLocalizedString(EnglishName, ChineseName);
	}

private:

	std::string getLocalizedString(const std::string& english, const std::string& chinese) const {
		return Config::Feature::EnglishMode ? english : chinese;
	}
};

class Menu {
public:
	std::vector<Category> categories;
	int selectedIndex;
	float baseX = 100.0f;
	float baseY = 50.0f;
	float spacingY = 20.f;
	float indentX = 50.f;

	Menu() : selectedIndex(0) {

		Category visibility(__("ESP"), (const char*)__(u8"透视"));

		Setting player(__("Player"), (const char*)__(u8"玩家"), &Config::ESP::Player::ShowPlayer);
		player.addSubSetting(Setting(__("MaxDistance"), (const char*)__(u8"最大距离"), &Config::ESP::Player::MaxPlayerDistance, 50, 0, 2000));
		player.addSubSetting(Setting(__("ShowSkeleton"), (const char*)__(u8"显示骨骼"), &Config::ESP::Player::ShowPlayerSkeleton));
		player.addSubSetting(Setting(__("ShowHealth"), (const char*)__(u8"显示血量"), &Config::ESP::Player::ShowPlayerHealth));
		player.addSubSetting(Setting(__("ShowBox"), (const char*)__(u8"显示方框"), &Config::ESP::Player::ShowPlayerBox));
		player.addSubSetting(Setting(__("ShowHands"), (const char*)__(u8"显示手持"), &Config::ESP::Player::ShowPlayerHands));
		player.addSubSetting(Setting(__("ShowDistance"), (const char*)__(u8"显示距离"), &Config::ESP::Player::ShowPlayerDistance));
		player.addSubSetting(Setting(__("ShowName"), (const char*)__(u8"显示名字"), &Config::ESP::Player::ShowPlayerName));
		player.addSubSetting(Setting(__("ShowSlaplines"), (const char*)__(u8"显示射线"), &Config::ESP::Player::ShowPlayerSlaplines));

		visibility.addSetting(player);

		Setting zombie(__("Zombie"), (const char*)__(u8"僵尸"), &Config::ESP::Zombie::ShowZombie);
		zombie.addSubSetting(Setting(__("MaxDistance"), (const char*)__(u8"最大距离"), &Config::ESP::Zombie::MaxZombieDistance, 50, 0, 2000));
		zombie.addSubSetting(Setting(__("ShowSkeleton"), (const char*)__(u8"显示骨骼"), &Config::ESP::Zombie::ShowZombieSkeleton));
		zombie.addSubSetting(Setting(__("ShowHealth"), (const char*)__(u8"显示血量"), &Config::ESP::Zombie::ShowZombieHealth));
		zombie.addSubSetting(Setting(__("ShowBox"), (const char*)__(u8"显示方框"), &Config::ESP::Zombie::ShowZombieBox));
		zombie.addSubSetting(Setting(__("ShowDistance"), (const char*)__(u8"显示距离"), &Config::ESP::Zombie::ShowZombieDistance));
		zombie.addSubSetting(Setting(__("ShowName"), (const char*)__(u8"显示名字"), &Config::ESP::Zombie::ShowZombieName));
		visibility.addSetting(zombie);

		Setting item(__("Item"), (const char*)__(u8"物品"), &Config::ESP::Item::ShowItems);
		item.addSubSetting(Setting(__("Weapon"), (const char*)__(u8"显示武器"), &Config::ESP::Item::ShowWeapon));
		item.addSubSetting(Setting(__("ProxyMagazines"), (const char*)__(u8"显示弹药"), &Config::ESP::Item::ShowProxyMagazines));
		item.addSubSetting(Setting(__("ItemPotics"), (const char*)__(u8"显示瞄具"), &Config::ESP::Item::ShowItemOptics));
		item.addSubSetting(Setting(__("Rodas"), (const char*)__(u8"显示车配"), &Config::ESP::Item::ShowRodas));
		item.addSubSetting(Setting(__("Clothing"), (const char*)__(u8"显示衣物"), &Config::ESP::Item::ShowClothing));
		item.addSubSetting(Setting(__("Others"), (const char*)__(u8"显示其他"), &Config::ESP::Item::ShowOthers));
		item.addSubSetting(Setting(__("Food"), (const char*)__(u8"显示食物"), &Config::ESP::Item::ShowFood));
		item.addSubSetting(Setting(__("Drink"), (const char*)__(u8"显示饮料"), &Config::ESP::Item::ShowDrink));
		item.addSubSetting(Setting(__("Medcine"), (const char*)__(u8"显示药品"), &Config::ESP::Item::ShowMedicine));
		item.addSubSetting(Setting(__("Containers"), (const char*)__(u8"显示容器"), &Config::ESP::Item::ShowContainers));
		item.addSubSetting(Setting(__("BackPack"), (const char*)__(u8"显示背包"), &Config::ESP::Item::ShowBackPack));
		item.addSubSetting(Setting(__("ItemInventory"), (const char*)__(u8"显示内部"), &Config::ESP::Item::ShowItemInventory));
		item.addSubSetting(Setting(__("ItemQualidade"), (const char*)__(u8"显示质量"), &Config::ESP::Item::ShowItemQualidade));
		item.addSubSetting(Setting(__("All"), (const char*)__(u8"显示全部"), &Config::ESP::Item::ShowAllItems));
		visibility.addSetting(item);

		Setting vehicle(__("Car"), (const char*)__(u8"车辆"), &Config::ESP::ShowCar);
		vehicle.addSubSetting(Setting(__("MaxDistance"), (const char*)__(u8"最大距离"), &Config::ESP::MaxCarDistance, 50, 0, 2000));
		visibility.addSetting(vehicle);

		Setting animal(__("Animal"), (const char*)__(u8"动物"), &Config::ESP::ShowAnimal);
		animal.addSubSetting(Setting(__("MaxDistance"), (const char*)__(u8"最大距离"), &Config::ESP::MaxAnimalDistance, 50, 0, 2000));
		visibility.addSetting(animal);

		Setting house(__("HouseBase"), (const char*)__(u8"地基"), &Config::ESP::ShowHouse);
		house.addSubSetting(Setting(__("MaxDistance"), (const char*)__(u8"最大距离"), &Config::ESP::MaxHouseDistance, 50, 0, 2000));
		visibility.addSetting(house);

		categories.push_back(visibility);


		Category aim(__("Aim"), (const char*)__(u8"瞄准"));
		aim.addSetting(Setting(__("MagicBullet"), (const char*)__(u8"魔法子弹"), &Config::Aim::SilentAim));
		aim.addSetting(Setting(__("ForceBullet"), (const char*)__(u8"千米追踪"), &Config::Aim::AmmoSpeedChange));
		aim.addSetting(Setting(__("ShowFov"), (const char*)__(u8"显示范围"), &Config::Aim::ShowAimFov));
		aim.addSetting(Setting(__("ShowCrossHair"), (const char*)__(u8"显示准心"), &Config::Aim::ShowCrossHair));
		aim.addSetting(Setting(__("AimKey"), (const char*)__(u8"瞄准热键"), &Config::Aim::AimKey, 1, 0, 254));
		aim.addSetting(Setting(__("AimBonePosition"), (const char*)__(u8"瞄准位置"), &Config::Aim::AimBonePosition, 1, 0, 154));
		aim.addSetting(Setting(__("MaxAimDistance"), (const char*)__(u8"最大距离"), &Config::Aim::MaxAimDistance, 50, 0, 2000));
		aim.addSetting(Setting(__("AimFov"), (const char*)__(u8"范围大小"), &Config::Aim::AimFov, 5, 0, 300,0));
		aim.addSetting(Setting(__("CrossHairSize"), (const char*)__(u8"准心大小"), &Config::Aim::CrossHairSize, 0.1f, 0, 5,-1));
		categories.push_back(aim);

		
		Category features(__("Features"), (const char*)__(u8"功能"));

		Setting remoteGrab(__("LootTeleport"), (const char*)__(u8"远程取物"), &Config::Feature::LootTeleport);
		remoteGrab.addSubSetting(Setting(__("HotKey"), (const char*)__(u8"取物热键"), &Config::Feature::LootTeleportKey, 1,0,254));

		Setting WorldLatitude(__("WorldLatitude"), (const char*)__(u8"游戏纬度"), &Config::Feature::LatitudeCheat);
		WorldLatitude.addSubSetting(Setting(__("Latitude"), (const char*)__(u8"纬度数值"), &Config::Feature::Latitude, 0.1f, -10.f, 10.f,-1));

		Setting worldLight(__("WorldLight"), (const char*)__(u8"游戏亮度"), &Config::Feature::WorldLightCheat);
		worldLight.addSubSetting(Setting(__("Light"), (const char*)__(u8"亮度数值"), &Config::Feature::WorldLight, 1.f, -50.f, 200.f,-1));


		Setting NoGrass(__("ClearGrass"), (const char*)__(u8"去除草地"), &Config::Feature::ClearGrass);


		Setting DebugCamera(__("FreeCamera"), (const char*)__(u8"灵魂出窍"), &Config::Feature::FreeCamera);
		DebugCamera.addSubSetting(Setting(__("CameraSpeed"), (const char*)__(u8"灵魂速度"), &Config::Feature::FreeCameraSpeed, 0.1, 0.1, 1000.f, -1));
		DebugCamera.addSubSetting(Setting(__("HotKey"), (const char*)__(u8"出窍热键"), &Config::Feature::FreeCameraKey, 1, 0, 254));
		Setting SpeedHack(__("CommunitySpeedHack"), (const char*)__(u8"人物加速"), &Config::Feature::SpeedHackCheat);
		SpeedHack.addSubSetting(Setting(__("CommunitySpeed"), (const char*)__(u8"人物速度"), &Config::Feature::SpeedHack, 0.001, 1, 2.f, -1));

		Setting changeFov(__("ChangeFov"), (const char*)__(u8"玩家视野"), &Config::Feature::ChangeFov);
		changeFov.addSubSetting(Setting(__("HotKey"), (const char*)__(u8"视野大小"), &Config::Feature::GameFov, 0.05, 0, 10, -1));

		Setting perfectWeather(__("PerfectWeather"), (const char*)__(u8"完美天气"), &Config::Feature::PerfectWeather);

		Setting worldCompass(__("GameCompass"), (const char*)__(u8"方向导航"), &Config::Feature::WorldCompass);

		Setting ShowLocalPosition(__("ShowLocalPosition"), (const char*)__(u8"显示坐标"), &Config::Feature::ShowLocalPlayerPosition);

		Setting ShowWorldTime(__("ShowWorldTime"), (const char*)__(u8"显示时间"), &Config::Feature::ShowWorldTime);

		Setting FightKey(__("FightModeKey"), (const char*)__(u8"战斗热键"), &Config::Feature::FightModeKey, 1, 0, 254);
		//Setting remoteDrop(__("RemoteDrop"), (const char*)__(u8"远程丢物"), &Config::Feature::RemoteDrop);
		//remoteDrop.addSubSetting(Setting(__("X"), (const char*)__(u8"X"), &Config::Feature::RemoteDropVec.x, 10, 0, 30000, 0));
		//remoteDrop.addSubSetting(Setting(__("Y"), (const char*)__(u8"Y"), &Config::Feature::RemoteDropVec.y, 10, 0, 500, 0));
		//remoteDrop.addSubSetting(Setting(__("Z"), (const char*)__(u8"Z"), &Config::Feature::RemoteDropVec.z, 10, 0, 30000, 0));

		Setting debugmode(__("DebugMode"), (const char*)__(u8"调试模式"), &Config::Feature::DebugMode);

		Setting ShowEnglish(__("EnglishMode"), __("EnglishMode"), &Config::Feature::EnglishMode);

		features.addSetting(worldLight);
		features.addSetting(WorldLatitude);
		features.addSetting(remoteGrab);
		features.addSetting(SpeedHack);
		features.addSetting(ShowWorldTime);
		features.addSetting(ShowLocalPosition);
		features.addSetting(NoGrass);
		features.addSetting(DebugCamera);
		features.addSetting(changeFov);
		features.addSetting(perfectWeather);
		features.addSetting(worldCompass);
		features.addSetting(FightKey);
		//features.addSetting(remoteDrop);
		features.addSetting(debugmode);
		features.addSetting(ShowEnglish);

		categories.push_back(features);
	}

	int calculateTotalItems() const {
		int count = 0;
		for (const auto& category : categories) {
			count++;  
			if (category.isExpanded) {
				for (const auto& setting : category.settings) {
					count++;  
					if (setting.isExpanded) {
						count += setting.subSettings.size();  
					}
				}
			}
		}
		return count;
	}

	bool getSelectedItem(int& selectedCategoryIndex, int& selectedSettingIndex, int& selectedSubSettingIndex) const {
		int currentIndex = 0;
		for (size_t i = 0; i < categories.size(); ++i) {
			if (currentIndex == selectedIndex) {
				selectedCategoryIndex = i;
				selectedSettingIndex = -1;
				selectedSubSettingIndex = -1;
				return true;
			}
			currentIndex++;

			if (categories[i].isExpanded) {
				for (size_t j = 0; j < categories[i].settings.size(); ++j) {
					if (currentIndex == selectedIndex) {
						selectedCategoryIndex = i;
						selectedSettingIndex = j;
						selectedSubSettingIndex = -1;
						return true;
					}
					currentIndex++;

					if (categories[i].settings[j].isExpanded) {
						for (size_t k = 0; k < categories[i].settings[j].subSettings.size(); ++k) {
							if (currentIndex == selectedIndex) {
								selectedCategoryIndex = i;
								selectedSettingIndex = j;
								selectedSubSettingIndex = k;
								return true;
							}
							currentIndex++;
						}
					}
				}
			}
		}
		return false;
	}

	void drawMenu(ImDrawList* drawList) {
		
		ImVec2 backgroundPos = ImVec2(baseX - 15, baseY - 20);

		
		float totalHeight = 40; 
		for (const auto& category : categories) {
			totalHeight += spacingY + 20.f; 
			if (category.isExpanded) {
				totalHeight += category.settings.size() * (spacingY + 20.f); 
				for (const auto& setting : category.settings) {
					if (setting.isExpanded) {
						totalHeight += setting.subSettings.size() * (spacingY + 20.f); 
					}
				}
			}
		}


		ImVec2 backgroundSize = ImVec2(400, totalHeight);
		drawList->AddRectFilled(backgroundPos, ImVec2(backgroundPos.x + backgroundSize.x, backgroundPos.y + backgroundSize.y),
			IM_COL32(30, 30, 30, 255)); 

		for (float i = 0; i < backgroundSize.y; i += 5) {
			int alpha = static_cast<int>(255 * (1 - (i / backgroundSize.y)));
			ImVec2 RectFilledTargetA = backgroundPos;
			ImVec2 RectFilledTargetB = backgroundPos;
			RectFilledTargetA.y += i;
			RectFilledTargetB.x += backgroundSize.x;
			RectFilledTargetB.y += i + 5;

			drawList->AddRectFilled(RectFilledTargetA, RectFilledTargetB, IM_COL32(0, 0, 0, alpha)); // 
		}

		float currentY = baseY;
		int currentIndex = 0;

		for (size_t i = 0; i < categories.size(); ++i) {
			float currentX = baseX;
			ImVec2 pos = ImVec2(currentX, currentY);

			float textWidth = ImGui::CalcTextSize(categories[i].getLocalizedName().c_str()).x;

			ImVec2 categoryRectStart = ImVec2(currentX - 10, currentY - 10);						
			ImVec2 categoryRectEnd = ImVec2(currentX + textWidth + 10, currentY + 15);				
			drawList->AddRectFilled(categoryRectStart, categoryRectEnd, IM_COL32(50, 50, 50, 200)); 

		
			drawList->AddText(ImVec2(currentX, currentY - 5), IM_COL32(255, 255, 255, 255), Config::Feature::EnglishMode ? categories[i].EnglishName.c_str() : categories[i].ChineseName.c_str());

			
			if (currentIndex == selectedIndex) {
				pos.x -= 20; 
				drawList->AddText(ImVec2(pos.x, currentY - 5), IM_COL32(0, 255, 0, 255), __(">"));  
			}
			currentIndex++;
			currentY += spacingY + 5.f; 

			if (categories[i].isExpanded) {
				for (size_t j = 0; j < categories[i].settings.size(); ++j) {
					pos = ImVec2(currentX + indentX, currentY);

					float settingTextWidth = ImGui::CalcTextSize((categories[i].settings[j].getLocalizedName() + __(": ") + categories[i].settings[j].getValueAsString()).c_str()).x;

					ImVec2 settingRectStart = ImVec2(currentX + indentX - 10, currentY - 10);					
					ImVec2 settingRectEnd = ImVec2(currentX + indentX + settingTextWidth + 10, currentY + 10);
					drawList->AddRectFilled(settingRectStart, settingRectEnd, IM_COL32(40, 40, 40, 200));		

				
					drawList->AddText(ImVec2(currentX + indentX, currentY - 5), IM_COL32(255, 255, 255, 255),
						(categories[i].settings[j].getLocalizedName() + ": " + categories[i].settings[j].getValueAsString()).c_str()); 

					
					if (currentIndex == selectedIndex) {
						drawList->AddText(ImVec2(currentX + indentX - 65, currentY - 5), IM_COL32(0, 255, 0, 255), __("------->")); 
					}
					currentIndex++;
					currentY += spacingY + 15.f; 

					if (categories[i].settings[j].isExpanded) {
						for (size_t k = 0; k < categories[i].settings[j].subSettings.size(); ++k) {
							pos = ImVec2(currentX + 2 * indentX, currentY);

						
							float subSettingTextWidth = ImGui::CalcTextSize((categories[i].settings[j].subSettings[k].getLocalizedName() + ": " + categories[i].settings[j].subSettings[k].getValueAsString()).c_str()).x;

							ImVec2 subSettingRectStart = ImVec2(currentX + 2 * indentX - 10, currentY - 10); 
							ImVec2 subSettingRectEnd = ImVec2(currentX + 2 * indentX + subSettingTextWidth + 10, currentY + 10);
							drawList->AddRectFilled(subSettingRectStart, subSettingRectEnd, IM_COL32(30, 30, 30, 200)); 

							drawList->AddText(ImVec2(currentX + 2 * indentX, currentY - 5), IM_COL32(255, 255, 255, 255),
								(categories[i].settings[j].subSettings[k].getLocalizedName() + __(": ") + categories[i].settings[j].subSettings[k].getValueAsString()).c_str()); 

							if (currentIndex == selectedIndex) {
								drawList->AddText(ImVec2(currentX + 2 * indentX - 115, currentY - 5), IM_COL32(0, 255, 0, 255), __("------------->")); 
							}
							currentIndex++;
							currentY += spacingY + 15.f; 
						}
					}
				}
			}
		}
	}

	void handleInput() {
		int totalItems = calculateTotalItems();
		if (safe_call(GetAsyncKeyState)(VK_UP) & 0x8000) {
			selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : totalItems - 1;
			safe_call(Sleep)(150);
		}
		if (safe_call(GetAsyncKeyState)(VK_DOWN) & 0x8000) {
			selectedIndex = (selectedIndex + 1) % totalItems;
			safe_call(Sleep)(150);
		}
		int selectedCategoryIndex, selectedSettingIndex, selectedSubSettingIndex;
		if (getSelectedItem(selectedCategoryIndex, selectedSettingIndex, selectedSubSettingIndex)) {
		
			if (safe_call(GetAsyncKeyState)(VK_LEFT) & 0x8000) {
				if (selectedSettingIndex == -1) {
					categories[selectedCategoryIndex].isExpanded = false;
				}
				else if (selectedSubSettingIndex == -1) {
					categories[selectedCategoryIndex].settings[selectedSettingIndex].isExpanded = false;
				}

				safe_call(Sleep)(50);
				
			}
			if (safe_call(GetAsyncKeyState)(VK_RIGHT) & 0x8000) {
				if (selectedSettingIndex == -1) {
					categories[selectedCategoryIndex].isExpanded = true;
				}
				else if (selectedSubSettingIndex == -1) {
					categories[selectedCategoryIndex].settings[selectedSettingIndex].isExpanded = true;
				}

				safe_call(Sleep)(50);
				
			}
		
			if (selectedSettingIndex != -1 && selectedSubSettingIndex == -1) {
				
				if (safe_call(GetAsyncKeyState)(VK_LEFT) & 0x8000) {
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].isBool) {
						categories[selectedCategoryIndex].settings[selectedSettingIndex].toggleBoolValue(false);
					}
					else {
					
						if (categories[selectedCategoryIndex].settings[selectedSettingIndex].intValuePtr) {
							categories[selectedCategoryIndex].settings[selectedSettingIndex].modifyIntValue(-categories[selectedCategoryIndex].settings[selectedSettingIndex].intStrength);
						}
						else if (categories[selectedCategoryIndex].settings[selectedSettingIndex].floatValuePtr) {
							categories[selectedCategoryIndex].settings[selectedSettingIndex].modifyFloatValue(-categories[selectedCategoryIndex].settings[selectedSettingIndex].floatStrength);
						}
					}
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength != -1)
					{
						safe_call(Sleep)(categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength);
					}
					else
					{
						safe_call(Sleep)(50);
					}
				}
				if (safe_call(GetAsyncKeyState)(VK_RIGHT) & 0x8000) {
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].isBool) {
						categories[selectedCategoryIndex].settings[selectedSettingIndex].toggleBoolValue(true);
					}
					else {

						if (categories[selectedCategoryIndex].settings[selectedSettingIndex].intValuePtr) {
							categories[selectedCategoryIndex].settings[selectedSettingIndex].modifyIntValue(categories[selectedCategoryIndex].settings[selectedSettingIndex].intStrength);
						}
						else if (categories[selectedCategoryIndex].settings[selectedSettingIndex].floatValuePtr) {
							categories[selectedCategoryIndex].settings[selectedSettingIndex].modifyFloatValue(categories[selectedCategoryIndex].settings[selectedSettingIndex].floatStrength);
						}
					}
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength != -1)
					{
						safe_call(Sleep)(categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength);
					}
					else
					{
						safe_call(Sleep)(50);
					}
				}
			}

			if (selectedSettingIndex != -1 && selectedSubSettingIndex != -1) {
		
				if (safe_call(GetAsyncKeyState)(VK_LEFT) & 0x8000) {
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].isBool) {
						categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].toggleBoolValue(false);
					}
					else {
						if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].intValuePtr)
						{
							categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].modifyIntValue(-categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].intStrength);
						}
						else if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].floatValuePtr)
						{
							categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].modifyFloatValue(-categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].floatStrength);
						}
					}
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength != -1)
					{
						safe_call(Sleep)(categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength);
					}
					else
					{
						safe_call(Sleep)(50);
					}
				}
				if (safe_call(GetAsyncKeyState)(VK_RIGHT) & 0x8000) {
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].isBool) {
						categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].toggleBoolValue(true);
					}
					else {
						if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].intValuePtr)
						{
							categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].modifyIntValue(categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].intStrength);
						}
						else if (categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].floatValuePtr)
						{
							categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].modifyFloatValue(categories[selectedCategoryIndex].settings[selectedSettingIndex].subSettings[selectedSubSettingIndex].floatStrength);
						}
					}
					if (categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength != -1)
					{
						safe_call(Sleep)(categories[selectedCategoryIndex].settings[selectedSettingIndex].SleepStrength);
					}
					else
					{
						safe_call(Sleep)(50);
					}
				}
			}
		}
	}



};



Menu menu;
void draw_abigsquare()
{
	auto* window = ImGui::GetBackgroundDrawList();
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	menu.drawMenu(drawList);
	menu.handleInput();

}



