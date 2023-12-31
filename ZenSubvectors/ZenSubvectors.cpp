#include <iostream>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <map>

void printVector(std::vector<int> vect)
{
	std::cout << "{";
	for (auto item : vect)
	{
		std::cout << item << ", ";
	}
	std::cout << '}' << std::endl;
}

// n^3 complexity
void findLongestConsecutiveMatching_INEFFICIENT(
	const std::vector<int>& vector1,
	const std::vector<int>& vector2,
	int& longestLengthFound,
	int& iLongestStartPositionVect1,
	int& iLongestStartPositionVect2)
{
	// find the longest consecutive correct items, remove from both, return 1 + minPieces( modified )
	longestLengthFound = -1;
	iLongestStartPositionVect1 = -1;
	iLongestStartPositionVect2 = -1;
	for (int i = 0; i < vector1.size(); i++)
	{
		// std::cout << "Outer it - i: " << i << " vector1[i]: " << vector1[i] << std::endl;
		for (int j = 0; j < vector2.size(); j++)
		{
			// std::cout << "Inner it - i: " << i << " vector1[i]: " << vector1[i] << std::endl;
			// std::cout << "Inner it - j: " << j << " vector2[j]: " << vector2[j] << std::endl;
			int currentLength = 0;
			int iSequence = i;
			int jSequence = j;
			// found start of a matching subsequence
			while (iSequence < vector1.size()
				&& jSequence < vector2.size()
				&& vector1[iSequence] == vector2[jSequence])
			{
				// std::cout << "Inner while it - currentOriginalPosition: " << iSequence << " vector1[currentOriginalPosition]: " << vector1[iSequence] << std::endl;
				// std::cout << "Inner while it - currentDesiredPosition: " << jSequence << " vector2[currentDesiredPosition]: " << vector2[jSequence] << std::endl;
				currentLength++;
				iSequence++;
				jSequence++;
			}

			// std::cout << "currentLength: " << currentLength << std::endl;
			// std::cout << "currentOriginalPosition: " << iSequence << std::endl;
			// std::cout << "currentDesiredPosition: " << jSequence <<  std::endl;

			if (currentLength > longestLengthFound)
			{
				// new longestLength
				longestLengthFound = currentLength;
				iLongestStartPositionVect1 = i;
				iLongestStartPositionVect2 = j;

				// std::cout << "NEW currentLongestLength: " << longestLengthFound << std::endl;
				// std::cout << "NEW currentLongestOriginalPosition: " << iLongestStartPositionVect1 << std::endl;
				// std::cout << "NEW currentLongestDesiredPosition: " << iLongestStartPositionVect2 << std::endl;
			}
		}
	}
}

// n^2 complexity
void findLongestConsecutiveMatching(
	const std::vector<int>& vector1,
	const std::vector<int>& vector2,
	int& longestLengthFound,
	int& iLongestStartPositionVect1,
	int& iLongestStartPositionVect2)
{
	int vector1Size = vector1.size();
	int vector2Size = vector2.size();

	std::vector<std::vector<int>> longestCommonSuffixLengths(vector1Size + 1, std::vector<int>(vector2Size + 1, 0));

	longestLengthFound = 0;
	iLongestStartPositionVect1 = -1;
	iLongestStartPositionVect2 = -1;

	for (int i = 1; i <= vector1Size; ++i) 
	{
		for (int j = 1; j <= vector2Size; ++j) 
		{
			if (vector1[i - 1] == vector2[j - 1]) 
			{
				longestCommonSuffixLengths[i][j] = longestCommonSuffixLengths[i - 1][j - 1] + 1;
				if (longestCommonSuffixLengths[i][j] > longestLengthFound) 
				{
					longestLengthFound = longestCommonSuffixLengths[i][j];
					iLongestStartPositionVect1 = i - longestLengthFound;
					iLongestStartPositionVect2 = j - longestLengthFound;
				}
			}
		}
	}
}

// n^3 complexity - n complexity recursion + n^2 findLongestConsecutiveMatching
int minPieces_INEFFICIENT(const std::vector<int>& original, const std::vector<int>& desired)
{
	// we may have multiple candidates, some with different number of cuts
	/*
	recursive:
	if original == desired, return 0

	find the longest consecutive correct items, remove them from both, return 1 + minPieces
	*/

	// std::cout << "minPieces called, printing vectors" << std::endl;
	// std::cout << "Original: " << std::endl;
	//printVector(original);
	// std::cout << "Desired: " << std::endl;
	//printVector(desired);

	if (original.size() == 0 || original == desired)
	{
		return 0; // no cuts needed, ie paper remains as 1 intact strip
	}
	else
	{
		int longestFound;
		int longestOriginalStartPos;
		int longestDesiredStartPos;
		findLongestConsecutiveMatching(
			original,
			desired,
			longestFound,
			longestOriginalStartPos,
			longestDesiredStartPos
		);

		// now we make a cut of the longest sequence, recurse
		// first handle erros

		if (longestFound < 0 || longestOriginalStartPos < 0 || longestDesiredStartPos < 0)
		{
			throw std::runtime_error("Failed to findLongestConsecutiveMatching");
		}

		std::vector<int> originalLeft, originalRight;
		originalLeft.assign(original.begin(), original.begin() + longestOriginalStartPos);
		originalRight.assign(original.begin() + longestOriginalStartPos + longestFound, original.end());

		return 1 + minPieces_INEFFICIENT(originalLeft, desired) + minPieces_INEFFICIENT(originalRight, desired);
	}
}

// n complexity
int minPieces_GOOD(const std::vector<int>& original, const std::vector<int>& desired)
{
	// count the number of cuts
	std::map<int, int> positions;
	for (int i = 0; i < original.size(); i++)
	{
		positions[original[i]] = i;
	}

	int cuts = 0;
	int i = 0;
	// for each number in `desired`, find it's position in `original` using the `positions` map 
	while (i < desired.size())
	{
		int current = positions[desired[i]];	// `desired` mapped to `original`
		i++;
		while (i < desired.size())
		{
			if (positions[desired[i]] == current + 1)
			{
				current = positions[desired[i]];
				i++;
			}
			else
			{
				break;
			}
		}	// when the while loop ends, we no longer have consecutive values in both, so time to make a cut
		cuts++;
	}

	return cuts;
}

int minPieces_RECURSIONBROKEN(const std::vector<int>& original, const std::vector<int>& desired)
{
	if (desired.empty())
	{
		return 0;   // base case for recursion
	}

	int firstElement = desired[0];
	auto iter = std::find(original.begin(), original.end(), firstElement);

	if (iter == original.end())
	{
		return -1; // error.. 
	}

	std::vector<int> origLeft(original.begin(), iter);
	std::vector<int> origRight(iter + 1, original.end());

	int splitPoint = std::distance(original.begin(), iter);
	std::vector<int> desiredLeft, desiredRight;

	for (int i = 1; i < desired.size(); ++i)
	{
		auto posInOriginal = std::find(original.begin(), original.end(), desired[i]);
		int dist = std::distance(original.begin(), posInOriginal);
		if (dist < splitPoint)
		{
			desiredLeft.push_back(desired[i]);
		}
		else
		{
			desiredRight.push_back(desired[i]);
		}

	}

	return 1 + minPieces_RECURSIONBROKEN(origLeft, desiredLeft) + minPieces_RECURSIONBROKEN(origRight, desiredRight);
}

int minPieces_FINALBROKEN(const std::vector<int>& original, const std::vector<int>& desired)
{
	std::map<int, int> indexMap;
	for (int i = 0; i < original.size(); ++i)
	{
		indexMap[original[i]] = i;
	}

	std::vector<int> transformedDesired;
	for (int num : desired)
	{
		transformedDesired.push_back(indexMap[num]);
	}

	int cuts = 0;
	int maxSoFar = transformedDesired[0];
	for (int i = 1; i < transformedDesired.size(); ++i)
	{
		if (transformedDesired[i] < maxSoFar)
		{
			cuts++;
		}
		else
		{
			maxSoFar = transformedDesired[i];
		}
	}

	return cuts + 1;
}

// Final, fixed
int minPieces(const std::vector<int>& original, const std::vector<int>& desired)
{	
	std::map<int, int> indexMap;
	for (int i = 0; i < original.size(); ++i)
	{
		indexMap[original[i]] = i;
	}

	std::vector<int> transformedDesired;
	for (int num : desired)
	{
		transformedDesired.push_back(indexMap[num]);
	}

	int cuts = 0;
	for (int i = 1; i < transformedDesired.size(); ++i)
	{
		// 0,1,2,4,5,3
		
		if (transformedDesired[i] != transformedDesired[i-1] + 1)
		{
			cuts++;
		}
	}

	return cuts + 1;
}

int main()
{
	// Original Scenario
	{
		std::vector<int> original = { 1, 4, 3, 2 };
		std::vector<int> desired = { 1, 2, 4, 3 };
		// (2) (4, 3) (2)
		int result = minPieces(original, desired);
		std::cout << "Original Scenario Result: " << result << std::endl;
		assert(result == 3);
	}

	// Scenario 1: Same Order
	{
		std::vector<int> original = { 1, 2, 3, 4, 5 };
		std::vector<int> desired = { 1, 2, 3, 4, 5 };
		// (1, 2, 3, 4, 5)
		int result = minPieces(original, desired);
		std::cout << "Scenario 1 Result: " << result << std::endl;
		assert(result == 1); // a bit of ambiguity here - it's 1 piece but 0 "cut pieces" as the question phrases it
	}

	// Scenario 2: Reversed Order
	{
		std::vector<int> original = { 1, 2, 3, 4, 5 };
		std::vector<int> desired = { 5, 4, 3, 2, 1 };
		// (1), (2), (3), (4), (5)
		int result = minPieces(original, desired);
		std::cout << "Scenario 2 Result: " << result << std::endl;
		assert(result == 5);
	}

	// Scenario 3: Single Swap
	{
		std::vector<int> original = { 1, 2, 4, 3, 5 };
		std::vector<int> desired = { 1, 2, 3, 4, 5 };
		// (1, 2), (3), (4), (5)
		int result = minPieces(original, desired);
		std::cout << "Scenario 3 Result: " << result << std::endl;
		assert(result == 4);
	}

	// Scenario 4: Multiple Swaps
	{
		std::vector<int> original = { 1, 3, 2, 5, 4 };
		std::vector<int> desired = { 1, 2, 3, 4, 5 };
		// (1), (2), (3), (4), (5)
		int result = minPieces(original, desired);
		std::cout << "Scenario 4 Result: " << result << std::endl;
		assert(result == 5);
	}

	// Scenario 5: Non-consecutive Elements
	{
		std::vector<int> original = { 1, 5, 3, 7, 4, 2, 6 };
		std::vector<int> desired = { 1, 2, 3, 4, 5, 6, 7 };
		// (1) (5) (3) (7) (4) (2) (6)
		int result = minPieces(original, desired);
		std::cout << "Scenario 5 Result: " << result << std::endl;
		assert(result == 7);
	}

	// Scenario 6: Edge Cases
	// NOTE: lots of ambiguity around these edge cases - some implementations may have different opinions
	{
		std::vector<int> original = {};
		std::vector<int> desired = {};
		int result = minPieces(original, desired);
		std::cout << "Scenario 6 (empty) Result: " << result << std::endl;
		assert(result == 1);

		original = { 1 };
		desired = { 1 };
		result = minPieces(original, desired);
		std::cout << "Scenario 6 (single element) Result: " << result << std::endl;
		assert(result == 1);
	}

}