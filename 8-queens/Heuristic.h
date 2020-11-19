#pragma once
#include "olcPixelGameEngine.h"


struct HeuristicValue {
	olc::vi2d position;
	int value;
};

struct Heuristic
{
	std::string name;

	Heuristic(const std::string& name) : name(name) {}
	Heuristic(std::string&& name) : name(std::move(name)) {}

	virtual std::vector<HeuristicValue> Function(int trgIndex, const std::vector<olc::vi2d>& positions) = 0;
};

