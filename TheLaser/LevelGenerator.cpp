#include "pch.h"
#include "LevelGenerator.h"

// ---------------------------------------------------------------
//  GENERATE PATH (laser 1)
//  Stores all beam-traversed cells in m_Path1Cells so that
//  GenerateSecondPath can fully avoid them.
// ---------------------------------------------------------------
void LevelGenerator::GeneratePath(Grid* grid, StartingPosition startPosition, Vector2f laserDirection, int numMirrors)
{
	int minimumDistance = 2;
	int numDecoys = 7;
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	bool pathGenerated = false;
	int  totalAttempts = 0;

	while (!pathGenerated)
	{
		++totalAttempts;

		// Every 150 attempts reduce mirror count (min 3) to avoid getting stuck
		if (totalAttempts % 150 == 0 && numMirrors > 3)
			--numMirrors;

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
				visitedCells[currentRow * totalColumns + currentColumn] = true;
				currentColumn += directionX;
				currentRow += directionY;
			}

			visitedCells[currentRow * totalColumns + currentColumn] = true;

			if (i == (numMirrors - 1))
				grid->AddReceiver(currentColumn, currentRow);
			else
				grid->AddMirror(currentColumn, currentRow);

			if (i < (numMirrors - 1))
			{
				if (!TryGetNewDirection(grid, currentColumn, currentRow, directionX, directionY, visitedCells, directionX, directionY))
				{
					isTrapped = true;
					break;
				}
			}
		}

		// NOTE: IsFinalPathTrapped is NOT called - the receiver terminates the laser.

		if (!isTrapped)
		{
			m_Path1Cells = visitedCells;
			pathGenerated = true;
		}
	}
}

// ---------------------------------------------------------------
//  GENERATE SECOND PATH (laser 2)
//  Uses m_Path1Cells as the initial blocked set, so the second
//  laser's beam never crosses any cell that laser 1 travels through.
//  Also ensures the ReceiverBlue is not placed on a path-1 cell.
// ---------------------------------------------------------------
void LevelGenerator::GenerateSecondPath(Grid* grid, int startCol, int startRow, int dirX, int dirY, int numMirrors)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();
	int totalCells = totalColumns * totalRows;
	int minimumDistance = 2;

	// Always reset so PlaceDecoys never uses stale data from a previous level
	m_Path2Cells.assign(totalCells, false);

	bool pathGenerated = false;
	int  totalAttempts = 0;

	while (!pathGenerated)
	{
		++totalAttempts;

		// Every 150 attempts reduce mirror count (min 2) to find room more easily
		if (totalAttempts % 150 == 0 && numMirrors > 2)
			--numMirrors;

		// Snapshot existing mirrors for rollback on failure
		std::vector<bool> existingMirrors(totalCells, false);
		for (int row = 0; row < totalRows; ++row)
			for (int col = 0; col < totalColumns; ++col)
				if (grid->GetCellFromIndex(col, row)->HasMirror())
					existingMirrors[row * totalColumns + col] = true;

		// Blocked = path1 beam cells OR existing mirrors
		bool path1Valid{ static_cast<int>(m_Path1Cells.size()) == totalCells };
		std::vector<bool> visitedCells(totalCells, false);
		for (int idx = 0; idx < totalCells; ++idx)
			visitedCells[idx] = (path1Valid && m_Path1Cells[idx]) || existingMirrors[idx];

		visitedCells[startRow * totalColumns + startCol] = true;

		int currentColumn = startCol;
		int currentRow = startRow;
		int directionX = dirX;
		int directionY = dirY;
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
				visitedCells[currentRow * totalColumns + currentColumn] = true;
				currentColumn += directionX;
				currentRow += directionY;
			}

			visitedCells[currentRow * totalColumns + currentColumn] = true;

			if (i == (numMirrors - 1))
			{
				int cellIdx = currentRow * totalColumns + currentColumn;
				if (path1Valid && m_Path1Cells[cellIdx]) { isTrapped = true; break; }
				grid->AddReceiverBlue(currentColumn, currentRow);
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
			m_Path2Cells = visitedCells;
			pathGenerated = true;
		}
		else
		{
			for (int row = 0; row < totalRows; ++row)
				for (int col = 0; col < totalColumns; ++col)
					if (!existingMirrors[row * totalColumns + col] && grid->GetCellFromIndex(col, row)->HasMirror())
						grid->RemoveMirror(col, row);
		}
	}
}


// ---------------------------------------------------------------
//  PRIVATE HELPERS
// ---------------------------------------------------------------
void LevelGenerator::ClearAllMirrors(Grid* grid)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();

	for (int row = 0; row < totalRows; ++row)
		for (int column = 0; column < totalColumns; ++column)
			if (grid->GetCellFromIndex(column, row)->HasMirror())
				grid->RemoveMirror(column, row);
}

void LevelGenerator::SetupStartingPosition(Grid* grid, StartingPosition startPosition, int& currentColumn, int& currentRow)
{
	if (startPosition == StartingPosition::BottomLeft) { currentColumn = 0;                      currentRow = 0; }
	else if (startPosition == StartingPosition::BottomRight) { currentColumn = grid->GetColumns() - 1; currentRow = 0; }
	else if (startPosition == StartingPosition::TopLeft) { currentColumn = 0;                      currentRow = grid->GetRows() - 1; }
	else if (startPosition == StartingPosition::TopRight) { currentColumn = grid->GetColumns() - 1; currentRow = grid->GetRows() - 1; }
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
		if (visitedCells[checkRow * totalColumns + checkColumn]) break;

		maxDistance++;
		checkColumn += directionX;
		checkRow += directionY;
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
	bool canTurnRight = false;

	int leftColumn = currentColumn + turnLeftX;
	int leftRow = currentRow + turnLeftY;
	if (leftColumn >= 0 && leftColumn < totalColumns && leftRow >= 0 && leftRow < totalRows)
		if (!visitedCells[leftRow * totalColumns + leftColumn])
			canTurnLeft = true;

	int rightColumn = currentColumn + turnRightX;
	int rightRow = currentRow + turnRightY;
	if (rightColumn >= 0 && rightColumn < totalColumns && rightRow >= 0 && rightRow < totalRows)
		if (!visitedCells[rightRow * totalColumns + rightColumn])
			canTurnRight = true;

	if (canTurnLeft && canTurnRight)
	{
		if (rand() % 2 == 0) { newDirectionX = turnLeftX;  newDirectionY = turnLeftY; }
		else { newDirectionX = turnRightX; newDirectionY = turnRightY; }
		return true;
	}
	else if (canTurnLeft) { newDirectionX = turnLeftX;  newDirectionY = turnLeftY;  return true; }
	else if (canTurnRight) { newDirectionX = turnRightX; newDirectionY = turnRightY; return true; }

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
		if (visitedCells[finalCheckRow * totalColumns + finalCheckColumn]) return true;
		finalCheckColumn += directionX;
		finalCheckRow += directionY;
	}

	return false;
}

void LevelGenerator::PlaceDecoyMirrors(Grid* grid, std::vector<bool>& visitedCells, int numberOfDecoys)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();
	int decoysPlaced = 0;
	int currentAttempts = 0;
	int maximumAttempts = numberOfDecoys * 10;

	while (decoysPlaced < numberOfDecoys && currentAttempts < maximumAttempts)
	{
		int randomColumn = rand() % totalColumns;
		int randomRow = rand() % totalRows;
		int targetIndex = randomRow * totalColumns + randomColumn;

		if (!visitedCells[targetIndex])
		{
			grid->AddMirror(randomColumn, randomRow);
			visitedCells[targetIndex] = true;
			decoysPlaced++;
		}
		currentAttempts++;
	}
}

// ---------------------------------------------------------------
//  PLACE DECOYS
//  Called after BOTH paths are generated so decoys avoid
//  both laser 1 and laser 2 beam cells.
// ---------------------------------------------------------------
void LevelGenerator::PlaceDecoys(Grid* grid, int numDecoys)
{
	int totalColumns = grid->GetColumns();
	int totalRows = grid->GetRows();
	int totalCells = totalColumns * totalRows;

	// Build exclusion set: path1 cells OR path2 cells OR existing mirrors
	std::vector<bool> blocked(totalCells, false);
	for (int idx = 0; idx < totalCells; ++idx)
	{
		bool p1 = (idx < static_cast<int>(m_Path1Cells.size())) && m_Path1Cells[idx];
		bool p2 = (idx < static_cast<int>(m_Path2Cells.size())) && m_Path2Cells[idx];
		blocked[idx] = p1 || p2;
	}
	// Also mark cells that already have mirrors (path mirrors)
	for (int row = 0; row < totalRows; ++row)
		for (int col = 0; col < totalColumns; ++col)
			if (grid->GetCellFromIndex(col, row)->HasMirror())
				blocked[row * totalColumns + col] = true;

	PlaceDecoyMirrors(grid, blocked, numDecoys);
}