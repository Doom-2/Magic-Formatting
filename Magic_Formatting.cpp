
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <regex>
#include <Windows.h>
#include <conio.h>
using namespace std;

DWORD WINAPI CheckEscape(LPVOID lpParam) {
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		//sleep 
		Sleep(500);
	}
	exit(0);

}

// Checks for positive integers, throws an error othewise.
// Returns correct integer.
// Takes a single argument indicating that the mode selection is active
int cin_positive_int(bool mode_sel_is_active = false)
{
	int val;
	do {
		try {
			if (!(cin >> val)) throw 1;
			if (mode_sel_is_active && cin.good() && val > 6 && cin.peek() == '\n') throw 4;
			if (cin.good() && val <= 0 && cin.peek() == '\n') throw 2;
			if (cin.peek() != '\n') throw 3;
		}
		catch (int i) {
			switch (i)
			{
			case 1:
			case 3:
				cerr << "Некорректный ввод. Допускается только целое число.\n" << ">";
				cin.clear();
				break;
			case 2:
				cerr << "Некорректный ввод. Целое число должно быть положительным.\n" << ">";
				cin.clear();
				break;
			case 4:
				cerr << "Некорректный ввод. Введите число из диапазона 1-6.\n" << ">";
				cin.clear();
				break;
			default:
				break;
			}
			while (cin.get() != '\n');
			continue;
		}
		break;
	} while (true);
	while (cin.get() != '\n');
	return val;
}

//asks for user input
//checks for input of only 'y' or 'n'
//not used in the current version
char cin_char_y_n() {
	char ch;
	while (1)
	{
		cin >> ch;
		if (ch == 'y' || ch == 'n') break;
		else {
			while (cin.get() != '\n');
			cin.clear();
			cout << "Некорректный ввод. Введите y или n." << endl << ">";
		}
	}
	return ch;
}

//Counts the number of words in string
int numWords(string str)
{
	int counting = 0;
	string word;
	stringstream strm(str);

	while (strm >> word)
		++counting;
	return counting;
}

//Removes a word from the string and returns a copy of the target string
string RemoveWordFromLine(string& line, string word)
{
	// \s* : extra optional spaces at the start
	// \b : word boundary
	regex reg("\\s*\\b" + word + "\\b");
	return regex_replace(line, reg, "");
}

string tabWords(string str)
{
	string word;
	stringstream strm(str);
	stringstream strmOut;
	bool xIsFound{}, zIsFound{};
	int count{};

	while (strm >> word) {
		if ((word.starts_with("N") || word.starts_with(":")) && isdigit(word[1]) && word.length() < 7)
			word.append(7 - word.length(), ' ');
		else if (word.length() < 10) {
			word.append(10 - word.length(), ' ');
			word += "\t";
			if (word.starts_with("X")) {
				xIsFound = true;
				switch (count)
				{
				case 1:
					word = "\t\t\t\t\t" + word;
					break;
				case 2:
					word = "\t\t" + word;
					break;
				default:
					break;
				}
			}
				
			else if (word.starts_with("Z")) {
				zIsFound = true;
				switch (count)
				{
				case 1:
					word = "\t\t\t\t\t\t\t\t" + word;
					break;
				case 2:
					if (xIsFound)
						word = "\t" + word;
					else
						word = "\t\t\t\t\t" + word;
					break;
				case 3:
					word = "\t" + word;
				default:
					break;
				}
			}
			else if (word.starts_with("T")) {
				word = "\t\t\t" + word;
			}
			else if (word.starts_with("M") && xIsFound && count == 3 ) {
				word = "\t" + word;
			}
			else if (word.starts_with("F") && xIsFound && !zIsFound) {
				word = "\t" + word;
			}
		}
		else
			word += "\t";
		strmOut << word;
		count++;
	}
		
	return strmOut.str();
}

int main()
{
	CreateThread(NULL, 0, CheckEscape, NULL, 0, NULL);

	SetConsoleCP(1251); // set the win-cp 1251 code page in the input stream
	SetConsoleOutputCP(1251); // set the win-cp 1251 code page in the output stream

	const string TITLE = "**********************\n"
		"МАГИЧЕСКОЕ ФОРМАТИРОВАНИЕ\n"
		"**********************\n";

	const string DESCRIPTION = "Выводит каждое слово кадра, используя фиксированную ширину\n"
		"В результате все слова получаются в столбик для удобства чтения.\n\n";

	const string FILE_NAME_INPUT = "Введите полное имя файла, включая путь.\n"
		"Можно использовать механизм перетаскивания Drag-and-Drop\n"
		"Для выхода нажмите Esc\n\n>";

	ifstream file;
	ofstream outfile;
	stringstream sstream;
	string fname, line;
	int counter = 1, incr = 1, digit_num = 1;

	cout << TITLE;
	cout << DESCRIPTION;
	cout << FILE_NAME_INPUT;

	while (getline(cin, fname))
	{
		// Exclude double quotes from the string if they exist. Need to put correct string into open() function
		if (fname == "") { cout << "Имя файла не может быть пустым.\n" << ">"; continue; }
		if (fname.front() == '"') {
			fname.erase(0, 1); // erase the first character
			fname.erase(fname.size() - 1); // erase the last character
		}
		file.open(fname);
		if (!file)
		{
			cout << "Файл не найден." << endl << ">";
			continue;
		}

		vector<string> buff; // buffer for saving data line by line

		bool isG54Found{}, isRecalcBFound{}, isTraoriFound{}, isM6Found{}, isM08Found{};

		while (getline(file, line)) // to get you all the lines.
		{
			// First, remove leading, trailing and extra spaces from line
			line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");

						// Find line wich contains NxxxGxx/Mxx without space and remove frame number (Nxxx) from it
			try
			{
				if (line.starts_with("N") && isdigit(line[1]) && line.substr(line.find_first_not_of("0123456789", 1), 1) != " ")
					line = line.substr(line.find_first_not_of("0123456789", 1));
			}
			catch (out_of_range& ex)
			{
				continue;
			}
			// Skip line if it consists only of frame number (Nxxx) or ;
			if (line.starts_with("N") && isdigit(line[1]) && numWords(line) == 1) continue;
			if (line.starts_with(";") && line.size() == 1) continue;
			// Skip line if it's empty
			if (line.empty()) { buff.push_back(line); continue; }
			// Remove Nxxxx and following space from each line if it exists
			if (line.starts_with("N") && isdigit(line[1])) line = line.substr(line.find_first_of(" \t") + 1);
			// Skip line if it's a label eg. 'LABEL1:', 'NO_MOVE:', etc.)
			if (line.ends_with(":")) { buff.push_back(line); continue; }
			// Skip line starting with '%', '(' or ';'
			if (line.starts_with("%") || line.starts_with("(") || line.starts_with(";")) { buff.push_back(line); continue; }
			
			if (line.starts_with(":")) {
				line = tabWords(line);
			}
			else {
				// Add frame number to the beginning of current line
				sstream << setfill('0') << setw(digit_num) << counter;
				line = "N" + sstream.str() + "   " + line;
				line = tabWords(line);
			}
			sstream.str(""); // clear stringstream
			buff.push_back(line); // push each modified line in vector object
			counter += incr;
		}

		file.close();
		// Open the same file and line by line write the data from vector into it
		outfile.open(fname);
		for (string line : buff)
			outfile << line << endl;
		outfile.close();
		system("CLS"); // console cleanup
		cout << TITLE;
		cout << DESCRIPTION;
		cout << endl << "Файл " << fname << " успешно перезаписан" << endl;
		cout << string(fname.size() + 25, '^') << endl << endl;
		cout << FILE_NAME_INPUT;
		counter = 1;
	}
	return 0;
}