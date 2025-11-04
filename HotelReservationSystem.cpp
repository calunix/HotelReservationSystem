#include "HotelReservationSystem.h"

#include <fstream>
#include <unordered_map>
#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <sstream>
#include <vector>
#include <chrono>
#include <filesystem>
#include <format>
#include <memory>

#include "Windows.h"

using namespace std;
using namespace std::filesystem;
using namespace std::chrono;

HotelReservationSystem::HotelReservationSystem()
{
	for (const auto& rt : *ROOM_TYPES) {
		_numReserved[rt.first] = 0;
	}
	time_point now{ system_clock::now() };
	sys_days currDays = floor<days>(now);
	_dateToday = currDays;
	_dateLoaded = currDays;
}

void HotelReservationSystem::ChangeDateLoaded(void)
{
	DeleteLines(2);
	_dateLoaded = UserInputDate();

	// FIXME: logic to repopulate _reservations and _numReserved

	MainMenu();
}

void HotelReservationSystem::Start(void)
{	
	system("cls");

	//check if reservations directory exists, if not make it
	path reservationsDir{ _reservationsDirName };
	if (!is_directory(reservationsDir)) {
		create_directory(reservationsDir);
	}

	ReadReservationFile();
	UpdateNumReserved();

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdOut, &mode);
	SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	HideConsoleCursor();

	locale current_locale("");
	cout.imbue(current_locale);

	DisplayStartScreen();
}

void HotelReservationSystem::DisplayStartScreen(void)
{
	DisplayApplicationTitle();
	PrintSeparator();
	cout << "\n";
	PrintCentered("::::::::::::::::::'##::::'##:'########:::'######:::::::::::::::::");
	PrintCentered(":::::::::::::::::: ##:::: ##: ##.... ##:'##... ##::::::::::::::::");
	PrintCentered(":::::::::::::::::: ##:::: ##: ##:::: ##: ##:::..:::::::::::::::::");
	PrintCentered(":::::::::::::::::: #########: ########::. ######:::::::::::::::::");
	PrintCentered(":::::::::::::::::: ##.... ##: ##.. ##::::..... ##::::::::::::::::");
	PrintCentered(":::::::::::::::::: ##:::: ##: ##::. ##::'##::: ##::::::::::::::::");
	PrintCentered(":::::::::::::::::: ##:::: ##: ##:::. ##:. ######:::::::::::::::::");
	PrintCentered("::::::::::::::::::..:::::..::..:::::..:::......::::::::::::::::::");
	cout << "\n";
	PrintSeparator();

	string startMessage{ "Starting Hotel Reservation System" };
	size_t startMessageLength{ startMessage.size() };
	cout << startMessage << "\n";
	PrintSeparator();

	// move cursor to the end of the start message
	string asq1{ "\x1b[" };
	string asq2{ to_string(startMessageLength) };
	string asq3{ "C" };
	cout << "\x1b[2A" << asq1 << asq2 << asq3 << flush;

	LoadingIndicator(2);
	ShowConsoleCursor();
	MainMenu();
}

void HotelReservationSystem::MainMenu(void)
{
	system("cls");
	ShowConsoleCursor();

	DisplayApplicationTitle();
	DisplayInventory();
	DisplayDatePane();
	DisplayCommandOptions();

	PromptForInput();
	string userInput{ };
	while (getline(cin, userInput)) {
		userInput = ToLower(userInput);
		if (userInput == "q") {
			exit(0);
		}
		else if (userInput == "1") {
			CreateReservationMenu();
		}
		else if (userInput == "2") {
			ChangeDateLoaded();
		}
		else if (userInput == "3") {
			DisplayRevenue();
		}
		else if (userInput == "4") {
			//DisplayDetailedReport();
		}
		else {
			cout << "Invalid selection";
			this_thread::sleep_for(seconds(2));
		}
		DeleteLines(2);
		PromptForInput();
	}
}

void HotelReservationSystem::HideConsoleCursor(void) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void HotelReservationSystem::ShowConsoleCursor(void) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);
	cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void HotelReservationSystem::LoadingIndicator(int numCycles, int pauseInterval) {
	for (int i{ 0 }; i < numCycles; ++i) {
		this_thread::sleep_for(chrono::milliseconds(pauseInterval));
		for (int i{ }; i < 3; ++i) {
			cout << ".";
			this_thread::sleep_for(chrono::milliseconds(pauseInterval));
		}
		cout << "\b\b\b   \b\b\b" << flush;
	}
}

string HotelReservationSystem::ToLower(const string& input) {
	string output{ };
	for (unsigned char c : input) { output += tolower(c); }
	return output;
}

void HotelReservationSystem::PromptForInput(void) {
	cout << "Enter menu option: ";
}

void HotelReservationSystem::PrintCentered(string str)
{
	size_t length{ str.length() };
	size_t totalPadding{ DISPLAY_WIDTH - length };
	size_t leftPadding{ totalPadding / 2 };
	cout << setfill(' ') << setw(leftPadding + length) << str << "\n";
}

void HotelReservationSystem::PrintCenteredOnLongest(const std::string strs[], size_t numStrings) {
	size_t maxLength{ };
	for (size_t i{ }; i < numStrings; ++i) {
		if (strs[i].size() > maxLength) {
			maxLength = strs[i].size();
		}
	}
	size_t totalPadding{ DISPLAY_WIDTH - maxLength };
	size_t leftPadding{ totalPadding / 2 };
	for (size_t i{ }; i < numStrings; ++i) {
		cout << setfill(' ')
			<< setw(leftPadding + strs[i].size())
			<< strs[i]
			<< "\n";
	}
}

void HotelReservationSystem::PrintCenteredOnLongest(const vector<string>* tokens) {
	size_t maxLength{ };
	for (string token : *tokens) {
		if (token.length() > maxLength) {
			maxLength = token.length();
		}
	}
	size_t totalPadding{ DISPLAY_WIDTH - maxLength };
	size_t leftPadding{ totalPadding / 2 };
	for (string token : *tokens) {
		cout << setfill(' ')
			<< setw(leftPadding + token.length())
			<< token
			<< "\n";
	}
}

void HotelReservationSystem::PrintSeparator(void)
{
	cout << setw(DISPLAY_WIDTH) << setfill(SEPARATOR_CHAR) << "\n";
	cout << setfill(' ');
}

void HotelReservationSystem::DisplayApplicationTitle(void)
{
	PrintSeparator();
	cout << "\n";
	PrintCentered(">>| Hotel Reservation System |<<");
	cout << "\n";
	PrintSeparator();
}

void HotelReservationSystem::DisplayInventory(void)
{
	PrintSeparator();
	cout << "\n";
	PrintCentered(">> Reservations Summary - " + 
		format("{:%m/%d/%Y}", _dateLoaded) + " <<" );
	PrintCentered("---------------------------------------");
	cout << "\n";
	cout << left << setw(20) << "Room Type"
		<< right << setw(10) << "Rate ($)"
		<< right << setw(15) << "Total Rooms"
		<< right << setw(20) << "Available Rooms" << "\n";
	cout << left << setw(20) << "---------"
		<< right << setw(10) << "--------"
		<< right << setw(15) << "-----------"
		<< right << setw(20) << "---------------" << "\n";

	for (const auto& room : *ROOM_TYPES) {
		cout << left << setw(20) << room.first
			<< right << setw(10) << room.second.at("Rate")
			<< right << setw(15) << room.second.at("NumRooms")
			<< right << setw(20) << room.second.at("NumRooms") - _numReserved.at(room.first) << "\n";
	}
	cout << "\n";
	PrintSeparator();
}

void HotelReservationSystem::DisplayCommandOptions(void)
{
	PrintSeparator();
	cout << "\n";
	PrintCentered(">> Main Menu <<");
	PrintCentered("---------------");
	string options[]{
		"1 - Create Reservation",
		"2 - Change Date Loaded",
		"3 - Compute Revenue for Date Loaded",
		"4 - Detailed Reservations Report",
		"Q - quit" };
	PrintCenteredOnLongest(options, sizeof(options) / sizeof(string));
	cout << "\n";
	PrintSeparator();
}

void HotelReservationSystem::CreateReservationMenu(void)
{
	system("cls");
	DisplayApplicationTitle();
	PrintSeparator();
	PrintCentered(">> Create Reservation <<");
	PrintCentered("------------------------");
	PrintCentered("- Enter m to exit to main menu");
	PrintCentered("- Enter q to exit program");
	PrintSeparator();

	map<string, string> options{  };
	int i{ 1 };
	for (auto& rt : *ROOM_TYPES) {
		options[to_string(i)] = rt.first;
		++i;
	}

	// get guest name input from user

	DisplayReservationOptions(&options);
	string roomType{ UserSelectRoomType(&options) };
	bool selectionValid{ };

	while (!selectionValid) {
		if (_numReserved.at(roomType) < ROOM_TYPES->at(roomType).at("NumRooms")) {
			_numReserved[roomType] += 1;
			selectionValid = !selectionValid;
		}
		else if (_numReserved.at(roomType) >= ROOM_TYPES->at(roomType).at("NumRooms")) {
			cout << "No " << roomType << " Available";
			this_thread::sleep_for(chrono::seconds(2));
			DeleteLines(3);
			roomType = UserSelectRoomType(&options);
		}
	}

	// display available room numbers, ask for a choice
	// could I accept any, then tie that to a randomize function that picks
	// any index in a vector of available room numbers

	// put together a unique pointer to Reservation

	// insert that reservation to reservations

	// separately write the reservation to file

	cout << "Reservation created\n";
	this_thread::sleep_for(seconds(4));
	MainMenu();
}

void HotelReservationSystem::DisplayRevenue(void)
{
	int revenue{ };
	HideConsoleCursor();
	for (const auto& room : *ROOM_TYPES) {
		revenue += _numReserved.at(room.first) * room.second.at("Rate");
	}
	locale current_locale("");
	cout.imbue(current_locale);
	cout << "Total revenue: $" << revenue;
	this_thread::sleep_for(chrono::seconds(5));
	ShowConsoleCursor();
}

// Returns a std::chrono::year_month_day object. Validates the date provided 
// by the user before returning. While loop taking user input does not break 
// until a valid date is provided.
chrono::year_month_day HotelReservationSystem::UserInputDate(void)
{
	string userInput{ };
	string datePrompt{ "Enter date as MM/DD/YYYY: " };
	istringstream ss{ };
	year_month_day ymd{ };
	cout << datePrompt;

	while (getline(cin, userInput)) {
		if (ToLower(userInput) == "q") {
			exit(0);
		}
		if (ToLower(userInput) == "m") {
			MainMenu();
		}
		ss = istringstream(userInput);
		ss >> parse("%m/%d/%Y", ymd);
		if (ymd.ok()) {
			break;
		}		
		cout << "Invalid date provided";
		this_thread::sleep_for(seconds(3));
		DeleteLines(2);
		cout << datePrompt;
	}
	return ymd;
}

void HotelReservationSystem::DisplayReservationOptions
	(map<string, string>* options)
{	
	vector<string> options_flat{ };
	for (auto& option : *options) {
		options_flat.push_back(option.first + " - " + option.second);
	}
	PrintCentered(">> Room Selection <<");
	PrintCenteredOnLongest(&options_flat);
	PrintSeparator();
}

string HotelReservationSystem::UserSelectRoomType
	(map<string, string>* options)
{
	string userInput{ "0" };
	while (userInput < options->begin()->first ||
		userInput > (--options->end())->first)
	{
		cout << "Enter corresponding number to select a room type: ";
		getline(cin, userInput);
		if (ToLower(userInput) == "q") {
			exit(0);
		}
		if (ToLower(userInput) == "m") {
			MainMenu();
		}
		if (userInput < options->begin()->first ||
			userInput >(--options->end())->first)
		{
			cout << "Invalid selection";
			this_thread::sleep_for(chrono::seconds(3));
			DeleteLines(2);
		}
	}
	userInput = options->at(userInput);
	cout << "Room type selected: " << userInput << "\n";
	return userInput;
}

// can probably remove this, but save somewhere for future use
void HotelReservationSystem::SplitString
	(const string& str, char delimiter, vector<string>& tokens)
{
	stringstream s{ str };
	string token;
	while (getline(s, token, delimiter)) {
		tokens.push_back(token);
	}
}

void HotelReservationSystem::DisplayDatePane(void)
{
	PrintCentered("Today's Date: " +
		format("{:%m/%d/%Y}", _dateToday) +
		"      |      " +
		"Date Loaded: " +
		format("{:%m/%d/%Y}", _dateLoaded)
	);
}

void HotelReservationSystem::DeleteLines(int numToDelete)
{
	// consider making ascii sequences vars so they're better understood
	cout << "\r\x1b[2K" << flush;
	while (numToDelete > 1) {
		cout << "\x1b[1A" << "\r\x1b[2K" << flush;
		--numToDelete;
	}
}

void HotelReservationSystem::ReadReservationFile(void)
{
	string reservationFilename{ ".\\" + _reservationsDirName +
		"\\" + format("{:%Y-%m-%d}", _dateLoaded) + ".csv"};
	std::ifstream reservationFile(reservationFilename);

	vector<string> lines{ };
	string line{ };
	if (!reservationFile.is_open()) {
		cout << reservationFilename << "\n";
		cout << "File not opened\n";
		return;
	}	
	while (getline(reservationFile, line)) {
		lines.push_back(line);
	}
	reservationFile.close();

	istringstream ss{ };
	int roomNumber{ };
	int rate{ };
	string name{ };
	string roomType{ };
	char delimiter{ ',' };
	unique_ptr<Reservation> reservation{ };
	vector<string> tokens{ };
	_reservations.clear();
	for (const string line : lines) {
		SplitString(line, delimiter, tokens);
		roomNumber = stoi(tokens.at(0));
		rate = stoi(tokens.at(1));
		name = tokens.at(2);
		roomType = tokens.at(3);

		_reservations.push_back(make_unique<Reservation>
			(roomNumber, rate, name, roomType));
		tokens.clear();
	}
}

//int roomNumber;
//int rate;
//std::string name;
//std::string roomType;

void HotelReservationSystem::WriteReservationFile(void)
{
	// if the file doesn't exist, make it

	// append the reservation to the file
}

void HotelReservationSystem::UpdateNumReserved(void)
{
	for (auto& roomType : _numReserved) {
		roomType.second = 0;
	}
	for (int i{ }; i < _reservations.size(); ++i) {
		_numReserved[_reservations.at(i)->roomType] += 1;
	}
}