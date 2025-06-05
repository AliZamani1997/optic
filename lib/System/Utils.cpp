#include "Arduino.h"
#include <vector>

std::vector<String> String2Vector(const String& input , const char * split) {
	std::vector<String> result;
	int start = 0;
	int commaIndex = 0;
	while ((commaIndex = input.indexOf(split, start)) != -1) {
		result.push_back(input.substring(start, commaIndex));
		start = commaIndex + 1;
	}
	if (start < input.length()) {
		result.push_back(input.substring(start));
	}
	return result;
}