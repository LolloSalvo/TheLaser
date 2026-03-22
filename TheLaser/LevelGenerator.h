#pragma once
#include "Grid.h"
#include <vector>

enum class StartingPosition
{
	BottomLeft,
	BottomRight,
	TopLeft,
	TopRight
};

class LevelGenerator
{

public:

	void GeneratePath(Grid* grid, StartingPosition startPosition, Vector2f laserDirection, int numMirrors);

private:

	void ClearAllMirrors(Grid* grid);
	void SetupStartingPosition(Grid* grid, StartingPosition startPosition, int& currentColumn, int& currentRow);

	int GetMaximumDistance(Grid* grid, int currentColumn, int currentRow, int directionX, int directionY, const std::vector<bool>& visitedCells);
	
	bool TryGetNewDirection(Grid* grid, int currentColumn, int currentRow, int previousDirectionX, int previousDirectionY, const std::vector<bool>& visitedCells, int& newDirectionX, int& newDirectionY);
	bool IsFinalPathTrapped(Grid* grid, int currentColumn, int currentRow, int directionX, int directionY, const std::vector<bool>& visitedCells);
	
	void PlaceDecoyMirrors(Grid* grid, std::vector<bool>& visitedCells, int numberOfDecoys);

};