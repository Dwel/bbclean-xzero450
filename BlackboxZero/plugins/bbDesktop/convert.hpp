
 // File: convert.hpp
 #include <iostream>
 #include <sstream>
 #include <string>
 #include <stdexcept>

 class BadConversion : public std::runtime_error {
 public:
  BadConversion(const std::string& s)
     : std::runtime_error(s)
     { }
 };

 inline double ConvertToDouble(const std::string& s,  bool failIfLeftoverChars = true)
 {
   std::istringstream i(s);
   double x;
   char c;
   if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
     throw BadConversion("convertToDouble(\"" + s + "\")");
   return x;
 }

inline int ConvertToInteger(const std::string& s,  bool failIfLeftoverChars = true)
 {
   std::istringstream i(s);
   int x;
   char c;
   if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
     throw BadConversion("convertToDouble(\"" + s + "\")");
   return x;
 }

inline std::string ConvertToString(int& i)
{
	std::stringstream s;
	s << i;
	return(s.str());
}

inline std::string ConvertToString(const char* st)
{
	std::stringstream s;
	s << st;
	return(s.str());
}
