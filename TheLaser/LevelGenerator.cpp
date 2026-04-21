#include "pch.h"
#include "LevelGenerator.h"

void LevelGenerator::GeneratePath(Grid* grid, StartingPosition startPosition, Vector2f laserDirection, int numMirrors)
{
	int minimumDistance = 2;
	int numDecoys = 7;

	bool pathGenerated = false;
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	while (!pathGenerated)
	{
		ClearAllMirrors(grid);
		std::vector<bool> visitedCells(totalColumns * totalRows, false);

		int currentColumn = 0;
		int currentRow = 0;

		SetupStartingPosition(grid, startPosition, currentColumn, currentRow);

		int directionX = static_cast<int>(laserDirection.x);
		int directionY = static_cast<int>(laserDirection.y);

		bool isTrapped = false;

		for (int i = 0; i < numMirrors; ++i)
		{
			int maxDistance = GetMaximumDistance(grid, currentColumn, currentRow, directionX, directionY, visitedCells);

			if (maxDistance < minimumDistance)
			{
				
				isTrapped = true;
				break;

			}

			int range = maxDistance - minimumDistance + 1;
			int stepsToTake = minimumDistance + (rand() % range);

			for (int step = 0; step < stepsToTake; ++step)
			{
				int currentIndex = currentRow * totalColumns + currentColumn;
				visitedCells[currentIndex] = true;

				currentColumn = currentColumn + directionX;
				currentRow = currentRow + directionY;

			}


			int finalIndex = currentRow * totalColumns + currentColumn;
			visitedCells[finalIndex] = true;

			if (i == (numMirrors - 1))
			{
				grid->AddReceiver(currentColumn, currentRow);
			}
			else
			{
				grid->AddMirror(currentColumn, currentRow);
			}

			if (i < (numMirrors - 1))
			{
				if (!TryGetNewDirection(grid, currentColumn, currentRow, directionX, directionY, visitedCells, directionX, directionY))
				{
					isTrapped = true;
					break;
				}
			}
		}

		if (!isTrapped)
		{
			isTrapped = IsFinalPathTrapped(grid, currentColumn, currentRow, directionX, directionY, visitedCells);
		}

		if (!isTrapped)
		{
			pathGenerated = true;
			PlaceDecoyMirrors(grid, visitedCells, numDecoys);
		}
	}
}

void LevelGenerator::ClearAllMirrors(Grid* grid)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	for (int row = 0; row < totalRows; ++row)
	{
		for (int column = 0; column < totalColumns; ++column)
		{
			if (grid->GetCellFromIndex(column, row)->HasMirror())
			{
				grid->RemoveMirror(column, row);
			}
		}
	}
}

void LevelGenerator::SetupStartingPosition(Grid* grid, StartingPosition startPosition, int& currentColumn, int& currentRow)
{
	if (startPosition == StartingPosition::BottomLeft)
	{
		currentColumn = 0;
		currentRow = 0;
	}
	else if (startPosition == StartingPosition::BottomRight)
	{
		currentColumn = grid->GetColumns() - 1;
		currentRow = 0;
	}
	else if (startPosition == StartingPosition::TopLeft)
	{
		currentColumn = 0;
		currentRow = grid->GetRows() - 1;
	}
	else if (startPosition == StartingPosition::TopRight)
	{
		currentColumn = grid->GetColumns() - 1;
		currentRow = grid->GetRows() - 1;
	}
}

int LevelGenerator::GetMaximumDistance(Grid* grid, int currentColumn, int currentRow, int directionX, int directionY, const std::vector<bool>& visitedCells)
{
	int maxDistance = 0;
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	int checkColumn = currentColumn + directionX;
	int checkRow = currentRow + directionY;

	while (checkColumn >= 0 && checkColumn < totalColumns && checkRow >= 0 && checkRow < totalRows)
	{
		int cellIndex = checkRow * totalColumns + checkColumn;

		if (visitedCells[cellIndex])
		{
			break;
		}

		maxDistance = maxDistance + 1;
		checkColumn = checkColumn + directionX;
		checkRow = checkRow + directionY;
	}

	return maxDistance;
}

bool LevelGenerator::TryGetNewDirection(Grid* grid, int currentColumn, int currentRow, int previousDirectionX, int previousDirectionY, const std::vector<bool>& visitedCells, int& newDirectionX, int& newDirectionY)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	int turnLeftX = -previousDirectionY;
	int turnLeftY = previousDirectionX;
	int turnRightX = previousDirectionY;
	int turnRightY = -previousDirectionX;

	bool canTurnLeft = false;
	int leftColumn = currentColumn + turnLeftX;
	int leftRow = currentRow + turnLeftY;

	if (leftColumn >= 0 && leftColumn < totalColumns && leftRow >= 0 && leftRow < totalRows)
	{
		int leftIndex = leftRow * totalColumns + leftColumn;
		if (!visitedCells[leftIndex])
		{
			canTurnLeft = true;
		}
	}

	bool canTurnRight = false;
	int rightColumn = currentColumn + turnRightX;
	int rightRow = currentRow + turnRightY;

	if (rightColumn >= 0 && rightColumn < totalColumns && rightRow >= 0 && rightRow < totalRows)
	{
		int rightIndex = rightRow * totalColumns + rightColumn;
		if (!visitedCells[rightIndex])
		{
			canTurnRight = true;
		}
	}

	if (canTurnLeft && canTurnRight)
	{
		if (rand() % 2 == 0)
		{
			newDirectionX = turnLeftX;
			newDirectionY = turnLeftY;
		}
		else
		{
			newDirectionX = turnRightX;
			newDirectionY = turnRightY;
		}
		return true;
	}
	else if (canTurnLeft)
	{
		newDirectionX = turnLeftX;
		newDirectionY = turnLeftY;
		return true;
	}
	else if (canTurnRight)
	{
		newDirectionX = turnRightX;
		newDirectionY = turnRightY;
		return true;
	}

	return false;
}

bool LevelGenerator::IsFinalPathTrapped(Grid* grid, int currentColumn, int currentRow, int directionX, int directionY, const std::vector<bool>& visitedCells)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	int finalCheckColumn = currentColumn + directionX;
	int finalCheckRow = currentRow + directionY;

	while (finalCheckColumn >= 0 && finalCheckColumn < totalColumns && finalCheckRow >= 0 && finalCheckRow < totalRows)
	{
		int cellIndex = finalCheckRow * totalColumns + finalCheckColumn;

		if (visitedCells[cellIndex])
		{
			return true;
		}

		finalCheckColumn = finalCheckColumn + directionX;
		finalCheckRow = finalCheckRow + directionY;
	}

	return false;
}

void LevelGenerator::PlaceDecoyMirrors(Grid* grid, std::vector<bool>& visitedCells, int numberOfDecoys)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	int decoysPlaced = 0;
	int maximumAttempts = numberOfDecoys * 10;
	int currentAttempts = 0;

	while (decoysPlaced < numberOfDecoys && currentAttempts < maximumAttempts)
	{
		int randomColumn = rand() % totalColumns;
		int randomRow = rand() % totalRows;
		int targetIndex = randomRow * totalColumns + randomColumn;

		if (!visitedCells[targetIndex])
		{
			grid->AddMirror(randomColumn, randomRow);
			visitedCells[targetIndex] = true;
			decoysPlaced = decoysPlaced + 1;
		}

		currentAttempts = currentAttempts + 1;
	}
}