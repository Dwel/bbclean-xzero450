/*
	Registry.cpp

    Author:     Ben M
	            Modified into oblivion by Noccy
    Date:       29-01-07 03:14
    Purpose:    Sets and updates the cursor theme
*/

#include "Registry.h"

/// Constructor
registry::registry() { }

/// Destructor.
registry::~registry() {
	// Call closeKey to close the key if open.
	(*this).closeKey();
}

bool registry::openKey(HKEY root, char* key) {
	// Open the key.
	if ( RegOpenKeyExA(root, key, 0, KEY_READ | KEY_WRITE, &m_key) != ERROR_SUCCESS ) {
		// Failed to open. Set to 0.
		m_key = 0;
		return(false);
	} else {
		return(true);
	}
}

bool registry::closeKey() {
	// Close the key if its opened
	if (m_key)
		RegCloseKey( m_key );
}

bool registry::is_open() {
	return(m_key != 0);
}

bool	registry::get_value( std::string leaf, std::string& result ) {
	if ( !is_open() ) {
		// We're not open, just return.
		return false;
	}

	DWORD type = REG_SZ;
	DWORD buffer_size = 1024;
	unsigned char buffer[1024];

	if ( RegQueryValueExA( m_key, leaf.c_str(), 0, &type, buffer, &buffer_size ) != ERROR_SUCCESS )	{
		// Failed to read for some reason, return false.
		return(false);
	}

	// If we're here, a valid string key has been read. buffer contains a null terminated string.
	// Chuck it in the result string and return.
	result = (char*)buffer;
	return(true);
}

bool	registry::set_value( std::string leaf, std::string value ) {
	if ( !is_open() ) {
		// We're not open, just return.
		return(false);
	}

	DWORD type = REG_SZ;

	if ( RegSetValueExA( m_key, leaf.c_str(), 0, type, (unsigned char*)value.c_str(), value.size()+1) != ERROR_SUCCESS ) {
		// Failed to write the new key for some reason, return false.
		return(false);
	}

	return(true);
}
