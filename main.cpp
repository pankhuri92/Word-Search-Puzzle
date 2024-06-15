#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <queue>
#include <conio.h>
#include <windows.h>
using namespace std;

const string COLOR_RED = "\033[31m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_MAGENTA = "\033[35m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_RESET = "\033[0m";
const string BOLD = "\033[1m";
const string LINE = "\033[4m";
const std::string WHITE_BG = "\033[107m";

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

vector<vector<char>> readWordGridFromFile(const string& filename)
{
    vector<vector<char>> grid;
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            vector<char> row;
            for (char c : line) {
                if(c != ' ')
                   row.push_back(c);
            }
            grid.push_back(row);
        }
        file.close();
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return grid;
}

vector<string> readWordsFromFile(const string& filename)
{
    vector<string> words;
    ifstream file(filename);
    if (file.is_open())
    {
        string word;
        while (file >> word)
        {
            words.push_back(word);
        }
        file.close();
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return words;
}


// Knuth-Morris-Pratt (KMP)
vector<int> computeLPS(string pattern)
{
    int m = pattern.length();
    vector<int> lps(m, 0);
    int len = 0;
    int i = 1;

    while (i < m)
    {
        if (pattern[i] == pattern[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else{
            if (len != 0)
                len = lps[len - 1];
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

unordered_map<string, pair<pair<int, int>, pair<int, int>>> searchword_kmp(const vector<vector<char>>& grid, const string& word)
{
    unordered_map<string, pair<pair<int, int>, pair<int, int>>> solutions;

    int rows = grid.size();
    int cols = grid[0].size();

    // Horizontal search
    for (int i = 0; i < rows; ++i)
    {
        string horizontal_row;
        for (int j = 0; j < cols; ++j)
        {
            horizontal_row.push_back(grid[i][j]);
        }

        vector<int> horizontal_lps = computeLPS(word);
        int m = word.length();
        int n = horizontal_row.length();
        int k = 0; //for row
        int l = 0; //for word

        while (k < n)
        {
            if (word[l] == horizontal_row[k])
            {
                k++;  l++;
            }

            if (l == m)
            {
                solutions[word] = {{i, k - m}, {i, k - 1}};
                break;
            }
            else if (k < n && word[l] != horizontal_row[k])
            {
                if (l != 0)
                    l = horizontal_lps[l - 1];
                else
                    k++;
            }
        }
    }

    // Vertical search
    for (int j = 0; j < cols; ++j)
    {
        string vertical_column;
        for (int i = 0; i < rows; ++i)
        {
            vertical_column.push_back(grid[i][j]);
        }

        vector<int> vertical_lps = computeLPS(word);
        int m = word.length();
        int n = vertical_column.length();
        int k = 0; //for column
        int l = 0; //for word

        while (k < n)
        {
            if (word[l] == vertical_column[k])
            {
                k++;  l++;
            }

            if (l == m)
            {
                solutions[word] = {{k - m, j}, {k - 1, j}};
                break;
            }
            else if (k < n && word[l] != vertical_column[k])
            {
                if (l != 0)
                    l = vertical_lps[l - 1];
                else
                    k++;
            }
        }
    }

    return solutions;
}

void displaysolutions_kmp(unordered_map<string, pair<pair<int, int>, pair<int, int>>>& solutions)
{
    int i = 7;
    gotoxy(115, 5);
    cout << "Solutions (KMP):" << endl;
    for (auto it = solutions.begin(); it != solutions.end(); ++it)
    {
        const auto& word = it->first;
        const auto& positions = it->second;
        auto start = positions.first;
        auto end = positions.second;
        gotoxy(115, i);
        cout << word << ": (" << start.first << "," << start.second << ") to (" << end.first << "," << end.second << ")";
        i += 2;
    }
}

//BACKTRACKING
bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

bool backtrack(vector<vector<char>>& grid, const string& word, int index, int x, int y, vector<vector<bool>>& visited, const vector<pair<int, int>>& directions, unordered_map<string, pair<pair<int, int>, pair<int, int>>>& solutions) {
    if (index == word.length()) {
        return true;
    }

    int rows = grid.size();
    int cols = grid[0].size();

    if (!isValid(x, y, rows, cols) || visited[x][y] || grid[x][y] != word[index]) {
        return false;
    }

    visited[x][y] = true;

    for (const auto& direction : directions) {
        int dx = direction.first;
        int dy = direction.second;
        int new_x = x;
        int new_y = y;
        int k;
        for (k = 1; k < word.length(); ++k) {
            new_x += dx;
            new_y += dy;
            if (!isValid(new_x, new_y, rows, cols) || visited[new_x][new_y] || grid[new_x][new_y] != word[k]) {
                break;
            }
        }
        if (k == word.length()) {
            solutions[word] = {{x, y}, {new_x, new_y}};
            return true;
        }
    }

    visited[x][y] = false;

    return false;
}
unordered_map<string, pair<pair<int, int>, pair<int, int>>> findwords_backtracking(vector<vector<char>>& grid, const vector<string>& words) {
    unordered_map<string, pair<pair<int, int>, pair<int, int>>> solutions;

    int rows = grid.size();
    int cols = grid[0].size();

    vector<pair<int, int>> directions = {
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0},
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1}
    };

    for (auto& word : words) {
        bool found = false;
        vector<vector<bool>> visited(rows, vector<bool>(cols, false));

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (backtrack(grid, word, 0, i, j, visited, directions, solutions)) {
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        if (found) {
        }
    }

    return solutions;
}
void displaysolutions_backtracking(const vector<vector<char>>& grid, const unordered_map<string, pair<pair<int, int>, pair<int, int>>>& solutions)
{
    int i = 7;
    gotoxy(115, 5);
    cout << "Solutions (Backtracking):" << endl;
    for (const auto& solution : solutions) {
        const auto& word = solution.first;
        const auto& positions = solution.second;
        const auto& start = positions.first;
        const auto& end = positions.second;
        gotoxy(115, i);
        cout << word << ": (" << start.first << "," << start.second << ") to (" << end.first << "," << end.second << ")";
        i += 2;
    }
}


//BFS word search
void searchWordBFS(const vector<vector<char>>& grid, const string& word, unordered_map<string, pair<pair<int, int>, pair<int, int>>>& solutions)
{
    int rows = grid.size();
    int cols = grid[0].size();

    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (grid[i][j] == word[0])
            {
                queue<pair<pair<int, int>, int>> q;
                q.push({{i, j}, 0});
                vector<vector<bool>> visited(rows, vector<bool>(cols, false));

                while (!q.empty())
                {
                    auto current = q.front();
                    q.pop();
                    int x = current.first.first;
                    int y = current.first.second;
                    int idx = current.second;

                    if (idx == word.size() - 1)
                    {
                        solutions[word] = {{i, j}, {x, y}};
                        return;
                    }

                    for (const auto& dir : directions)
                    {
                        int newX = x + dir.first;
                        int newY = y + dir.second;


                        if (newX >= 0 && newX < rows && newY >= 0 && newY < cols &&
                            !visited[newX][newY] && grid[newX][newY] == word[idx + 1])
                        {
                            q.push({{newX, newY}, idx + 1});
                            visited[newX][newY] = true;
                        }
                    }
                }
            }
        }
    }
}
void displaySolution(unordered_map<string, pair<pair<int, int>, pair<int, int>>>& solutions)
{
    int i = 7;
    gotoxy(115, 5);
    cout << "Solutions (BFS) " << endl;
    for (const auto& [word, positions] : solutions)
    {
        auto [start, end] = positions;
        gotoxy(115, i);
        cout << word << ": (" << start.first << "," << start.second << ") to (" << end.first << "," << end.second << ")";
        i += 2;
    }
}

void displayGrid(const vector<vector<char>>& grid)
{
    int i = 7;
    for (const auto& row : grid)
    {
        gotoxy(35,i);
        for (char cell : row)
        {
            cout << cell << " ";
        }
        cout << endl;
        i = i + 1;
    }
}

void displayWelcomeScreen()
{
    cout << COLOR_CYAN;
    gotoxy(55, 13);
    cout << "**********************************************";
    gotoxy(60, 17);
    cout << "Welcome to Word Search Puzzle Solver";
    gotoxy(55, 21);
    cout << "**********************************************";
    gotoxy(65, 25);
    cout << "Press Enter to continue...";
    cin.ignore();
    system("cls");
    cout << WHITE_BG;
}

int main()
{
    char choice;
    string gridFile, listFile;
    int themeChoice;
    int difficultyChoice;
    int algorithmChoice;
    displayWelcomeScreen();

    do {
        system("cls");
        gotoxy(55, 16);
        cout << COLOR_GREEN << LINE << "Menu:" << COLOR_RESET << WHITE_BG << endl;
        gotoxy(55, 19);
        cout << COLOR_BLUE;
        cout << "1. Select Theme" << endl;
        gotoxy(55, 22);
        cout << "2. Exit" << endl;
        gotoxy(55, 25);
        cout << "Enter your choice: ";
        cin >> choice;
        system("cls");

        switch (choice) {
            case '1': {
                gotoxy(55, 10);
                cout << COLOR_RED << LINE << "Select Theme:" << COLOR_RESET << WHITE_BG << endl;
                gotoxy(55, 13);
                cout << COLOR_MAGENTA;
                cout << "1. Christmas" << endl;
                gotoxy(55, 16);
                cout << "2. Halloween" << endl;
                gotoxy(55, 19);
                cout << "3. Thanksgiving" << endl;
                gotoxy(55, 22);
                cout << "4. Sports" << endl;
                gotoxy(55, 25);
                cout << "Enter your choice: ";
                cin >> themeChoice;
                system("cls");

                switch (themeChoice) {
                    case 1:
                        gridFile = "christmas_grid.txt";
                        listFile = "christmas_words.txt";
                        break;
                    case 2:
                        gridFile = "halloween_grid.txt";
                        listFile = "halloween_words.txt";
                        break;
                    case 3:
                        gridFile = "thanksgiving_grid.txt";
                        listFile = "thanksgiving_words.txt";
                        break;
                    case 4:
                        gridFile = "sports_grid.txt";
                        listFile = "sports_words.txt";
                        break;
                    default:
                        gotoxy(55, 20);
                        cout << "Invalid theme choice!" << endl;
                        getch();
                        system("cls");
                        continue;
                }

                gotoxy(55, 13);
                cout << COLOR_YELLOW << LINE << "Select Difficulty Level:" << COLOR_RESET << WHITE_BG << endl;
                gotoxy(55, 16);
                cout << COLOR_CYAN;
                cout << "1. Easy" << endl;
                gotoxy(55, 19);
                cout << "2. Medium" << endl;
                gotoxy(55, 22);
                cout << "3. Hard" << endl;
                gotoxy(55, 25);
                cout << "Enter your choice: ";
                cin >> difficultyChoice;
                system("cls");

                switch (difficultyChoice) {
                    case 1:
                        gridFile = "easy_" + gridFile;
                        listFile = "easy_" + listFile;
                        break;
                    case 2:
                        gridFile = "medium_" + gridFile;
                        listFile = "medium_" + listFile;
                        break;
                    case 3:
                        gridFile = "hard_" + gridFile;
                        listFile = "hard_" + listFile;
                        break;
                    default:
                        gotoxy(50, 20);
                        cout << "Invalid difficulty level choice!" << endl;
                        getch();
                        system("cls");
                        continue;
                }

                gotoxy(55, 13);
                cout << COLOR_GREEN << LINE << "Select Algorithm:" << COLOR_RESET << WHITE_BG << endl;
                cout << COLOR_YELLOW;
                gotoxy(55, 16);
                cout << "1. Knuth-Morris-Pratt (KMP)" << endl;
                gotoxy(55, 19);
                cout << "2. Backtracking" << endl;
                gotoxy(55, 22);
                cout << "3. BFS" << endl;
                gotoxy(55, 25);
                cout << "Enter your choice: ";
                cin >> algorithmChoice;
                system("cls");

                if (!gridFile.empty() && !listFile.empty()) {
                    vector<vector<char>> wordGrid;
                    vector<string> wordsToSearch = readWordsFromFile(listFile);

                    wordGrid = readWordGridFromFile(gridFile);

                    gotoxy(35, 5);
                    cout << COLOR_MAGENTA << "Word Search Grid:" << endl;
                    int gridHeight = wordGrid.size();
                    int gridWidth = wordGrid[0].size();
                    displayGrid(wordGrid);

                    gotoxy(5, 5);
                    cout << COLOR_BLUE << "Words to Search:";
                    int i = 7;
                    for (const string& word : wordsToSearch) {
                        gotoxy(5, i);
                        cout << word << endl;
                        i += 2;
                    }
                    getch();
                    cout << COLOR_CYAN;
                    unordered_map<string, pair<pair<int, int>, pair<int, int>>> solutions;

                    unordered_map<string, pair<int, int>> userGuesses;

                    int j = 5;
                    gotoxy(75, j);
                    cout << "Make your guesses (row column):" << endl;
                    while (true) {
                        int row, col;
                        gotoxy(75, ++j);
                        cout << "Enter row (-1 to stop): ";
                        cin >> row;
                        if (row == -1) break;
                        gotoxy(75, ++j);
                        cout << "Enter column: ";
                        cin >> col;
                        if (row < 0 || row >= gridHeight || col < 0 || col >= gridWidth) {
                            cout << "Invalid coordinates! Try again." << endl;
                            continue;
                        }
                        string guessedWord;
                        gotoxy(75, ++j);
                        cout << "Enter guessed word: ";
                        cin >> guessedWord;
                        userGuesses[guessedWord] = {row, col};
                    }

                    if (!userGuesses.empty()) {
                        char reveal;
                        gotoxy(75, ++j);
                        cout << "Enter -2 to reveal solutions: ";
                        cin >> reveal;
                        if (reveal == -2) {
                            switch (algorithmChoice) {
                                case 1:
                                    displaysolutions_kmp(solutions);
                                    break;
                                case 2:
                                    displaysolutions_backtracking(wordGrid, solutions);
                                    break;
                                case 3:
                                    displaySolution(solutions);
                                    break;
                            }
                            getch();
                        } else {
                            gotoxy(75, ++j);
                            cout << "Invalid input. Exiting..." << endl;
                        }
                    } else {
                        gotoxy(75, ++j);
                        cout << "No guesses made. Exiting..." << endl;
                    }


                    auto start_time = chrono::high_resolution_clock::now();

                    switch(algorithmChoice) {
                        case 1:
                            for (const string& word : wordsToSearch) {
                                auto currentSolutions = searchword_kmp(wordGrid, word);
                                solutions.insert(currentSolutions.begin(), currentSolutions.end());

                            }
                            displaysolutions_kmp(solutions);
                            break;
                        case 2:
                            solutions = findwords_backtracking(wordGrid, wordsToSearch);
                            displaysolutions_backtracking(wordGrid, solutions);
                            break;
                        case 3:
                            for (const string& word : wordsToSearch) {
                                searchWordBFS(wordGrid, word, solutions);
                            }
                            displaySolution(solutions);
                            break;
                        default:
                            gotoxy(55, 20);
                            cout << "Invalid algorithm choice!" << endl;
                            system("cls");
                            continue;
                    }

                    auto end_time = chrono::high_resolution_clock::now();
                    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
                    gotoxy(95, 80);
                    cout << "Time taken: " << duration.count() << "ms" << endl;
                    getch();
                }
                break;
            }
            case '2':
                gotoxy(55, 25);
                cout << "Exiting..." << endl;
                system("cls");
                break;
            default:
                gotoxy(55, 20);
                cout << "Invalid choice! Please enter 1 or 2." << endl;
                getch();
                system("cls");
                break;
        }
    } while (choice != '2');

    return 0;
}
