#include "ini_file.h"

#include "utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include <exception>
#include <stdexcept>

using namespace std;

bool ini_file::load_ini(const string & filename)
{
	filename_ = filename;
	data_.clear();
	ifstream in_file;
	in_file.open(filename_.c_str());

	if (!in_file)
	{
		return false;
	}

	string line_str;
	string section, key, value;
	string cur_section;
	while (getline(in_file, line_str))
	{
		// 去除两边空白字符
        Utils::trim(line_str);

		// if this line is a comment line ,do nothing
		if (is_comment(line_str))
		{
			continue;
		}

		// parse if this line is a section line
		if (parse_section(line_str,section))
		{
			cur_section = section;
			continue;
		}

		if (cur_section.empty())
			continue;
			
		// parse the key and value
		if (parse_key_value(line_str, key, value))
		{
			data_[cur_section][key] = value;
		}
	}

	in_file.close();
	return true;
}

bool ini_file::parse_section(const string & line_str, string & section)
{
	string::size_type pos1 = line_str.find('[');

	if (pos1 != 0)
	{
		return false;
	}

	string::size_type pos2 = line_str.find(']', pos1 + 1);

	if (pos2 == string::npos)
	{
		return false;
	}

	string::size_type sec_len = pos2 - pos1 - 1;

	if (sec_len == 0)
	{
		return false;
	}

	section = line_str.substr(pos1 + 1, sec_len);

    Utils::trim(section);
	return true;
}

bool ini_file::parse_key_value(const string & line_str, string & key, string & value)
{
	string::size_type pos1 = line_str.find('=');

	if (pos1 == string::npos || pos1 == 0 || pos1 == line_str.size() - 1)
	{
		return false;
	}

	key = line_str.substr(0, pos1);
	value = line_str.substr(pos1 + 1);

    Utils::trim(key);
    Utils::trim(value);
	return true;
}

bool ini_file::is_comment(const string & line_str)
{
	string::size_type pos = line_str.find("\\\\");

	if (pos != 0)
	{
		return false;
	}

	return true;
}

string ini_file::read_many_string(const string & section, const string & key) const
{
	SECTION_MAP::const_iterator citer = data_.find(section);
	if (citer == data_.end())
	{
		std::cout << "ini_file read warn, section = " << section
			<< " key = " << key << " not found!" << std::endl;
		return "not found"; //string();
	}

	KEY_VALUE_MAP::const_iterator citer2 = citer->second.find(key);

	if (citer2 == citer->second.end())
	{
		return "not found"; //string();
	}

	return citer2->second;
}

string ini_file::read_string(const string & section, const string & key) const
{
	SECTION_MAP::const_iterator citer = data_.find(section);

	if (citer == data_.end())
	{
		stringstream ss;
		ss << "ini_file sec : " << section << " key : " << key << " not found";
        throw runtime_error(ss.str());
	}

	KEY_VALUE_MAP::const_iterator citer2 = citer->second.find(key);

	if (citer2 == citer->second.end())
	{
		stringstream ss;
		ss << "ini_file sec : " << section << " key : " << key << " not found";
		throw runtime_error(ss.str());
	}

	return citer2->second;
}

int ini_file::read_int(const string & section, const string & key) const
{
	string str_val = read_string(section, key);

	if (str_val.empty())
	{
		stringstream ss;
		ss << "ini_file sec : " << section << " key : " << key << " not found";
		throw runtime_error(ss.str());
	}
	else
	{
        int val = 0;
        stringstream sss;
        sss << str_val;
        sss >> val;
		return val;
	}
}

float ini_file::read_float(const string & section, const string & key) const
{
	string str_val = read_string(section, key);

	if (str_val.empty())
	{
		stringstream ss;
		ss << "ini_file sec : " << section << " key : " << key << " not found";
		throw runtime_error(ss.str());
	}
	else
	{
        float val = 0;
        stringstream sss;
        sss << str_val;
        sss >> val;
		return val;
	}
}



string ini_file::read_string(const string & section, const string & key, const string & def) const
{
	try
	{
		return read_string(section, key);
	}
	catch (runtime_error &)
	{
		return def;
	}
}

int ini_file::read_int(const string & section, const string & key, int def) const
{
	try
	{
		return read_int(section, key);
	}
	catch (runtime_error &)
	{
		return def;
	}
}

float ini_file::read_float(const string & section, const string & key, float def) const
{
	try
	{
		return read_float(section, key);
	}
	catch (runtime_error &)
	{
		return def;
	}
}

void ini_file::set_val(const string & section, const string &key, const string &value)
{
    string sec = section;
    string kk = key;
    string val = value;
	data_[Utils::trim(sec)][Utils::trim(kk)] = Utils::trim(val);
}

void ini_file::set_val(const string & section, const string &key, int value)
{
    string val;
    stringstream sss;
    sss << value;
    sss >> val;
	set_val(section, key, val);
}

void ini_file::set_val(const string & section, const string &key, float value)
{
    string val;
    stringstream sss;
    sss << value;
    sss >> val;
	set_val(section, key, val);
}

// 删除
void ini_file::del_sec(const string & section)
{
	SECTION_MAP::iterator iter = data_.find(section);
	if (iter != data_.end())
		data_.erase(iter);
}

void ini_file::del_key(const string & section, const string & key)
{
	SECTION_MAP::iterator iter = data_.find(section);
	if (iter != data_.end())
	{
		KEY_VALUE_MAP & key_values = iter->second;
		KEY_VALUE_MAP::iterator iter2 = key_values.find(key);

		if (iter2 != key_values.end())
			key_values.erase(iter2);
	}
}

bool ini_file::write_ini(const string &filename) const
{
	ofstream out_file;
	out_file.open(filename.c_str());

	if (!out_file)
	{
		return false;
	}

	SECTION_MAP::const_iterator citer = data_.begin();

	for (; citer != data_.end(); ++citer)
	{
		const string &section = citer->first;
		const KEY_VALUE_MAP &key_value =citer->second;

		out_file << "[" << section << "]" << endl;
		KEY_VALUE_MAP::const_iterator citer2= key_value.begin();

		for (; citer2 != key_value.end(); ++citer2)
		{
			const string & key = citer2->first;
			const string & value = citer2->second;

			out_file << key << "=" << value << endl;
		}

		out_file << endl;
	}

	out_file.close();

	return true;
}

bool ini_file::write_ini() const
{
	return write_ini(filename_);
}

const KEY_VALUE_MAP * ini_file::get_key_value_map(const string & sec) const
{
	SECTION_MAP::const_iterator citer = data_.find(sec);

	if (citer == data_.end())
	{
		return NULL;
	}
	else
	{
		return &citer->second;
	}
}


// 将数据输出到终端，测试用
void ini_file::dump() const
{
	SECTION_MAP::const_iterator citer = data_.begin();

	for (; citer != data_.end(); ++citer)
	{
		std::cout << "[" << citer->first << "]" << std::endl;
		
		KEY_VALUE_MAP::const_iterator citer2 = citer->second.begin();

		for (; citer2 != citer->second.end(); ++ citer2)
		{
			std::cout << citer2->first << "=" << citer2->second << std::endl;
		}
	}
}
