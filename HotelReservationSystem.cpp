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
#include <random>

#include "Windows.h"

using namespace std;
using namespace std::filesystem;
using namespace std::chrono;

HotelReservationSystem::HotelReservationSystem()
{
	int firstRoomNum{ };
	int numRooms{ };
	for (const auto& rt : *ROOM_TYPES) {
		// start with no reservations
		_numReserved[rt.first] = 0;
		// start with all rooms available
		_availableRooms[rt.first] = { };
		firstRoomNum = rt.second.at("FirstRoomNum");
		numRooms = rt.second.at("NumRooms");
		for (int i{ firstRoomNum }; i < firstRoomNum + numRooms; ++i) {
			_availableRooms[rt.first].insert(i);
		}
	}
	time_point now{ system_clock::now() };
	sys_days currDays = floor<days>(now);
	_dateToday = currDays;
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
	cout << "\n\n";

	year_month_day _dateInput{ UserInputDate() };
	_dateLoaded = _dateInput;
	string reservationFilename{ ".\\" + _reservationsDirName +
		"\\" + format("{:%Y-%m-%d}", _dateInput) + ".csv" };

	if (exists(reservationFilename)) {
		cout << "Reservations for "
			<< format("{:%m/%d/%Y}", _dateInput)
			<< " found\n";
		cout << "Would you like to load them? (y/n): ";
		string loadConfirmation{ };
		while (getline(cin, loadConfirmation)) {
			loadConfirmation = ToLower(loadConfirmation);
			if (loadConfirmation == "y" || loadConfirmation == "yes") {
				
				ReadReservationFile(reservationFilename);
				UpdateNumReserved();
				break;
			}
			else if (loadConfirmation == "n" || loadConfirmation == "no") {
				return;
			}
			else {
				DeleteLines(3);
				cout << "Reservations for "
					<< format("{:%m/%d/%Y}", _dateInput)
					<< " found\n";
				cout << "Would you like to load them? (y/n): ";
			}
		}
	}	
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
			DisplayDetailedReport();
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
		"4 - Detailed Reservations Report by Date",
		"Q - Quit" };
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

	DisplayReservationOptions(&options);
	string roomType{ UserSelectRoomType(&options) };
	bool selectionValid{ };

	string guestName{ };
	cout << "Enter guest name: ";
	getline(cin, guestName);
	MainMenuOrQuit(guestName);

	while (!selectionValid) {
		if (_numReserved.at(roomType) < ROOM_TYPES->at(roomType).at("NumRooms")) {
			_numReserved[roomType] += 1;
			selectionValid = !selectionValid;
		}
		else if (_numReserved.at(roomType) >=
			ROOM_TYPES->at(roomType).at("NumRooms"))
		{
			cout << "No " << roomType << " Available";
			this_thread::sleep_for(chrono::seconds(2));
			DeleteLines(3);
			roomType = UserSelectRoomType(&options);
			MainMenuOrQuit(roomType);
		}
	}

	// pick a random room number
	vector<int> roomNums{ _availableRooms.at(roomType).begin(),
		_availableRooms.at(roomType).end() };
	random_device rdev{ };
	mt19937 generator{ rdev() };
	uniform_int_distribution<> roomDistribution{ 0, (int)roomNums.size() - 1};
	int roomNum{ roomNums.at(roomDistribution(generator)) };
	_availableRooms[roomType].erase(roomNum);

	_reservations.push_back(make_unique<Reservation>
		(roomNum, ROOM_TYPES->at(roomType).at("Rate"), guestName, roomType));

	// separately write the reservation to file
	// all reservations must be immediately committed to file
	WriteReservationFile(".\\" + _reservationsDirName +
		"\\" + format("{:%Y-%m-%d}", _dateToday) + ".csv");

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
year_month_day HotelReservationSystem::UserInputDate(void)
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

void HotelReservationSystem::ReadReservationFile(string filename)
{
	ifstream reservationFile(filename);
	if (!reservationFile.is_open()) {
		// throw exception here instead
		cout << filename << "\n";
		cout << "File not opened\n";
		return;
	}

	vector<string> lines{ };
	string line{ };
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

// Writes the reservations stored in HotelReservationSystem::_reservations to a
// file named using the input string. For simplicity, existing file contents
// are always discarded completely.
void HotelReservationSystem::WriteReservationFile(string filename)
{
	ofstream reservationFile{ filename, ios::trunc };
	if (!reservationFile.is_open()) {
		// throw exception here
		cout << filename << " not opened for writing\n";
	}
	reservationFile << "Room Number, Rate, Name, Room Type\n";
	for (const auto& res : _reservations) {
		reservationFile << res->roomNumber << ","
			<< res->rate << ","
			<< res->name << ","
			<< res->roomType << "\n";
	}
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

void HotelReservationSystem::DisplayDetailedReport(void)
{
	// get a date from the user
	// check if a file exists for that date
	// if one does, load all the data to a variable that is local to this function
	// display all the data from the file

	return;
}

void HotelReservationSystem::MainMenuOrQuit(string userInput)
{
	if (ToLower(userInput) == "m") {
		MainMenu();
	}
	else if (ToLower(userInput) == "q") {
		exit(0);
	}
	return;
}