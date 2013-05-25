#pragma once

#include <vector>
#include "Engine/Point.h"

struct OBJ_Data
{
	std::vector<CircleEngine::Point> m_Points;
	std::vector<std::pair<size_t, size_t>> m_Edges;	
};

bool LoadObjFile(OBJ_Data* a_Objdata, const std::string& a_FileName);
