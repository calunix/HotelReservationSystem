#pragma once

#include "HotelRoom.hpp"

#include <unordered_map>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <memory>
#include <set>

struct Reservation
{
	int roomNumber;
	int rate;
	std::string name;
	std::string roomType;
};

class HotelReservationSystem
{
public:
	HotelReservationSystem();
	void Start(void);
private:
	const static unsigned short DISPLAY_WIDTH{ 81 };
	const static char SEPARATOR_CHAR{ '=' };
	const size_t NUM_ROOM_TYPES{ Registry::GetRegistry()->size() };

	const std::unordered_map<std::string, std::unordered_map
		<std::string, int>>* ROOM_TYPES{ Registry::GetRegistry() };
	const std::string RESERVATIONS_DIR_NAME{ "reservations" };

	std::vector<std::unique_ptr<Reservation>> _reservations{ };
	std::unordered_map<std::string, int> _numReserved{ };
	std::unordered_map<std::string, std::set<int>> _availableRooms{ };

	std::chrono::year_month_day _dateToday{ };
	std::chrono::year_month_day _dateLoaded{ };

	/*
	* Utility functions
	*/
	static std::string ToLower(const std::string&);
	static void PrintCentered(std::string);
	static void PrintCenteredOnLongest(const std::string[], size_t);
	static void PrintCenteredOnLongest(const std::vector<std::string>*);
	static void PrintSeparator(void);
	static void HideConsoleCursor(void);
	static void ShowConsoleCursor(void);
	static void LoadingIndicator(int, int = 750);
	static void SplitString
	    (const std::string&, char, std::vector<std::string>&);
	static void DeleteLines(int = 1);
	
	/*
	* Data management
	*/
	int ComputeDayLoadedRevenue(void);
	void UpdateRoomsAvailable(void);
	void UpdateNumReserved(void);
	void ReadReservationFile
	    (std::string, std::vector<std::unique_ptr<Reservation>>&);
	void WriteReservationFile(std::string);

	/*
	* Input prompting
	*/
	// rename PromptForInput to MainMenuPromptForInput
	static void PromptForInput(void);
	bool PromptLoadAndOverwrite(void);
	std::chrono::year_month_day UserInputDate(void);
	std::string UserSelectRoomType(std::map<std::string, std::string>*);

	/*
	* Displays, menus, and navigation
	*/
	void ChangeDateLoaded(void);
	void CreateReservationMenu(void);
	void DisplayApplicationTitle(void);
	void DisplayCommandOptions(void);
	void DisplayDatePane(void);
	void DisplayDetailedReport(void);
	void DisplayInventory(void);
	void DisplayReservationOptions(std::map<std::string, std::string>*);
	void DisplayStartScreen(void);
	void MainMenu(void);
	void MainMenuOrQuit(std::string);
};
