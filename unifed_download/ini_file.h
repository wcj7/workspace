/*
* ==============================================================================
*
* Version: 1.0
* Date: 2014.06.27
*
* Author: Alex Xing
* Company: EasyWay
*
* Description: 
*
* ==============================================================================
*/

#pragma once
#include <string>
#include <map>
using std::map;
using std::string;

typedef map<string, string> KEY_VALUE_MAP;
typedef map<string, KEY_VALUE_MAP > SECTION_MAP;
class ini_file
{
public:
	ini_file(void){}
	~ini_file(void){}

	// 读取
	bool load_ini(const string & filename);

	// 读取键值， 不存在时抛异常easy_exception
	string read_many_string(const string & section, const string & key) const;
	string read_string(const string & section, const string & key) const;
	int read_int(const string & section, const string & key) const;
	float read_float(const string & section, const string & key) const;

	// 读取键值,不能存在时使用默认值返回
	string read_string(const string & section, const string & key, const string & def) const;
	int read_int(const string & section, const string & key, int def) const;
	float read_float(const string & section, const string & key, float def) const;

	// 设置值
	void set_val(const string & section, const string & key, const string & value);
	void set_val(const string & section, const string & key, int value);
	void set_val(const string & section, const string & key, float value);

	// 删除
	void del_sec(const string & section);
	void del_key(const string & section, const string & key);

	// 写入到文件
	bool write_ini(const string &filename) const;
	bool write_ini() const;
	
	// 获取键
	const KEY_VALUE_MAP * get_key_value_map(const string & sec) const;
	
	// 获取整个文件数据
	const SECTION_MAP & get_section_map() const { return data_;}

	// 将数据输出到终端，测试用
	void dump() const;
private:
	// 解析section
	static bool parse_section(const string & line_str, string & section);

	// 解析key与value
	static bool parse_key_value(const string & line_str, string & key, string & value);

	// 判断是否为注释
	static bool is_comment(const string & line_str);
private:
	// 文件名
	string filename_;

	//ini数据
	// map<section, map<key value> >
	SECTION_MAP data_;
};
