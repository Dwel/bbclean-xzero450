#include "index.h"
#include "config.h"
#include "search.h"
#include <fstream>
#include <blackbox/BBApi.h>
#include "serialize.h"

namespace bb { namespace search {

void makeIndex (trie_t & trie, props_t & props, Config const & cfg, tstring const & fname)
{
	try
	{
		for (SearchLocationInfo const & info : cfg.m_locations)
		{
			SearchDirectory(
				  info.m_dir_path, info.m_includes, info.m_excludes, info.m_recursive, info.m_follow_symlinks
				, TEXT("")
				, [] (tstring const & fname, tstring const & cmp) { return true; }
				, [&trie, &props] (tstring const & fname, tstring const & fpath)
					 {
						props.push_back(Props(fname, fpath));
						//printf("update: fname=%s fpath=%s\n", fname.c_str(), fpath.c_str());
						trie_t::result_type const id = static_cast<trie_t::result_type>(props.size() - 1);
						trie.update(fname.c_str(), fname.length(), id);
					 });
		}
	}
	catch (std::regex_error const & e)
	{
		dbg_printf("Exception caught: %s", e.what());
	}

	printf("keys: %ld\n", trie.num_keys ());
	printf("size: %ld\n", trie.size ());
}

bool saveIndex (trie_t const & t, tstring const & fpath)
{
	if (t.save(fpath.c_str()) != 0)
	{
		// err
		return false;
	}
	return true;
}

bool loadIndex (trie_t & t, tstring const & fpath)
{
	return t.open(fpath.c_str()) == 0;
}

bool loadProps (props_t & t, tstring const & fpath)
{
	std::fstream f(fpath.c_str(), std::ios::in | std::ios::binary);
	if (!f)
		return false;

	unsigned v = 0;
	if (read(f, v).fail())
	{
		f.close();
		return false;
	}
	if (v != version11)
	{
		f.close();
		return false;
	}

	if (read(f, t).fail())
	{
		f.close();
		return false;
	}

	f.close();
	return true;
}

bool saveProps (props_t const & t, tstring const & fpath)
{
	std::fstream f(fpath.c_str(), std::ios::out | std::ios::binary);
	if (!f)
		return false;

	if (write(f, version11).fail())
	{
		f.close();
		return false;
	}
	if (write(f, t).fail())
	{
		f.close();
		return false;
	}
	f.close();
	return true;
}

}}
