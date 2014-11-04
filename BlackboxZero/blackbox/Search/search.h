#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include <windows.h>
#include <boost/algorithm/string/predicate.hpp>

struct SearchLocationInfo
{
	tstring m_dir_path;
	std::vector<tstring> m_includes;
	std::vector<tstring> m_excludes;
	bool m_recursive;
	bool m_follow_symlinks;

	SearchLocationInfo () { }

	SearchLocationInfo (tstring const & dir, tstring const & includes, tstring const & excludes, bool rec)
		: m_dir_path(dir)
		, m_includes()
		, m_excludes()
		, m_recursive(rec)
		, m_follow_symlinks(false)
	{
		std::vector<tstring> tok;
		split(tok, includes, boost::is_any_of(L";"));
		for(std::vector<tstring>::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
			m_includes.push_back(*tok_iter);
		tok.clear();
		split(tok, excludes, boost::is_any_of(L";\n"));
		for (std::vector<tstring>::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
		{
			tstring tmp(*tok_iter);
			boost::trim_right_if(tmp, boost::is_any_of(L"\\"));
			m_excludes.push_back(tmp);
		}
	}
};

bool matchExclude (std::vector<tstring> const & excludes, tstring const & file_name)
{
	for (tstring const & ws : excludes)
	{
		if (boost::iequals(ws, file_name))
			return true;
	}
	return false;
}

inline int SearchDirectory (tstring const & dir_path, std::vector<tstring> const & includes, std::vector<tstring> const & excludes, bool recursive, bool follow_symlinks
				, tstring const & file_name
				, std::vector<tstring> & matches)
{
	tstring filePath;
	tstring pattern = dir_path;
	if (!boost::algorithm::ends_with(pattern, TEXT("\\")))
		pattern += TEXT("\\");
	pattern += TEXT("*.*");

	WIN32_FIND_DATA fi;
	HANDLE hFile = ::FindFirstFile(pattern.c_str(), &fi);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fi.cFileName[0] != '.')
			{
				filePath.erase();
				filePath = dir_path;
				if (!boost::algorithm::ends_with(filePath, TEXT("\\")))
					filePath += TEXT("\\");
				filePath += fi.cFileName;

				if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // @TODO: follow symlinks
				{
					if (recursive)
					{
						if (!matchExclude(excludes, filePath))
						{
							// descend into subdirectory
							if (int const iRC = SearchDirectory(filePath, includes, excludes, recursive, follow_symlinks, file_name, matches))
								return iRC;
						}
					}
				}
				else
				{
					if (boost::algorithm::iends_with(fi.cFileName, file_name))
						matches.push_back(filePath);
				}
			}
		}
		while(::FindNextFile(hFile, &fi) == TRUE);

		::FindClose(hFile);// Close handle

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
			return dwError;
	}

	return 0;
}

inline int SearchDirectory (SearchLocationInfo const & info, tstring const & file_name
				, std::vector<tstring> & matches)
{
	return SearchDirectory(info.m_dir_path, info.m_includes, info.m_excludes, info.m_recursive, info.m_follow_symlinks, file_name, matches);
}


