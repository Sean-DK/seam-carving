#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int getSeamEnergy(const std::vector<std::vector<int>>& vec, const std::vector<std::vector<int>>& vec2, const int& x, const int& y) {
	if (y == 0)
		return vec[y][x];
	else {
		//not an edge case
		if (x > 0 && x < vec[0].size() - 1) {
			return vec[y][x] + std::min(std::min(vec2[y - 1][x - 1], vec2[y - 1][x]), vec2[y - 1][x + 1]);
		}
		//edge case
		if (x == 0) {
			return vec[y][x] + std::min(vec2[y - 1][x], vec2[y - 1][x + 1]);
		}
		//edge case
		if (x == vec[0].size() - 1) {
			return vec[y][x] + std::min(vec2[y - 1][x - 1], vec2[y - 1][x]);
		}
	}
}

std::vector<int> findPath(std::vector<std::vector<int>> vec) {
	std::vector<int> path;
	int y = vec.size();
	int x = vec[y - 1].size();
	int minValue = 9999999;
	int minIndex = -1;
	int prevIndex = -1;
	for (int i = 0; i < x; i++)
		if (vec[y - 1][i] < minValue) {
			minValue = vec[y - 1][i];
			minIndex = i;
		}
	path.push_back(minIndex);
	prevIndex = minIndex;
	y--;
	do {
		minValue = 9999999;
		//check y - 1, x - 1
		if (prevIndex - 1 >= 0)
			if (vec[y - 1][prevIndex - 1] < minValue) {
				minValue = vec[y - 1][prevIndex - 1];
				minIndex = prevIndex - 1;
			}
		//check y - 1, x
		if (vec[y - 1][prevIndex] < minValue) {
			minValue = vec[y - 1][prevIndex];
			minIndex = prevIndex;
		}
		//check y - 1, x + 1
		if (prevIndex + 1 < x)
			if (vec[y - 1][prevIndex + 1] < minValue) {
				minValue = vec[y - 1][prevIndex + 1];
				minIndex = prevIndex + 1;
			}
		path.push_back(minIndex);
		prevIndex = minIndex;
		y--;
	} while (y > 0);
	return path;
}

//split a string with a specified delimiter and return a vector of all substrings
std::vector<std::string> stringsplit(const std::string& s, const std::string& delim) {
	std::vector<std::string> result; //create a vector to be returned
	std::string str = s; //create a modifiable copy of the string
	do {
		if (delim == "" || str == "") return result; //if the string or delimiter are empty, return an empty vector
		else if (str.find(delim) == 0) str.erase(str.begin()); //if there is a delimiter at the beginning of the string, remove it
		else if (str.find(delim) > 0 && str.find(delim) < 32767) { //if there is a delimiter in the string
			result.push_back(str.substr(0, str.find(delim))); //add substring to the vector
			str.erase(str.begin(), str.begin() + str.find(delim) + delim.size()); //erase the substring from the string
		}
		else { //if there is no delimiter in the string
			result.push_back(str); //add the entire string to the vector
			return result; //return vector
		}
	} while (1);
}

int main(int argc, char* argv[])
{
	int verCuts = 20;
	int attempts = 0;
	//read image file
	std::string str, input;
	std::ifstream file;
	file.open("Images\\bug.pgm");
	//skip the first two lines
	std::getline(file, input);
	std::getline(file, input);
	//get dimensions for arrays
	std::getline(file, input);
	std::vector<std::string> vec;
	vec = stringsplit(input, " ");
	int x, y;
	x = std::stoi(vec.at(0));
	y = std::stoi(vec.at(1));
	//create the vectors
	//vector with .pgm pixel data
	std::vector<std::vector<int>> pgmVec(y, std::vector<int>(x));
	//vector with pixel energy data
	std::vector<std::vector<int>> energyVec(y, std::vector<int>(x));
	//vector with cumulative seam energy
	std::vector<std::vector<int>> verVec(y, std::vector<int>(x));
	std::vector<std::vector<int>> horVec(y, std::vector<int>(x));
	//vector for path of seam
	std::vector<int> path;
	//get the max greyscale value
	std::getline(file, input);
	int maxVal = std::stoi(input);
	//load pixel data into vector
	while (std::getline(file, input))
		str += input + " ";
	vec = stringsplit(str, " ");
	//close image file
	file.close();
	//push the vector into the other vector
	for (int i = 0; i < y; i++)
		for (int j = 0; j < x; j++)
			pgmVec[i][j] = std::stoi(vec[(i * x) + j]);

//delete a vertical seam
	do {
		path.clear();
		//calculate the energy of all pixels
		int sum = 0;
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				if (i > 0)
					sum += abs(pgmVec[i][j] - pgmVec[i - 1][j]);
				if (i < y - 1)
					sum += abs(pgmVec[i][j] - pgmVec[i + 1][j]);
				if (j > 0)
					sum += abs(pgmVec[i][j] - pgmVec[i][j - 1]);
				if (j < x - 1)
					sum += abs(pgmVec[i][j] - pgmVec[i][j + 1]);
				energyVec[i][j] = sum;
				sum = 0;
			}
		}
		//find the cumulative energy seams
		for (int i = 0; i < y; i++)
			for (int j = 0; j < x; j++)
				verVec[i][j] = getSeamEnergy(energyVec, verVec, j, i);
		//find the path of the minimum seam
		path = findPath(verVec);



		//output bullshit
		std::cout << "\n----\n";
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++)
				std::cout << pgmVec[i][j] << " ";
			std::cout << "\n\n";
		}
		std::cout << "----\n";
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++)
				std::cout << energyVec[i][j] << " ";
			std::cout << "\n\n";
		}
		std::cout << "----\n";
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++)
				std::cout << verVec[i][j] << " ";
			std::cout << "\n\n";
		}
		std::cout << "----\n";
		for (int i = 0; i < y; i++) {
			std::cout << path[i] << " ";
		}
		////////////////////////////////////////////////



		//delete the seam
		for (int i = 0; i < y; i++)
			pgmVec[y - 1 - i].erase(pgmVec[y - 1 - i].begin() + path[i]);
		for (int i = 0; i < y; i++)
			energyVec[y - 1 - i].erase(energyVec[y - 1 - i].begin() + path[i]);
		for (int i = 0; i < y; i++)
			verVec[y - 1 - i].erase(verVec[y - 1 - i].begin() + path[i]);
		attempts++;
		x--;
	} while (attempts < verCuts);
//loop again

//delete a horizontal seam








	//output to file
	std::ofstream fileOut;
	fileOut.open("pgm.pgm");
	fileOut << "P2\n";
	fileOut << "#...\n";
	fileOut << pgmVec[0].size() << " " << pgmVec.size() << "\n";
	fileOut << maxVal << "\n";
	for (int i = 0; i < pgmVec.size(); i++) {
		for (int j = 0; j < pgmVec[0].size(); j++)
			fileOut << pgmVec[i][j] << " ";
	}


    return 0;
}

/*
//output the vector
for (int i = 0; i < y; i++) {
	for (int j = 0; j < x; j++)
		std::cout << pgmVec[i][j] << " ";
	std::cout << "\n";
}
*/

/*
int getSeamEnergy2(const std::vector<std::vector<int>>& vec, const int& x, const int& y, const int& maxSize) {
int result;
do {
if (y < 0)
return result;

}
}
*/