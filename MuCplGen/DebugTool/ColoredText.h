#pragma once
#ifdef _WIN32
#include <Windows.h>
#else

#endif

//   0 = black			8 = gray
//   1 = blue			9 = tint blue
//   2 = green			A = tint green
//   3 = light green	B = tint light green
//   4 = red			C = tint red
//   5 = purple			D = tint purple
//   6 = yellow			E = tint yellow
//   7 = white			F = bright white

namespace MuCplGen::Debug
{
	enum class ConsoleBackgroundColor
	{
#ifdef _WIN32
		Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
		Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
		Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
		Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
		Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
		Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
		White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
		HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
		Black = 0,
#else
		Black = 9,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Purple = 5,
		Cyan = 6,
		White = 7,
		Gray = 7
#endif
	};

	enum class ConsoleForegroundColor
	{
#ifdef _WIN32
		Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
		Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Gray = FOREGROUND_INTENSITY,
		White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Black = 0,
#else
		Black = 0,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Purple = 5,
		Cyan = 6,
		White = 7,
		Gray = 7
#endif
	};


	inline void SetConsoleColor(
		ConsoleForegroundColor foreColor = ConsoleForegroundColor::White,
		ConsoleBackgroundColor backColor = ConsoleBackgroundColor::Black)
	{
#ifdef _WIN32
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, (WORD)foreColor | (WORD)backColor);
#endif
#ifdef __GNUC__
		if((int)foreColor >= 8 || (int)foreColor < 0) foreColor = ConsoleForegroundColor::White;
		if((int)backColor >= 8 || (int)backColor < 0) backColor = ConsoleBackgroundColor::Black;
		std::cout << "\e[" << 30 + (int)foreColor <<";" << 40 + (int)backColor << "m";
#endif
	}
}
