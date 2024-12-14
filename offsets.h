#pragma once
#include <Windows.h>

#define OFF_TEXT				       0x10
#define OFF_LENGTH				       0x8
#define OFF_WORLD				       0x41B32A0  //2
#define OFF_NEAR_ENTITY_TABLE	       0xF48 //2
#define OFF_FAR_ENTITY_TABLE	       0x1090 //2
#define OFF_SLOW_ENTITY_TABLE	       0x2010 //2


#define OFF_BULLET					   0xE00 //World::BulletList  2

#define OFF_BULLETSIZE				   OFF_BULLET + 8




#define OFF_CLEANNAME_1 0x4F0
#define OFF_CLEANNAME_2 0xA8
#define OFF_CLEANNAME_3 0x88


#define OFF_WORLD_ITEMTABLE			   0x2060 //2
#define OFF_WORLD_ITEMTABLESIZE		   OFF_WORLD_ITEMTABLE +8
#define OFF_ITEM_INVENTORY			   0x660//2
#define OFF_ITEM_CARGOGRID			   0x148//2
#define OFF_ITEM_LIST				   0x38


#define OFF_NETWORK_MANAGER            0xF5F2B0                            //2-------48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 1D ?? ?? ?? ?? 84
#define OFF_NETWROK_CLIENT             0x50     
#define OFF_NETWORK_SCOREBOARD         0x18     
#define OFF_NETWORK_SCOREBOARD_SIZE    0x1c								   //OFF_NETWORK_SCOREBOARD+4 ?
#define OFF_NETWORK_SCOREBOARD_ID      0x30     
#define OFF_NETWORK_SCOREBOARD_NAME    0xF8     
#define OFF_NETWORK_ID                 0x6EC							   //2-------41 8B 9E ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8
									   

#define OFF_EYE_ACCOM 0x2974 //游戏亮度 SetEyeAccom函数偏移 2
#define OFF_LATITUDE 0x2978 //修改世界时间纬度 GetLatitude函数偏移 2
#define OFF_NOGRASS  0xC00  //除草 2





#define OFF_DAYZPLAYER 0x41AFFD8 //可能是DayZPlayer地址 2
#define OFF_PLAYER_INVENTORY 0x660//2
#define	OFF_PLAYER_INVENTORY_HANDS 0x1B0//2
#define OFF_PLAYER_HUMAN_TYPE 0x180//2
#define OFF_AMMO_TYPE1 0x6B8//2
#define OFF_AMMO_TYPE2 0x20//2
#define OFF_INIT_SPEED 0x364 //2





#define OFF_WORLD_TIME1 0x41BF220 //GetDayTime 2

#define OFF_SPEED_HACK 0xF44A80 //2





#define OFF_LANDSCAPE 0x41B6238 //GetWeather第一个偏移   2
#define OFF_WEATHER 0x7458//GetWeather第二个偏移         2


#define OFF_WEATHER_OVERCAST 0x10
#define OFF_WEATHER_FOG 0x18
#define OFF_WEATHER_RAIN 0x20
#define OFF_WEATHER_SNOWFALL 0x28






#define Function_GetBonePosition_OFF	0x8C51D0 //GetBonePositionWS   2
#define Function_GetBoneIndexByName_OFF 0x4739A0 //GetBoneIndexByName  2


















//#define OFF_skeletonPlayer	0x810 // 40 53 48 83 EC 20 48 8B 89 ? ? ? ? 48 8B DA E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8B 89 ? ? ? ? E9 ? ? ? ? CC CC CC CC
//#define OFF_skeletonZombie	0x680 // 48 8B 8B ?? ?? ?? ?? 48 8D 55 C0 40 32 FF




//AUTO_OFFSET(Magazine, BulletList, "\x48\x8D\xB9\x00\x00\x00\x00\x48\x8B\xDA\x48\x8B\x17", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);
//AUTO_OFFSET(Magazine, MagazineType, "\x4C\x8B\xB1\x00\x00\x00\x00\x32\xDB\x0F\x29\x74\x24\x00", "xxx????xxxxxx?", ".text", ScanType::MovReg, 0);
//AUTO_OFFSET(Magazine, AmmoCount, "\x8B\xA9\x00\x00\x00\x00\x4C\x89\x74\x24\x00", "xx????xxxx?", ".text", ScanType::MovRegSml, 0);
//
//AUTO_OFFSET(AmmoType, InitSpeed, "\x45\x0F\x2F\x8F\x00\x00\x00\x00\x0F\x83\x00\x00\x00\x00", "xxxx????xx????", ".text", ScanType::MovRegXmm, 0);
//AUTO_OFFSET(AmmoType, AirFriction, "\xF3\x45\x0F\x10\x87\x00\x00\x00\x00\x48\x8D\x55\xC7", "xxxxx????xxxx", ".text", ScanType::MovRegXmmLrg, 0);

//
//AUTO_OFFSET(DayZPlayer, Inventory, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x00\x00\x00\x00\xEB\x02", "xxx????xxxxx????xx", ".text", ScanType::MovReg, 0);
//
//AUTO_OFFSET(DayZPlayerInventory, Hands, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\xF8\x48\x85\xC9", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);