//#pragma once
//
//#include <string>
//#include <unordered_map>
//
//class HotelRoom
//{
//protected:
//	std::string _name;
//
//public: 
//	static std::unordered_map<std::string, std::unordered_map<std::string, int>>
//		_derived;
//	
//	std::string Name() const { return _name; }
//	void Name(std::string fullName) {
//		_name = fullName;
//	}
//
//	//virtual void Register() = 0;
//	//static void Register(HotelRoom& hotelRoom) {
//	//	hotelRoom.Register();
//	//}
//
//	//static void Register(HotelRoom& hotelRoom) { _derived.insert({
//	//	hotelRoom.RoomType(), {
//	//		{ "Rate", hotelRoom.Rate() },
//	//		{ "FirstRoomNum", hotelRoom.FirstRoomNum() },
//	//		{ "NumRooms", hotelRoom.NumRooms() }}
//	//	});
//	//}
//
//	std::string RoomType() const;
//	unsigned int Rate() const;
//	unsigned int FirstRoomNum() const;
//	unsigned int NumRooms() const;
//};
//
////std::unordered_map<std::string, std::unordered_map<std::string, int>> _derived;
//
//class CourtyardRoom : HotelRoom
//{
//public:
//	//static CourtyardRoom room;
//	//CourtyardRoom() {
//	//    _derived.insert({ room.RoomType(), {
//	//	        { "Rate", room.Rate() },
//	//	        { "FirstRoomNum", room.FirstRoomNum() },
//	//	        { "NumRooms", room.NumRooms() }}
//	//	});
//	//}
//	//CourtyardRoom();
//	static std::string _roomType;
//	static const unsigned int _rate{ 125 };
//	static const unsigned int _firstRoomNum{ 101 };
//	static const unsigned int _numRooms{ 70 };
//
//	std::string RoomType() const override { return "CourtyardRoom"; }
//	unsigned int Rate() const override { return _rate; }
//	unsigned int FirstRoomNum() const override { return _firstRoomNum; }
//	unsigned int NumRooms() const override { return _numRooms; }
//private:
//	
//
//};
//std::string CourtyardRoom::_roomType = "CourtyardRoom";
////CourtyardRoom CourtyardRoom::room;
//
//class ScenicRoom : HotelRoom
//{
//public:
//	unsigned int Rate() { return 145; }
//	unsigned int FirstRoomNum() { return 201; }
//	unsigned int NumRooms() { return 35; }
//};
//
//class DeluxeSuite : HotelRoom
//{
//public:
//	unsigned int Rate() { return 350; }
//	unsigned int FirstRoomNum() { return 236; }
//	unsigned int NumRooms() { return 15; }
//};
//
//class Penthouse : HotelRoom
//{
//public:
//	unsigned int Rate() { return 1135; }
//	unsigned int FirstRoomNum() { return 301; }
//	unsigned int NumRooms() { return 2; }
//};