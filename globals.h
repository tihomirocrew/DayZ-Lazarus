#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <cstdint>

struct _globals
{

	HWND OverlayWnd;
	//HWND TargetWnd;
	int Width, Height;
	uint64_t Base;
	class ImFont* CompassFont;
	//uint64_t World;
};

typedef struct _player_t
{
	std::uint64_t EntityPtr;
	std::uint64_t TableEntry;
	int NetworkID;
} player_t;

typedef struct _item_t
{
	std::uint64_t ItemPtr;
	std::uint64_t ItemTable;
} item_t;

extern _globals globals;