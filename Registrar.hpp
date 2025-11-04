#pragma once

#include "HotelRoom.h"

#include <string>
#include <unordered_map>

class Registry 
{
protected:
	static std::unordered_map
		<std::string, std::unordered_map<std::string, int>> _roomTypes;
public:
	static std::unordered_map<std::string, std::unordered_map
		<std::string, int>>* GetRegistry()
	{
		static std::unordered_map<std::string, std::unordered_map
			<std::string, int>> _roomTypes;
		return &_roomTypes;
	}
	virtual ~Registry() = 0;
};

template <class RoomType>
class Registrar : Registry
{
public:
	static bool Register() {
		std::unordered_map<std::string, std::unordered_map<std::string, int>>*
			rt = GetRegistry();
		(*rt)[RoomType::RoomType()]["Rate"] = RoomType::Rate();
		(*rt)[RoomType::RoomType()]["FirstRoomNum"] = RoomType::FirstRoomNum();
		(*rt)[RoomType::RoomType()]["NumRooms"] = RoomType::NumRooms();
		return true;
	}
	virtual ~Registrar<RoomType>() = 0;
};