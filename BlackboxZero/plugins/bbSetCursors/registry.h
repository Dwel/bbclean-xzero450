/*
	Registry.h

    Author:     Ben M
	            Modified into oblivion by Noccy
    Date:       29-01-07 03:14
    Purpose:    Declarations for registry.cpp
*/


#ifndef REGISTRY_CLASS_HPP
#define REGISTRY_CLASS_HPP

#include <windows.h>
#include <string>

class registry {
public:
		/// Constructor. Takes a string specifying the name of the key to open.
			registry();

		/// Destructor. Closes the key when the variable goes out of scope.
			~registry();

	bool	openKey(HKEY root, char* key);
	bool	closeKey();

		/// Method that returns true if the key was opened successfully.
	bool	is_open( void );

		/// Method that queries the open key for a string.
		/// Returns true if successful. Note the & on the second param.
		/// Means it's pass by ref and the string goes there.
	bool	get_value( std::string leaf, std::string& result );
		/// Sets the new value to the registry key.
	bool	set_value( std::string leaf, std::string value );

private:
		/// Member variable holds the open key.
	HKEY          m_key;

};

#endif	//REGISTRY_CLASS_HPP

