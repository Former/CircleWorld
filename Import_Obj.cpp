#include "Import_Obj.h"
#include <fstream>
#include <sstream>
#include <algorithm>

static bool ReadAllFileToVectorString(std::vector<std::string>* a_FileData, const std::string& a_FilePath)
{
	std::ifstream in;
	try
	{
		in.open(a_FilePath.c_str());
		if (!in)
			return false;
			
		std::string tmp;
		while (true)
		{
			if (!(std::getline)(in, tmp))
				break;

			a_FileData->push_back(tmp);
		}
	} 
	catch (const std::ifstream::failure & e)
	{
		if (!in.eof())
		{
			std::string err = e.what();
			return false;
		}
	}
	return true;
}

static void Optimize(OBJ_Data* a_Objdata)
{
	std::vector<size_t> new_point_index(a_Objdata->m_Points.size());
	for (size_t i = 0; i < new_point_index.size(); ++i)
		new_point_index[i] = i;
		
	for (size_t i = 0; i < a_Objdata->m_Points.size(); ++i)
	{
		for (size_t j = i; j < a_Objdata->m_Points.size(); ++j)
		{
			if (i == j)
				continue;
			
			if (!(a_Objdata->m_Points[i] == a_Objdata->m_Points[j]))
				continue;
				
			a_Objdata->m_Points.erase(a_Objdata->m_Points.begin() + j);
			
			for (size_t k = 0; k < new_point_index.size(); ++k)
			{
				if (new_point_index[k] == j)
					new_point_index[k] = i;
					
				if (new_point_index[k] > j)
					new_point_index[k]--;
			}
		}
	}
	
	for (size_t i = 0; i < a_Objdata->m_Edges.size(); ++i)
	{
		std::pair<size_t, size_t>& cur_egde = a_Objdata->m_Edges[i];
		
		cur_egde.first = new_point_index[cur_egde.first];
		cur_egde.second = new_point_index[cur_egde.second];
	}
	
	std::vector<std::pair<size_t, size_t>>::iterator it;
	it = std::unique (a_Objdata->m_Edges.begin(), a_Objdata->m_Edges.end());
	a_Objdata->m_Edges.resize(std::distance(a_Objdata->m_Edges.begin(), it)); 
}

bool LoadObjFile(OBJ_Data* a_Objdata, const std::string& a_FileName)
{
	std::vector<std::string> lines;
	if (!ReadAllFileToVectorString(&lines, a_FileName))
		return false;
		
	for (size_t i = 0; i < lines.size(); ++i)
	{
		const std::string& line = lines[i];
		if (line.empty())
			continue;
			
		char type = line[0];
		
		std::string parse_line(line.begin() + 1, line.end());
		
		switch (type)
		{
		case 'v':
			{
				std::stringstream str(parse_line);
				CircleEngine::Point cur_point;
				str >> cur_point.x >> cur_point.y >> cur_point.z;
				a_Objdata->m_Points.push_back(cur_point);
			}
			break;
		case 'f':
			{
				std::stringstream str(parse_line);
				std::pair<size_t, size_t> cur_egde;
				str >> cur_egde.first >> cur_egde.second;
				cur_egde.first--;
				cur_egde.second--;
				a_Objdata->m_Edges.push_back(cur_egde);				
			}
			break;
		default:
			break;
		}
	}
	
	Optimize(a_Objdata);
	
	return true;
}
