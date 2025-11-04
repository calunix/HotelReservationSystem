#pragma once

#include "HotelRoom.h"

#include <unordered_map>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <memory>

#include "Windows.h"

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
	const static unsigned short DISPLAY_WIDTH{ 80 };
	const static char SEPARATOR_CHAR{ '=' };
	const size_t NUM_ROOM_TYPES{ Registry::GetRegistry()->size() };

	const std::unordered_map<std::string, std::unordered_map
		<std::string, int>>* ROOM_TYPES{ Registry::GetRegistry() };
	const std::string _reservationsDirName{ "reservations" };

	std::vector<std::unique_ptr<Reservation>> _reservations{ };
	std::unordered_map<std::string, int> _numReserved{ };

	std::chrono::year_month_day _dateToday{ };
	std::chrono::year_month_day _dateLoaded{ };

	// can I break all or most of these into a graphics header
	// do the prompts need to be funtions or can they just be cout calls to const strings defined in the header
	static std::string ToLower(const std::string&);
	// rename PromptForInput to MainMenuPromptForInput
	static void PromptForInput(void);
	static void PrintCentered(std::string);
	static void PrintCenteredOnLongest(const std::string[], size_t);
	static void PrintCenteredOnLongest(const std::vector<std::string>*);
	static void PrintSeparator(void);
	static void HideConsoleCursor(void);
	static void ShowConsoleCursor(void);
	static void LoadingIndicator(int, int=750);
	static void SplitString(const std::string&, char, std::vector<std::string>&);
	static void DeleteLines(int=1);

	void ChangeDateLoaded(void);
	void DisplayStartScreen(void);
	void MainMenu(void);
	void DisplayApplicationTitle(void);
	void DisplayInventory(void);
	void DisplayCommandOptions(void);
	void CreateReservationMenu(void);
	void DisplayRevenue(void);
	std::chrono::year_month_day UserInputDate(void);
	void DisplayReservationOptions(std::map<std::string, std::string>*);
	std::string UserSelectRoomType(std::map<std::string, std::string>*);
	void DisplayDatePane(void);
	void ReadReservationFile(void);
	void WriteReservationFile(void);
	void UpdateNumReserved(void);
	
};
