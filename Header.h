
// IMPORTS
#include <iostream>
#include <conio.h>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <thread>
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

// CONFIGS
#include "config.h"
// ASSETS
#include "Assets.h"
#include "maze.h"

// global vars
int playerMoves = 0;
int obstacleHitCount = 0;
int collectibleCount = 0;
int coinCount = 0;
int metric;
int userScore;
string playerName;
string currMazeColor;

// flag
bool isFullScreen = false;
bool isPaused = false;

using namespace std;

// template stack
template <typename T>
class customStack
{
private:
    struct Node
    {
        T data;
        Node *next;

        Node(const T &value) : data(value), next(nullptr) {}
    };

    Node *topNode;

public:
    customStack() : topNode(nullptr) {}

    customStack(const customStack &other) : topNode(nullptr) // copy constructor
    {
        if (other.topNode != nullptr)
        {
            Node *otherNode = other.topNode;
            Node *newNode = new Node(otherNode->data);
            topNode = newNode;

            while (otherNode->next != nullptr)
            {
                otherNode = otherNode->next;
                newNode->next = new Node(otherNode->data);
                newNode = newNode->next;
            }
        }
    }

    void push(const T &value)
    {
        Node *newNode = new Node(value);
        newNode->next = topNode;
        topNode = newNode;
    }

    void pop()
    {
        if (topNode != nullptr)
        {
            Node *temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    T top() const
    {
        if (topNode != nullptr)
        {
            return topNode->data;
        }
        throw std::out_of_range("Stack is empty");
    }

    bool empty() const
    {
        return topNode == nullptr;
    }
};

// template queue
template <typename T>
class customQueue
{
private:
    struct Node
    {
        T data;
        Node *next;

        Node(const T &value) : data(value), next(nullptr) {}
    };

    Node *frontNode;
    Node *rearNode;

public:
    customQueue() : frontNode(nullptr), rearNode(nullptr) {}

    void push(const T &value)
    {
        Node *newNode = new Node(value);
        if (rearNode == nullptr)
        {
            frontNode = rearNode = newNode;
        }
        else
        {
            rearNode->next = newNode;
            rearNode = newNode;
        }
    }

    void pop()
    {
        if (frontNode != nullptr)
        {
            Node *temp = frontNode;
            frontNode = frontNode->next;
            delete temp;
            if (frontNode == nullptr)
            {
                rearNode = nullptr;
            }
        }
    }

    T front() const
    {
        if (frontNode != nullptr)
        {
            return frontNode->data;
        }
        throw std::out_of_range("Queue is empty");
    }

    bool empty() const
    {
        return frontNode == nullptr;
    }
};

// Usage
// customStack<int> mystack;
// customQueue<int> myqueue;

// intiaializers for stack and queue
template <typename T>
using myqueue = customQueue<T>;

template <typename T>
using mystack = customStack<T>;

// using _kbhit() to check if a key is pressed, return true if a key is pressed
bool isKeyPressed()
{
    /*if (!isPaused) {
        return _kbhit() != 0;
    }
    else {
        return false;
    }*/
    return _kbhit() != 0;
}

// pathNode class, has x and y coordinates, and pointers to its neighbors, and bools for conditions
class pathNode
{
public:
    int x, y;
    bool isWall;
    bool isObstacle;
    bool isCollectible;
    bool isCoin;
    // neighbors
    pathNode *up;
    pathNode *down;
    pathNode *left;
    pathNode *right;

    pathNode(int x, int y, bool isWall, bool isObstacle, bool isCollectible, bool isCoin)
        : x(x), y(y), isWall(isWall), isObstacle(isObstacle), isCollectible(isCollectible), isCoin(isCoin), up(nullptr), down(nullptr), left(nullptr), right(nullptr) {}
};

class graphNode
{
public:
    pathNode ***emptyPathNodes; // adjacency matrix
    int rows;
    int cols;
    bool **adjacencyMatrix; // true represents the path

    void createAdjacencyMatrix() // create adjacency matrix by copying the path from emptyPathNodes
    {
        adjacencyMatrix = new bool *[rows];
        for (int i = 0; i < rows; ++i)
        {
            adjacencyMatrix[i] = new bool[cols];
            for (int j = 0; j < cols; ++j)
            {
                adjacencyMatrix[i][j] = !emptyPathNodes[i][j]->isWall;
            }
        }
    }

    void connectAllNodes() // connecting all the empty path nodes to each other
    {
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                if (!emptyPathNodes[i][j]->isWall)
                {
                    connectNode(i, j, i - 1, j); // up
                    connectNode(i, j, i + 1, j); // down
                    connectNode(i, j, i, j - 1); // left
                    connectNode(i, j, i, j + 1); // right
                }
            }
        }
    }

    char mazeVec_A[31][21];

    bool generateMaze(int x, int y, int endX, int endY)
    {
        if (x == endX && y == endY)
        {
            return true;
        }

        mazeVec_A[x][y] = ' ';
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        srand(time(0));

        int order[] = {0, 1, 2, 3};
        for (int i = 0; i < 4; ++i)
        {
            int j = i + rand() % (4 - i);
            swap(order[i], order[j]);
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = x + dx[order[i]];
            int ny = y + dy[order[i]];

            if (isValidPosition(nx, ny) && generateMaze(nx, ny, endX, endY))
            {
                return true;
            }
        }

        mazeVec_A[x][y] = '#'; // walls

        return false;
    }

    void connectNode(int x1, int y1, int x2, int y2)
    // connecting nodes to each other by checking if the node is a wall or not
    {
        if (isValidPosition(x2, y2) && !emptyPathNodes[x2][y2]->isWall)
        {
            if (x1 - 1 == x2) // if the node is up
            {
                emptyPathNodes[x1][y1]->up = emptyPathNodes[x2][y2];
            }
            else
            {
                emptyPathNodes[x1][y1]->up = emptyPathNodes[x1][y1]->up;
            }

            if (x1 + 1 == x2) // if the node is down
            {
                emptyPathNodes[x1][y1]->down = emptyPathNodes[x2][y2];
            }
            else
            {
                emptyPathNodes[x1][y1]->down = emptyPathNodes[x1][y1]->down;
            }

            if (y1 - 1 == y2) // if the node is left
            {
                emptyPathNodes[x1][y1]->left = emptyPathNodes[x2][y2];
            }
            else
            {
                emptyPathNodes[x1][y1]->left = emptyPathNodes[x1][y1]->left;
            }

            if (y1 + 1 == y2) // if the node is right
            {
                emptyPathNodes[x1][y1]->right = emptyPathNodes[x2][y2];
            }
            else
            {
                emptyPathNodes[x1][y1]->right = emptyPathNodes[x1][y1]->right;
            }
        }
    }

    bool isValidPosition(int x, int y) const
    {
        return x >= 0 && x < rows && y >= 0 && y < cols; // check if the position is valid or not, if inside map or out
    }

public:
    graphNode(const char maze[][31], int numRows, int numCols)
    {
        rows = numRows;
        cols = numCols;

        // initialize emptyPathNodes, a 2D array of pathNode pointers
        emptyPathNodes = new pathNode **[rows];
        for (int i = 0; i < rows; ++i)
        {
            emptyPathNodes[i] = new pathNode *[cols];
            for (int j = 0; j < cols; ++j)
            {
                bool isWall = maze[i][j] == '#';
                emptyPathNodes[i][j] = new pathNode(i, j, isWall, false, false, false);
            }
        }

        connectAllNodes();
        createAdjacencyMatrix();
    }

    ~graphNode()
    {
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                delete emptyPathNodes[i][j];
            }
            delete[] emptyPathNodes[i];
        }
        delete[] emptyPathNodes;
    }

    pathNode *getNode(int x, int y)
    {
        if (isValidPosition(x, y))
        {
            return emptyPathNodes[x][y];
        }
        else
        {
            return nullptr;
        }
    }

    // marking nodes as obstacles, collectibles, coins

    void markNodeAsObstacle(int x, int y)
    {
        if (isValidPosition(x, y))
        {
            emptyPathNodes[x][y]->isObstacle = true;
        }
    }

    void markNodeAsCollectible(int x, int y)
    {
        if (isValidPosition(x, y))
        {
            emptyPathNodes[x][y]->isCollectible = true;
        }
    }

    void markNodeAsCoin(int x, int y)
    {
        if (isValidPosition(x, y))
        {
            emptyPathNodes[x][y]->isCoin = true;
        }
    }
};

// classes for obstacles, collectibles, coins
class obstacles
{
public:
    int x, y;
    obstacles(int x, int y) : x(x), y(y){};
};

class collectible
{
public:
    int x, y;
    collectible(int x, int y) : x(x), y(y){};
};

class coins
{
public:
    int x, y;
    coins *next;
    coins(int x, int y) : x(x), y(y){};
};

class LinkedList
{
private:
    coins *head;

public:
    LinkedList() : head(nullptr) {}

    void addNode(int x, int y)
    {
        coins *newNode = new coins(x, y);
        newNode->next = head;
        head = newNode;
    }

    void markNodesAsCoin(graphNode *graph)
    {
        coins *current = head;
        while (current != nullptr)
        {
            graph->markNodeAsCoin(current->x, current->y);
            current = current->next;
        }
    }

    ~LinkedList()
    {
        coins *current = head;
        while (current != nullptr)
        {
            coins *nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
    }
};

const int scoreCapacity = 50; // max number of scores to be saved in the heap

struct scoreStruct
{
    string playerName;
    int score;
};

class leaderboardHeap
{
private:
    scoreStruct scores[scoreCapacity];
    int size;

    void heapifyUp()
    {
        int currI = size - 1;
        while (currI > 0)
        {
            int parentI = (currI - 1) / 2;
            if (scores[currI].score > scores[parentI].score)
            {
                swap(scores[currI], scores[parentI]);
                currI = parentI;
            }
            else
            {
                break;
            }
        }
    }

public:
    leaderboardHeap()
    {
        size = 0;
    }
    void insert(const scoreStruct &score)
    {
        if (size < scoreCapacity)
        {
            scores[size++] = score;
            heapifyUp();
        }
    }
    void displayTopScores()
    {
        int count = 0;
        for (int i = 0; i < size; ++i)
        {
            if (count >= SHOW_N_SCORES) // display only top N SCORES
            {
                break;
            }
            cout << Assets::brightWhiteColor << setw(20) << left << scores[i].playerName
                 << setw(5) << right << scores[i].score;
            cout << endl;
            ++count;
        }
        cout << endl;
        cout << "Input 0 to return;" << endl;
        if (_getch() == '0')
        {
            return;
        }
    }
};

class mainGame
{
private:
    char mazeVec[21][31];
    graphNode *graph;
    pathNode *player;
    time_t startTime;
    time_t endTime;
    bool isTimeRunning;
    char lastKeyClicked;
    myqueue<obstacles> obstacleQueue;
    myqueue<collectible> collectibleQueue;
    LinkedList coins;
    int vehicleHealth;

    void updateTimer()
    {
        while (isTimeRunning)
        {
            this_thread::sleep_for(chrono::seconds(1));

            if (isTimeRunning)
            {
                printMaze();
            }
        }
    }

    void initializeMaze()
    {
        srand(static_cast<unsigned int>(time(0)));
        int ranChoice = rand() % 4 + 1;
        switch (ranChoice)
        {
        case 1:
            copy(&maze::mazeVecA[0][0], &maze::mazeVecA[0][0] + sizeof(maze::mazeVecA), &mazeVec[0][0]);
            break;
        case 2:
            copy(&maze::mazeVecB[0][0], &maze::mazeVecB[0][0] + sizeof(maze::mazeVecB), &mazeVec[0][0]);
            break;
        case 3:
            copy(&maze::mazeVecC[0][0], &maze::mazeVecC[0][0] + sizeof(maze::mazeVecC), &mazeVec[0][0]);
            break;
        case 4:
            copy(&maze::mazeVecD[0][0], &maze::mazeVecD[0][0] + sizeof(maze::mazeVecD), &mazeVec[0][0]);
            break;
        }
    }

    void printWinMessage()
    {
        system("cls");
        time_t elapsedTime = endTime - startTime;
        int minutes = static_cast<int>(elapsedTime / 60);
        int seconds = static_cast<int>(elapsedTime % 60);
        cout << setw(50) << Assets::brightGreenColor << R"(
         __   __  _______  __   __     _     _  ___  __    _  __  
        |  | |  ||       ||  | |  |   | | _ | ||   ||  |  | ||  | 
        |  |_|  ||   _   ||  | |  |   | || || ||   ||   |_| ||  | 
        |       ||  | |  ||  |_|  |   |       ||   ||       ||  | 
        |_     _||  |_|  ||       |   |       ||   ||  _    ||__| 
          |   |  |       ||       |   |   _   ||   || | |   | __  
          |___|  |_______||_______|   |__| |__||___||_|  |__||__|     
)";
        cout << "Congratulations! You completed the map in ";
        cout << setfill('0') << setw(2) << minutes << " minutes and ";
        cout << setfill('0') << setw(2) << seconds << " seconds." << endl;
        system("pause");
    }

    void printLoseMessage()
    {
        system("cls");
        if (vehicleHealth <= 0)
        {
            cout << Assets::redColor << setw(50) << "GAME OVER! PLAYER DIED!\n";
        }
        else
        {
            cout << Assets::redColor << setw(50) << "GAME OVER! YOU EXITED WITHOUT COLLECTING ALL COLLECTIBLES!\n";
        }

        system("pause");
    }

    // handling input from user
    void processInput(char move)
    {
        player->isWall = false;

        switch (move)
        {

            // ENSURING NOT CASE SENSITIVE INPUTS
        case 'w':
            lastKeyClicked = 'w';
            movePlayer(player, -1, 0); // move up
            break;
        case 'W':
            lastKeyClicked = 'W';
            movePlayer(player, -1, 0); // move up
            break;
        case 's':
            lastKeyClicked = 's';
            movePlayer(player, 1, 0); // move down
            break;
        case 'S':
            lastKeyClicked = 'S';
            movePlayer(player, 1, 0); // move down
            break;
        case 'a':
            lastKeyClicked = 'a';
            movePlayer(player, 0, -1); // move left
            break;
        case 'A':
            lastKeyClicked = 'A';
            movePlayer(player, 0, -1); // move left
            break;
        case 'd':
            lastKeyClicked = 'd';
            movePlayer(player, 0, 1); // move right
            break;
        case 'D':
            lastKeyClicked = 'D';
            movePlayer(player, 0, 1); // move right
            break;
        case 'P':
            isPaused = true;
            break;
        case 'R':
            isPaused = false;
            break;
        case 'p':
            isPaused = true;
            break;
        case 'r':
            isPaused = false;
            break;
        }
    }

    void movePlayer(pathNode *currentNode, int moveX, int moveY)
    {
        int newX = currentNode->x + moveX; // new x and y coordinates
        int newY = currentNode->y + moveY;

        if (graph->isValidPosition(newX, newY))
        {
            pathNode *moveNode = graph->getNode(newX, newY);

            if (moveNode && !moveNode->isWall)
            {
                player = moveNode;

                if (moveNode->isObstacle)
                {
                    vehicleHealth -= 10; // if obstacle is hit, decrease health by 10
                    obstacleHitCount++;  // if obstacle is hit, increase obstacle hit count by 1
                }

                if (moveNode->isCollectible)
                {
                    collectibleCount++;              // if collectible is collected, increase collectible count by 1
                    moveNode->isCollectible = false; // if collectible is collected, remove collectible from map
                }

                if (moveNode->isCoin)
                {
                    coinCount++;
                    moveNode->isCoin = false; // if coin is collected, remove coin from map
                }
            }
        }
    }

    // generating obstacles, collectibles, coins
    void generateObstacles(const char maze[][31], myqueue<obstacles> &obstacleQueue, graphNode *graph, int rows, int cols, int numObstacles)
    {
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < numObstacles; ++i)
        {
            int randX, randY;
            do
            {
                randX = rand() % rows;
                randY = rand() % cols;
            } while (mazeVec[randX][randY] != ' ');

            mazeVec[randX][randY] = 'X';
            // graph->markNodeAsObstacle(randX, randY);
            obstacleQueue.push(obstacles(randX, randY));
        }
        while (!obstacleQueue.empty())
        {
            obstacles currentObstacle = obstacleQueue.front();
            obstacleQueue.pop();
            graph->markNodeAsObstacle(currentObstacle.x, currentObstacle.y);
        }
    }

    void generateCollectible(const char maze[][31], myqueue<collectible> &collectibleQueue, graphNode *graph, int rows, int cols, int numCollectibles)
    {
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < numCollectibles; ++i)
        {
            int randX, randY;
            do
            {
                randX = rand() % rows;
                randY = rand() % cols;
            } while (mazeVec[randX][randY] != ' ');

            mazeVec[randX][randY] = 'P';
            // graph->markNodeAsCollectible(randX, randY);
            collectibleQueue.push(collectible(randX, randY));
        }
        while (!collectibleQueue.empty())
        {
            collectible currentCollectible = collectibleQueue.front();
            collectibleQueue.pop();
            graph->markNodeAsCollectible(currentCollectible.x, currentCollectible.y);
        }
    }

    void generateCoins(graphNode *graph, int rows, int cols, int numCoins)
    {
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < numCoins; ++i)
        {
            int randX, randY;
            do
            {
                randX = rand() % rows;
                randY = rand() % cols;
            } while (mazeVec[randX][randY] != ' ');

            mazeVec[randX][randY] = 'C';
            coins.addNode(randX, randY);
        }

        coins.markNodesAsCoin(graph);
    }

    void saveScore(int score)
    {
        ofstream file("scores.txt", ios::app); // append to file

        if (file.is_open())
        {
            // to save the time in the file along w the scores
            time_t currentTime = time(0);
            struct tm now;
            localtime_s(&now, &currentTime);

            char buffer[20];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now);

            file << left << setw(20) << playerName
                 << right << setw(5) << score
                 << " - "
                 << buffer << "\n";

            file.close();
            cout << "Score saved successfully.\n";
        }
        else
        {
            cerr << "Unable to open file.\n";
        }
    }

    string getRecentScore()
    {
        ifstream file("scores.txt");

        if (file.is_open())
        {
            string playerName;
            int score;
            string timestamp;
            string line;
            while (getline(file, line))
            {
                istringstream iss(line);
                iss >> setw(20) >> left >> playerName;
                iss >> setw(5) >> right >> score;
                /*iss.ignore(3);
                iss >> timestamp;*/
            }

            file.close();
            ostringstream result;
            result << "Most Recent Score : ";
            result << "Player Name: " << playerName << " ";
            result << "Score: " << score << " ";
            // result << "Timestamp: " << timestamp;
            return result.str();
        }
        else
        {
            cerr << "Unable to open file.\n";
        }
    }

    void printMaze()
    {
        string playerSymbol;
        system("cls"); // Clear console screen
        cout << setw(20) << setfill('=') << "=" << setfill(' ') << setw(20) << " WELCOME TO ENIGMA ESCAPE" << setw(20) << setfill('=') << "=" << setfill(' ') << endl
             << endl;
        for (int i = 0; i < graph->rows; ++i)
        {
            // cout << setw(10); doesntwork
            for (int j = 0; j < graph->cols; ++j)
            {
                if (graph->getNode(i, j)->isWall)
                {
                    cout << Assets::brightMagentaColor << '#'; // Wall
                }
                else if (graph->getNode(i, j) == player)
                {
                    if (lastKeyClicked == 'a')
                    {
                        playerSymbol = Assets::leftArrow; // Left arrow
                    }
                    else if (lastKeyClicked == 'w')
                    {
                        playerSymbol = Assets::upArrow; // Up arrow
                    }
                    else if (lastKeyClicked == 'd')
                    {
                        playerSymbol = Assets::rightArrow; // Right arrow
                    }
                    else if (lastKeyClicked == 's')
                    {
                        playerSymbol = Assets::downArrow; // Down arrow
                    }
                    else
                    {
                        playerSymbol = 'O'; // Default symbol
                    }
                    cout << Assets::blueColor << playerSymbol;
                }
                else if (mazeVec[i][j] == 'X')
                {
                    cout << Assets::warning;
                }
                else if (mazeVec[i][j] == 'C' && graph->getNode(i, j)->isCoin) // Coin
                {
                    cout << Assets::coin;
                }
                else if (mazeVec[i][j] == 'P' && graph->getNode(i, j)->isCollectible) // Collectible
                {
                    cout << Assets::greenColor << '+';
                }
                else if (mazeVec[i][j] == 'i')
                {
                    cout << Assets::brightGreenColor << '.'; // Path
                }
                else
                {
                    cout << ' ';
                }
            }
            if (i == 3) {
                if (isPaused) {
                    cout << setw(30) << Assets::brightRedColor << "GAME PAUSED!"; // Most recent score
                }
            }

            if (i == 5)
            {
                cout << setw(30) << Assets::brightYellowColor << getRecentScore(); // Most recent score
            }
            if (i == 7)
            {
                cout << setw(30) << Assets::brightCyanColor << "Vehicle Health: "; // Vehicle health BARS
                for (int k = 0; k < vehicleHealth / 10; ++k)
                {
                    cout << Assets::greenColor << Assets::healthBar;
                }
                for (int k = vehicleHealth / 10; k < 10; ++k)
                {
                    cout << " ";
                }
            }

            else if (i == 9)
            {
                cout << setw(30) << Assets::brightCyanColor << "Time: "; // Time
                if (isTimeRunning)
                {
                    time_t currentTime = time(nullptr);
                    time_t elapsedTime = currentTime - startTime;
                    int minutes = static_cast<int>(elapsedTime / 60);
                    int seconds = static_cast<int>(elapsedTime % 60);
                    cout << setfill(' ') << setw(2) << Assets::brightWhiteColor << minutes << ":";
                    cout << setfill(' ') << setw(2) << Assets::brightWhiteColor << seconds;
                }
                else
                {
                    cout << Assets::brightCyanColor << Assets::brightWhiteColor << "00:00";
                }
            }
            else if (i == 11)
            {
                cout << setw(30) << Assets::brightCyanColor << "Moves: " << Assets::brightWhiteColor << playerMoves; // Moves
            }
            else if (i == 13)
            {
                time_t currentTime = time(nullptr);
                time_t elapsedTime = currentTime - startTime;
                int seconds = static_cast<int>(elapsedTime % 60);
                // userScore = abs((-1 * seconds) + (-1 * playerMoves) + (-5 * obstacleHitCount) + (2 * collectibleCount)); // Score
                userScore = abs(playerMoves + (-5 * obstacleHitCount) + (10 * collectibleCount) + (coinCount * 2) - seconds);
                cout << setw(30) << Assets::brightCyanColor << "Score   : " << Assets::brightWhiteColor << userScore;
            }
            else if (i == 15)
            {
                cout << setw(30) << Assets::brightCyanColor << "Cash: " << Assets::brightWhiteColor << 10 * collectibleCount << " $";
            }
            else if (i == 17)
            {
                cout << setw(30) << Assets::whiteColor << "Legend: ";
            }
            else if (i == 19)
            {

                cout << setw(30) << Assets::whiteColor << "Collectible Powerups : " << Assets::brightGreenColor << "+" << Assets::brightYellowColor << "    (+10 Score)    " << Assets::whiteColor << "Obstacles: " << Assets::warning << Assets::brightYellowColor << "    (-1hp health, -5 score)";
            }
            else if (i == 20)
            {

                cout << setw(30) << Assets::whiteColor << "Collectible Coins : " << Assets::coin << Assets::brightYellowColor << "    (+2 Score)    ";
            }

            cout << endl;
        }
        cout << endl;
    }

public:
    mainGame()
    {
        initializeMaze();
        graph = new graphNode(mazeVec, 21, 31);
        // generateObstacles(mazeVec, obstacleQueue, graph);
        player = graph->getNode(0, 0);
        startTime = time(nullptr);
        endTime = 0;
        vehicleHealth = 100;
    }

    // SETTING CONSOLE CONFIGS
    void setConsoleColor(int text, int background)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, text | (background << 4));
    }

    void resetConsoleColor()
    {
        setConsoleColor(7, 0); // Reset color to default (white text on black background)
    }

    void displayLeaderboard()
    {
        cout << Assets::redColor << R"(
  _      _____     _     ____   _____  ____   ____    ___     _     ____   ____  
 | |    | ____|   / \   |  _ \ | ____||  _ \ | __ )  / _ \   / \   |  _ \ |  _ \ 
 | |    |  _|    / _ \  | | | ||  _|  | |_) ||  _ \ | | | | / _ \  | |_) || | | |
 | |___ | |___  / ___ \ | |_| || |___ |  _ < | |_) || |_| |/ ___ \ |  _ < | |_| |
 |_____||_____|/_/   \_\|____/ |_____||_| \_\|____/  \___//_/   \_\|_| \_\|____/ 
                                                                                    
                                                                          )";
        cout << endl;
        ifstream file("scores.txt");

        if (file.is_open())
        {
            leaderboardHeap leaderboard;

            string playerName;
            int score;
            string timestamp;
            string line;

            while (getline(file, line))
            {
                istringstream iss(line);
                iss >> setw(20) >> left >> playerName;
                iss >> setw(5) >> right >> score;
                scoreStruct currentScore{playerName, score};
                leaderboard.insert(currentScore);
            }

            file.close();
            leaderboard.displayTopScores(); // display top scores
            mainMenu();
        }
        else
        {
            cerr << "Unable to open file.\n";
        }
    }

    void helpScreenHelper() // helper function for help screen
    {
        helpScreen();
        mainMenu();
    }

    void run(int diff)
    {
        int numObs, numCoins, numColl;
        if (diff == 1) // setting difficulty
        {
            numObs = 5;
            numCoins = 5;
            numColl = 5;
        }
        else if (diff == 2)
        {
            numObs = 10;
            numCoins = 10;
            numColl = 10;
        }
        else if (diff == 3)
        {
            numObs = 15;
            numCoins = 15;
            numColl = 15;
        }
        printMaze();
        // CONSTRUCTING obstacles, collectibles, coins
        generateObstacles(mazeVec, obstacleQueue, graph, 21, 31, numObs);
        generateCollectible(mazeVec, collectibleQueue, graph, 21, 31, numColl);
        generateCoins(graph, 21, 31, numCoins);

        thread timerThread(&mainGame::updateTimer, this);
        startTime = time(nullptr);
        bool playerMoved = false;
        bool gameWon = false;
        bool gameLost = false;

        while (!gameWon && !gameLost)
        {
            if (isKeyPressed())
            {
                playerMoves++;
                char move = _getch();
                processInput(move);
                if (!playerMoved && (player->x != 0 || player->y != 0)) // start timer when player moves
                {
                    startTime = time(nullptr);
                    isTimeRunning = true;
                    playerMoved = true;
                }

                printMaze();

                if ((player->x == graph->rows - 1 && player->y == graph->cols - 1) && vehicleHealth > 0) // if player reaches end
                {
                    if (diff == 1 && collectibleCount == 5)
                    {
                        endTime = time(nullptr);
                        gameWon = true;
                    }
                    else if (diff == 2 && collectibleCount == 10)
                    {
                        endTime = time(nullptr);
                        gameWon = true;
                    }
                    else if (diff == 3 && collectibleCount == 15)
                    {
                        endTime = time(nullptr);
                        gameWon = true;
                    }
                    else
                    {
                        gameLost = true;
                    }
                }
                if (vehicleHealth == 0) // if vehicle health is 0
                {
                    gameLost = true;
                }
            }
        }
        isTimeRunning = false;
        timerThread.join();

        // EVENT HANDLERS
        if (gameWon)
        {
            printWinMessage();
            saveScore(userScore);
        }
        else if (gameLost)
        {
            printLoseMessage();
            saveScore(userScore);
        }
    }

    void autoRun()
    {
        mystack<pathNode *> shortestPath = findShortestPath(); // find shortest path
        mystack<pathNode *> temp = shortestPath;               // store shortest path in temp
        while (!temp.empty())
        {
            pathNode *curr = temp.top();
            temp.pop();
            // draw path
            mazeVec[curr->x][curr->y] = 'i'; // show path using dots
            printMaze();
        }
        while (!shortestPath.empty()) // move player on shortest path
        {
            // store curr
            pathNode *curr = shortestPath.top();
            // pop it
            shortestPath.pop();
            player = curr;
            printMaze();
            Sleep(50);
        }
    }

    mystack<pathNode *> findShortestPath() // bfs traversal algo
    {
        myqueue<pathNode *> nodeQueue;
        bool visitedNodes[MAP_ROWS][MAP_COLS] = {false};                  // visited nodes
        pathNode *previousTraversedNodes[MAP_ROWS][MAP_COLS] = {nullptr}; // previous traversed nodes
        mystack<pathNode *> shortestPath_nodes;                           // shortest path nodes

        // bfs traversal algo from start pos (0,0)
        nodeQueue.push(graph->emptyPathNodes[MAP_START_X][MAP_START_Y]); // push start node
        visitedNodes[MAP_START_X][MAP_START_Y] = true;

        while (!nodeQueue.empty())
        {
            pathNode *curr = nodeQueue.front();
            nodeQueue.pop();

            pathNode *neighborsArray[] = {curr->up, curr->down, curr->left, curr->right}; // array of neighbors
            for (pathNode *neighborNode : neighborsArray)
            {
                if (neighborNode != nullptr && !neighborNode->isWall && !visitedNodes[neighborNode->x][neighborNode->y]) // if neighbor is not a wall and not visited
                {
                    nodeQueue.push(neighborNode);
                    visitedNodes[neighborNode->x][neighborNode->y] = true;
                    previousTraversedNodes[neighborNode->x][neighborNode->y] = curr;

                    if (neighborNode->x == graph->rows - 1 && neighborNode->y == graph->cols - 1) // if destination reached
                    {
                        // destination reached
                        pathNode *dest = neighborNode;
                        while (dest != nullptr)
                        {
                            //backtrack
                            shortestPath_nodes.push(dest);
                            dest = previousTraversedNodes[dest->x][dest->y]; // backtrack
                        }
                        return shortestPath_nodes;
                    }
                }
            }
        }

        return shortestPath_nodes;
    }

    ~mainGame()
    {
        delete graph;
    }

    void displayWelcomeMessage()
    {
        setConsoleColor(12, 0); // Red text on black background
        cout << R"(
        ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗    ████████╗ ██████╗     ████████╗██╗  ██╗███████╗
        ██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝    ╚══██╔══╝██╔═══██╗    ╚══██╔══╝██║  ██║██╔════╝
        ██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗         ██║   ██║   ██║       ██║   ███████║█████╗  
        ██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝         ██║   ██║   ██║       ██║   ██╔══██║██╔══╝  
        ╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗       ██║   ╚██████╔╝       ██║   ██║  ██║███████╗
         ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝       ╚═╝    ╚═════╝        ╚═╝   ╚═╝  ╚═╝╚══════╝
            ███████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗     ███████╗███████╗ ██████╗ █████╗ ██████╗ ███████╗            
            ██╔════╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗    ██╔════╝██╔════╝██╔════╝██╔══██╗██╔══██╗██╔════╝            
            █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║    █████╗  ███████╗██║     ███████║██████╔╝█████╗              
            ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║    ██╔══╝  ╚════██║██║     ██╔══██║██╔═══╝ ██╔══╝              
            ███████╗██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║    ███████╗███████║╚██████╗██║  ██║██║     ███████╗            
            ╚══════╝╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝    ╚══════╝╚══════╝ ╚═════╝╚═╝  ╚═╝╚═╝     ╚══════╝                                             
                                                     
)";
        // cout << setw(10) << "Hi, " << playerName << endl;
        resetConsoleColor();
    }

    char mainMenu()
    {
        // if (!isFullScreen)
        //{
        //     // to trigger fullscreen
        //     keybd_event(VK_F11, 0, KEYEVENTF_EXTENDEDKEY, 0); // press F11 key
        //     keybd_event(VK_F11, 0, KEYEVENTF_KEYUP, 0);       // release f11 key
        //     isFullScreen = true;
        // }

        system("cls"); // Clear console screen
        displayWelcomeMessage();
        char inputChoice_mainmenu;
        cout << R"(
        ███    ███     █████     ██    ███    ██       ███    ███    ███████    ███    ██    ██    ██ 
        ████  ████    ██   ██    ██    ████   ██       ████  ████    ██         ████   ██    ██    ██ 
        ██ ████ ██    ███████    ██    ██ ██  ██       ██ ████ ██    █████      ██ ██  ██    ██    ██ 
        ██  ██  ██    ██   ██    ██    ██  ██ ██       ██  ██  ██    ██         ██  ██ ██    ██    ██ 
        ██      ██    ██   ██    ██    ██   ████       ██      ██    ███████    ██   ████     ██████                                                        
    )";
        cout << R"(
      _        _   _   _  _____  ___    __  __   ___   ___   ___                 
     / |      /_\ | | | ||_   _|/ _ \  |  \/  | / _ \ |   \ | __|                
     | | _   / _ \| |_| |  | | | (_) | | |\/| || (_) || |) || _|                 
     |_|(_) /_/_\_\\___/ _ |_|_ \___/  |_|  |_| \___/ |___/ |___|___   ___   ___ 
     |_  )    |  \/  |  /_\  | \| || | | | /_\  | |    |  \/  | / _ \ |   \ | __|
      / /  _  | |\/| | / _ \ | .` || |_| |/ _ \ | |__  | |\/| || (_) || |) || _| 
     /___|(_) |_| _|_|/_/ \_\|_|\_| \___//_/ \_\|____| |_|  |_| \___/ |___/ |___|
     |__ /    | || || __|| |   | _ \                                             
      |_ \ _  | __ || _| | |__ |  _/                                             
     |___/(_) |_||_||___||____||_|__   ___  ___  ___   ___    _    ___  ___      
     | | |    | |   | __|  /_\  |   \ | __|| _ \| _ ) / _ \  /_\  | _ \|   \     
     |_  _|_  | |__ | _|  / _ \ | |) || _| |   /| _ \| (_) |/ _ \ |   /| |) |    
      _|_|(_) |____||___|/_/_\_\|___/ |___||_|_\|___/ \___//_/ \_\|_|_\|___/     
     | __|    | __|\ \/ /|_ _||_   _|                                            
     |__ \ _  | _|  >  <  | |   | |                                              
     |___/(_) |___|/_/\_\|___|  |_|                                                                                      
                                                                             
        )";
        inputChoice_mainmenu = _getch();
        return inputChoice_mainmenu;
    }
    char selectDifficulty()
    {
        system("cls");
        char inputChoice_difficulty;
        // raw string
        cout << R"( 
      __     ______           _______     __             
     /_ |   |  ____|   /\    / ____\ \   / /             
      | |   | |__     /  \  | (___  \ \_/ /              
      | |   |  __|   / /\ \  \___ \  \   /               
      | |_  | |____ / ____ \ ____) |  | |                
      |_(_) |______/_/    \_\_____/   |_|                )
      ___      __  __ ______ _____ _____ _    _ __  __   
     |__ \    |  \/  |  ____|  __ \_   _| |  | |  \/  |  
        ) |   | \  / | |__  | |  | || | | |  | | \  / |  
       / /    | |\/| |  __| | |  | || | | |  | | |\/| |  
      / /_ _  | |  | | |____| |__| || |_| |__| | |  | |  
     |____(_) |_|  |_|______|_____/_____|\____/|_|  |_|  
      ____     _    _          _____  _____              
     |___ \   | |  | |   /\   |  __ \|  __ \             
       __) |  | |__| |  /  \  | |__) | |  | |            
      |__ <   |  __  | / /\ \ |  _  /| |  | |            
      ___) |  | |  | |/ ____ \| | \ \| |__| |            
     |____(_) |_|  |_/_/    \_\_|  \_\_____/             
      _  _     _____  ______ _______ _    _ _____  _   _ 
     | || |   |  __ \|  ____|__   __| |  | |  __ \| \ | |
     | || |_  | |__) | |__     | |  | |  | | |__) |  \| |
     |__   _| |  _  /|  __|    | |  | |  | |  _  /| . ` |
        | |_  | | \ \| |____   | |  | |__| | | \ \| |\  |
        |_(_) |_|  \_\______|  |_|   \____/|_|  \_\_| \_|                                                                                                     
    )";
        inputChoice_difficulty = _getch();
        return inputChoice_difficulty;
    }

    void helpScreen()
    {
        cout << R"(
        In this maze game, you have two modes. 
        Auto Mode lets the player automatically navigate to the maze end. 
        In Manual Mode, choose difficulty (Easy - 1, Medium - 2, Hard - 3) and use W, A, S, D keys to move. 
        The objective is to reach the maze end while avoiding obstacles (danger signs) that decrease health. 
        Collect cash ($) for score, and make sure to gather all powerups (+) to win (and x2 your score). 
        Successfully complete the maze with all powerups to achieve victory. 
        Best of luck, and enjoy the Maze Game!        
        )";
        cout << endl;

        cout << Assets::brightYellowColor << "Input 0 to return;" << endl;
        if (_getch() == '0')
        {
            return;
        }
    }
};

void choiceHandler() // mainmenu basically
{
    while (true)
    {
        mainGame game;
        char inputChoice_mainmenu = game.mainMenu();
        char inputChoice_difficulty;

        switch (inputChoice_mainmenu)
        {
        case '1':
            game.autoRun();
            break;
        case '2':
            inputChoice_difficulty = game.selectDifficulty();
            if (inputChoice_difficulty == '1')
            {
                game.run(1);
            }
            else if (inputChoice_difficulty == '2')
            {
                game.run(2);
            }
            else if (inputChoice_difficulty == '3')
            {
                game.run(3);
            }
            else if (inputChoice_difficulty == '4')
            {
                break; // exit the loop and return to the main menu
            }
            else
            {
                cout << "Invalid input. Please try again." << endl;
            }
            break;
        case '3':
            game.helpScreenHelper();
            break;
        case '4':
            game.displayLeaderboard();
            break;
        case '5':
            exit(0);
        default:
            cout << "Invalid input. Please try again." << endl;
            break;
        }
    }
}

void printWelcomeScreen()
{
    system("cls");
    cout << "Welcome, " << playerName << "!" << endl;

    cout << endl
         << endl
         << endl;

    string welcomeMsg = R"(
                 ___       ________  ________  ________  ___  ________   ________     
                |\  \     |\   __  \|\   __  \|\   ___ \|\  \|\   ___  \|\   ____\    
                \ \  \    \ \  \|\  \ \  \|\  \ \  \_|\ \ \  \ \  \\ \  \ \  \___|    
                 \ \  \    \ \  \\\  \ \   __  \ \  \ \\ \ \  \ \  \\ \  \ \  \  ___  
                  \ \  \____\ \  \\\  \ \  \ \  \ \  \_\\ \ \  \ \  \\ \  \ \  \|\  \ 
                   \ \_______\ \_______\ \__\ \__\ \_______\ \__\ \__\\ \__\ \_______\
                    \|_______|\|_______|\|__|\|__|\|_______|\|__|\|__| \|__|\|_______|
    )";

    while (!isKeyPressed())
    {
        for (int color = 1; color <= 15; color++) // changing colors automatically
        {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
            cout << welcomeMsg << endl;
            Sleep(100);
        }
    }

    // Reset console color before exiting
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    system("cls");
}
