// Project 5

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"

using namespace std;

class maze
{
public:
    maze(ifstream &fin);
    void print(int, int, int, int) const;
    bool isLegal(int i, int j);

    void setMap(int i, int j, int n);
    int getMap(int i, int j) const;
    void mapMazeToGraph(graph &g);

    //Get the row and column for a specific node
    void getCoord(int node, int &r, int &c) const;

private:
    int rows; // number of rows in the maze
    int cols; // number of columns in the maze

    matrix<bool> value;
    matrix<int> map;      // Mapping from maze (i,j) values to node index values
};

void maze::setMap(int i, int j, int n)
// Set mapping from maze cell (i,j) to graph node n.
{
    map[i][j] = n;
}

int maze ::getMap(int i, int j) const
// Return mapping of maze cell (i,j) in the graph.
{

    if (map.rows() > i && map.cols() > j && i >= 0 && j >= 0)
    {
        return map[i][j];
    }

    //if out of range or filled
    return -1;
}

maze::maze(ifstream &fin)
// Initializes a maze by reading values from fin.  Assumes that the
// number of rows and columns indicated in the file are correct.
{
    fin >> rows;
    fin >> cols;

    char x;

    value.resize(rows, cols);

    for (int i = 0; i <= rows - 1; i++)
        for (int j = 0; j <= cols - 1; j++)
        {
            fin >> x;

            if (x == 'O')
            {
                value[i][j] = true;
            }
            else
            {
                value[i][j] = false;
            }
        }

    map.resize(rows, cols);
}

void maze::print(int goalI, int goalJ, int currI, int currJ) const
// Print out a maze, with the goal and current cells marked on the
// board.
{
    cout << endl;

    if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
    {
        throw rangeError("Bad value in maze::print");
    }

    if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
    {
        throw rangeError("Bad value in maze::print");
    }

    for (int i = 0; i <= rows - 1; i++)
    {
        for (int j = 0; j <= cols - 1; j++)
        {
            if (i == goalI && j == goalJ)
            {
                cout << "*";
            }
            else if (i == currI && j == currJ)
            {
                cout << "+";
            }
            else if (value[i][j])
            {
                cout << " ";
            }
            else
            {
                cout << "X";
            }
        }

        cout << endl;
    }

    cout << endl;
}

bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze.
{
    if (i < 0 || i > rows || j < 0 || j > cols)
    {
        throw rangeError("Bad value in maze::isLegal");
    }

    return value[i][j];
}

void maze::mapMazeToGraph(graph &g)
// Create a graph g that represents the legal moves in the maze m.
{
    //set maps for the whole maze
    int n = 0;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (value[r][c] == true)
            {
                setMap(r, c, n); //set map
                node newNode;
                newNode.setNode(n, 0, false, false);
                g.addNode(newNode);
                n++; //increment n
            }
            else //set unusable squares to -1 instead of 0
            {
                setMap(r, c, -1);
            }
        }
    }

    //r and c iterate over all squares in maze
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int self = getMap(r, c);
            int neighbors[4]; //array of neighbor maps
            string dir;

            neighbors[0] = getMap(r, c + 1); //right
            neighbors[1] = getMap(r + 1, c); //down
            neighbors[2] = getMap(r, c - 1); //left
            neighbors[3] = getMap(r - 1, c); //up

            if (self >= 0)
            {
                for (int n = 0; n <= 4; n++)
                {
                    if (neighbors[n] >= 0)
                    {
                        switch (n)
                        {
                        case 0:
                            dir = "right";
                            break;

                        case 1:
                            dir = "down";
                            break;

                        case 2:
                            dir = "left";
                            break;

                        case 3:
                            dir = "up";
                            break;
                        }

                        g.addEdge(self, neighbors[n], 0, dir);
                    }
                }
            }
        }
    }
}

void maze::getCoord(int node, int &r, int &c) const
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (map[i][j] == node)
            {
                r = i;
                c = j;
                return;
            }
}

//Takes a vector of result in reverse order (0 is the final node number in path)
//and prints out the directions
int printPath(const maze &m, const graph &g, vector<int> result, bool graph)
{
	//get the goal x and y on the maze
    int goalx;
    int goaly;
    m.getCoord(result[0], goalx, goaly);
    cout << "\nInitial Maze";

	if(!graph) //make new line if not using graphics
		cout<<endl;

	//for every node in the result, create an inage and print out maze and direction
    for (int i = result.size() - 1; i >= 0; i--)
    {
		//get current position x and y
        int currx;
        int curry;
        m.getCoord(result[i], currx, curry);

		//print maze if desired
        if (graph)
        {
            m.print(goalx, goaly, currx, curry);
        }

		//print direction of moveent
        if (i >= 1)
            cout << "Moving " << g.getEdge(g.getNode(result[i]).getId(),
                                           g.getNode(result[i - 1]).getId()).getDirection();
		if(!graph)
			cout<<endl;
    }

    return 0;
}

int main()
{
    ifstream fin;

    // Read the maze from the file.
    string fileName = "maze1.txt";

    fin.open(fileName.c_str());

	//error hanidng
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        system("pause");
        exit(1);
    }

    try
    {

        graph g;

        while (fin && fin.peek() != 'Z')
        {
			//read in maze
            maze m(fin);
            m.mapMazeToGraph(g);

			//user options
            cout << "Enter r for recursive, n for non-recursive solving: ";
            char mode;
            cin >> mode;
            cout << "For graphical output (verbose), enter g, else enter any letter: ";
            char x;
            cin >> x;
            bool graph = x == 'g';

            if (mode == 'r')
            {
                vector<int> result = g.recursiveDFS(g);
                printPath(m, g, result, graph);
            }
            else if (mode == 'n')
            {
                vector<int> result = g.findPath();
                printPath(m, g, result, graph);
            }
            else
            {
                cout << "Sorry didn't recognize that letter..." << endl;
            }


        }


    }
	//exception handing
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl;
        system("pause");
        exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl;
        system("pause");
        exit(1);
    }

    //printf("Exited successfully...\n");
    system("pause");
    return 0;
}
