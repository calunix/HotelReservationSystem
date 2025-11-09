#pragma once

#include "Registrar.hpp"

#include <iostream>
#include <string>

/*
* The Curiously Recurring Template Pattern is used here to create multiple abstract 
* HotelRoom classes that can then self register during static member initialization
* phase of program startup. This enables two things:
*  1) the HotelReservationSystem class to remain mostly generic
*  2) easy addition of more HotelRoom types by simply adding new hotel rooms that
*     that inherit from the HotelRoom class template
*/

template <class RoomType>
class HotelRoom
{
protected:
	inline static unsigned short _rate{ };
	inline static unsigned short _firstRoomNum{ };
	inline static unsigned short _numRooms{ };
public:
	HotelRoom() = default;
	static unsigned short Rate() { return RoomType::_rate; }
	static unsigned short FirstRoomNum() { return RoomType::_firstRoomNum; }
	static unsigned short NumRooms() { return RoomType::_numRooms; }
};

class CourtyardRoom : public HotelRoom<CourtyardRoom>
{
protected:
	static const int _rate{ 125 };
	static const int _firstRoomNum{ 101 };
	static const int _numRooms{ 70 };
	static const bool _courtyardRegistered;
public:
	friend class HotelRoom<CourtyardRoom>;
	static std::string RoomType() { return "Courtyard Room"; }
};
const bool _courtyardRegistered{ Registrar<CourtyardRoom>::Register() };

class ScenicRoom : public HotelRoom<ScenicRoom>
{
protected:
	static const int _rate{ 145 };
	static const int _firstRoomNum{ 201 };
	static const int _numRooms{ 35 };
	static const bool _scenicRegistered;
public:
	friend class HotelRoom<ScenicRoom>;
	static std::string RoomType() { return "Scenic Room"; }
};
const bool _scenicRegistered{ Registrar<ScenicRoom>::Register() };

class DeluxeSuite : public HotelRoom<DeluxeSuite>
{
protected:
	static const int _rate{ 350 };
	static const int _firstRoomNum{ 236 };
	static const int _numRooms{ 15 };
	static const bool _deluxeRegistered;
public:
	friend class HotelRoom<DeluxeSuite>;
	static std::string RoomType() { return "Deluxe Suite"; }
};
const bool _deluxeRegistered{ Registrar<DeluxeSuite>::Register() };

class Penthouse : public HotelRoom<Penthouse>
{
protected:
	static const int _rate{ 1135 };
	static const int _firstRoomNum{ 301 };
	static const int _numRooms{ 2 };
	static const bool _penthouseRegistered;
public:
	friend class HotelRoom<Penthouse>;
	static std::string RoomType() { return "Penthouse"; }
};
const bool _penthouseRegistered{ Registrar<Penthouse>::Register() };