#include "config.h"

struct Node
{
	bool bCollapse = false;
	bool bTopWall = true;
	bool bLeftWall = true;
};

void mazeGen(std::vector<std::vector<Node>>& nodes, int iRows, int iCols);
void displayConsoleInformation(std::map<std::string, float>& cfg);

std::map<std::string, float> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);

int init(sf::RenderWindow& window);
int main();