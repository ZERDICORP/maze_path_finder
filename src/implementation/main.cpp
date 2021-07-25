#include "config.h"
#include "tools.h"

int loop(sf::RenderWindow& window, std::map<std::string, float>& cfg)
{
	bool bNeedToUpdateConsole = true;
	bool bFinding = true;
	bool bRollback = false;

	int iSqW = cfg["squareWidth"];
	int iRows = mWH / iSqW;
	int iCols = mWW / iSqW;

	sf::Vector2i startPos(0, 0);
	sf::Vector2i finishPos(zer::athm::rand_int(10, iCols - 1), zer::athm::rand_int(10, iRows - 1));
	sf::Vector2i currentPos = startPos;

	std::vector<std::vector<Node>> nodes(iRows, std::vector<Node>(iCols, Node()));

	mazeGen(nodes, iRows, iCols);

	for (int iRow = 0; iRow < iRows; ++iRow)
		for (int iCol = 0; iCol < iCols; ++iCol)
			nodes[iRow][iCol].bCollapse = false;

	std::vector<sf::Vector2i> history;
	history.push_back(currentPos);

	std::vector<sf::Vector2i> vectors({
		sf::Vector2i(0, -1),
		sf::Vector2i(0, 1),
		sf::Vector2i(-1, 0),
		sf::Vector2i(1, 0)
	});

	sf::RectangleShape rectStart(sf::Vector2f(iSqW, iSqW));
	rectStart.setFillColor(sf::Color::Yellow);
	rectStart.setPosition(0, 0);

	sf::RectangleShape rectFinish(sf::Vector2f(iSqW, iSqW));
	rectFinish.setFillColor(sf::Color::Red);
	rectFinish.setPosition(finishPos.x * iSqW, finishPos.y * iSqW);
	
	sf::VertexArray pathLine(sf::LinesStrip, 2);
	pathLine[0].color = sf::Color(15, 255, 252);
	pathLine[1].color = sf::Color(15, 255, 252);

	sf::VertexArray mazeLine(sf::LineStrip, 2);
	mazeLine[0].color = sf::Color::White;
	mazeLine[1].color = sf::Color::White;

	while (window.isOpen())
	{
		window.clear();

		/*
			Maze path finder algorithm body.
		*/
		if (bFinding)
		{
			if (currentPos.y == finishPos.y && currentPos.x == finishPos.x)
			{
				bFinding = false;
				rectFinish.setFillColor(sf::Color::Green);
				continue;
			}

			std::vector<int> vectorVariants({0, 1, 2, 3});

			while (true)
			{
				int iRandomVectorVariantIndex = zer::athm::rand_int(vectorVariants.size());
				int iRandomVectorIndex = vectorVariants[iRandomVectorVariantIndex];

				sf::Vector2i tempPos = currentPos + vectors[iRandomVectorIndex];

				if (zer::athm::inRange2D(iRows, iCols, tempPos.y, tempPos.x))
				{
					bool bWall = ((vectors[iRandomVectorIndex].y < 0 && nodes[currentPos.y][currentPos.x].bTopWall) ||
						(vectors[iRandomVectorIndex].y > 0 && nodes[tempPos.y][tempPos.x].bTopWall) ||
						(vectors[iRandomVectorIndex].x < 0 && nodes[currentPos.y][currentPos.x].bLeftWall) ||
						(vectors[iRandomVectorIndex].x > 0 && nodes[tempPos.y][tempPos.x].bLeftWall));

					/*
						If there are no walls in the direction and we have not been there yet.
					*/
					if (!bWall && !nodes[tempPos.y][tempPos.x].bCollapse)
					{
						if (bRollback)
						{
							history.push_back(currentPos);
							bRollback = false;
						}

						currentPos = tempPos;

						history.push_back(currentPos);
						
						nodes[currentPos.y][currentPos.x].bCollapse = true;

						break;
					}
					else
						vectorVariants.erase(vectorVariants.begin() + iRandomVectorVariantIndex);
				}
				else
					vectorVariants.erase(vectorVariants.begin() + iRandomVectorVariantIndex);

				if (vectorVariants.size() == 0)
				{
					currentPos = history[history.size() - 1];

					history.erase(history.end() - 1);
					
					bRollback = true;

					break;
				}
			}
		}

		/*
			Drawing start & finish.
		*/
		window.draw(rectStart);
		window.draw(rectFinish);

		/*
			Drawing path.
		*/
		if (history.size() > 1)
		{
			for (int i = 1; i < history.size() - 1; ++i)
			{
				pathLine[0].position = sf::Vector2f(history[i - 1].x * iSqW + (iSqW / 2), history[i - 1].y * iSqW + (iSqW / 2));
				pathLine[1].position = sf::Vector2f(history[i].x * iSqW + (iSqW / 2), history[i].y * iSqW + (iSqW / 2));

				window.draw(pathLine);
			}

			pathLine[0].position = sf::Vector2f(
				history[history.size() - 2].x * iSqW + (iSqW / 2), history[history.size() - 2].y * iSqW + (iSqW / 2));
			pathLine[1].position = sf::Vector2f(
				history[history.size() - 1].x * iSqW + (iSqW / 2), history[history.size() - 1].y * iSqW + (iSqW / 2));
			
			window.draw(pathLine);
		}

		/*
			Drawing maze.
		*/
		for (int r = 0; r < iRows; ++r)
		{
			for (int c = 0; c < iCols; ++c)
			{
				if (nodes[r][c].bTopWall)
				{
					mazeLine[0].position = sf::Vector2f(c * iSqW, r * iSqW);
					mazeLine[1].position = sf::Vector2f(c * iSqW + iSqW, r * iSqW);

					window.draw(mazeLine);
				}
				if (nodes[r][c].bLeftWall)
				{
					mazeLine[0].position = sf::Vector2f(c * iSqW, r * iSqW);
					mazeLine[1].position = sf::Vector2f(c * iSqW, r * iSqW + iSqW);
					
					window.draw(mazeLine);
				}
			}
		}

		window.display();

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(cfg);
			bNeedToUpdateConsole = false;
		}

		switch (eventListener(window))
		{
			case EVENT_CODE::CLOSE:
				window.close();
				break;

			case EVENT_CODE::RESTART:
				init(window);
				break;
		}
	}
	return 0;
}

int init(sf::RenderWindow& window)
{
	std::map<std::string, float> cfg = readConfig(mConfigPath);
	
	return loop(window, cfg);
}