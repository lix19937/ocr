
#include <map>
#include <iostream>
#include <sstream>
#include "digital_to_chinese.h"

enum MONEYFLAG{ INT_ONLY, SMALL_ONLY };

using namespace std;

/**
* 数字转成字符串
* @param digital - 数字
*/
string digital_to_string(double digital);

/**
* 数字字符串转成大写字符串
* @param flag - 是整数部分还是小数部分
* @param strdigital - 数字字符串
*/
string string_to_chinese_implement(MONEYFLAG flag, string strdigital);

/**
* 使用给定的 replacement 替换此字符串所有匹配给定的 regex 的子字符串。
* @param src - 待操作的源字符串
* @param regex - 用来匹配此字符串的正则表达式
* @param replacement - 用来替换每个匹配项的字符串
* @return 替换后的字符串
*/
string replace_all(string src, string regex, string replacement);

/**
* 把已经转换好的中文金额大写形式加以改进，清理这个字
* 符串里面多余的零，让这个字符串变得更加可观
* 符串应该已经是用中文金额大写形式表示的
* @param s string 已经转换好的字符串
* @return 改进后的字符串
*/
string clean_zero(string s);

/**
* 把数字转为中文大写字符串
* @param digital 数字
* @param strresult1 大写字符
* @param strresult2 大写字符带整字
* @return 改进后的字符串
*/
//void string_to_chinese(double digital, string& strresult1, string& strresult2);

/**
* 把数字字符串转为中文大写字符串
* @param digital 数字
* @param strresult1 大写字符
* @param strresult2 大写字符带整字
* @return 改进后的字符串
*/
void string_to_chinese(string strdigital, string& strresult1, string& strresult2);

string digital_to_string(double digital)
{
	stringstream strdigital;    //流输出
	strdigital.str("");
	strdigital << std::fixed << digital;
	return strdigital.str();
}

string string_to_chinese_implement(MONEYFLAG flag, string strdigital)
{
	int stringlength = strdigital.length();

	// 货币大写形式  
	string uppercase[] = { "零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖" };
	// 货币单位  
	string moneyunit[] = { "元", "拾", "佰", "仟", "万", 
		"拾", "佰", "仟", // 拾万位到仟万位  
		"亿", "拾", "佰", "仟", "万" }; // 亿位到万亿位 
	//小数点后面的货币单位
	string smallunit[] = { "角", "分" };
	string strnewupper = "";

	for (int i = 0; i < stringlength; i++)
	{
		if (flag == INT_ONLY)
		{
			strnewupper = strnewupper + uppercase[strdigital.at(i) - 48] + moneyunit[stringlength - i - 1];
		}
		else if (flag == SMALL_ONLY && i < 2)
		{
			strnewupper = strnewupper + uppercase[strdigital.at(i) - 48] + smallunit[i];
		}		 
	}

	return strnewupper;
}

string replace_all(string src, string regex, string replacement) 
{
	int length = regex.length();
	while (src.find(regex) < src.length()) 
	{
		// 替换 src 字符串中从第一个匹配 regex 的字符串索引开始的 length 个字符为 replacement 字符串          
		src.replace(src.find(regex), length, replacement);
	}
	return src;
}

string clean_zero(string s) 
{
	// 如果传入的是空串则继续返回空串  
	if ("" == s) 
	{
		return "";
	}
	// 字符串中存在多个'零'在一起的时候只读出一个'零'，并省略多余的单位  
	/* 由于本人对算法的研究太菜了，只能用4个正则表达式去转换了，各位大虾别介意哈... */
	string regex1[] = { "零仟", "零佰", "零拾" };
	string regex2[] = { "零亿", "零万", "零元" };
	string regex3[] = { "亿", "万", "元" };
	string regex4[] = { "零角", "零分" };

	// 第一轮转换把 "零仟", 零佰","零拾"等字符串替换成一个"零"  
	for (int i = 0; i < 3; i++) 
	{
		s = replace_all(s, regex1[i], "零");
	}

	// 第二轮转换考虑 "零亿","零万","零元"等情况  
	// "亿","万","元"这些单位有些情况是不能省的，需要保留下来  
	for (int i = 0; i < 3; i++) 
	{
		// 当第一轮转换过后有可能有很多个零叠在一起  
		// 要把很多个重复的零变成一个零  
		s = replace_all(s, "零零零", "零");
		s = replace_all(s, "零零", "零");
		s = replace_all(s, regex2[i], regex3[i]);
	}

	// 第三轮转换把"零角","零分"字符串省略  
	for (int i = 0; i < 2; i++) 
	{
		s = replace_all(s, regex4[i], "");
	}

	// 当"万"到"亿"之间全部是"零"的时候，忽略"亿万"单位，只保留一个"亿"  
	s = replace_all(s, "亿万", "亿");
	return s;
}

void string_to_chinese(double digital, string& strresult1, string& strresult2)
{
	string strdigital = digital_to_string(digital);
	cout << std::fixed << strdigital << endl;

	string_to_chinese(strdigital, strresult1, strresult2);
}  

void string_to_chinese(string strdigital, string& strresult1, string& strresult2)
{
	if (strdigital.length() > 15)
	{
		cout << "输入数据过大！（整数部分最多13位！）" << endl;
		return;
	}

	if (!strdigital.empty())
	{
		int smallpoint = strdigital.find_last_of(".");
		string strint = "";
		string strsmall = "";

		if (smallpoint != -1)
		{
			strint = strdigital.substr(0, smallpoint);
			strsmall = strdigital.substr(smallpoint + 1);
		}

		bool bhaveint = false;
		if (strint.length() > 0)
		{
			for (int i = 0; i < strint.length(); i++)
			{
				if ('0' != strint.at(i))
				{
					bhaveint = true;
					break;
				}
			}
		}		

		bool bhavesamll = false;
		if (strsmall.length() > 0)
		{
			for (int i = 0; i < strsmall.length(); i++)
			{
				if ('0' != strsmall.at(i))
				{
					bhavesamll = true;
					break;
				}
			}
		}		

		string strintupper = "";
		if (bhaveint)
		{
			strintupper = string_to_chinese_implement(INT_ONLY, strint);
		}

		string strsmallupper = "";
		if (bhavesamll)
		{
			strsmallupper = string_to_chinese_implement(SMALL_ONLY, strsmall);
		}

		strresult1 = strintupper + strsmallupper;
		strresult1 = clean_zero(strresult1);		
		if (!strresult1.empty())
		{
			strresult2 = strresult1 + "整";
		}		
	}
}

//int main()
//{
//	string str1;
//	string str2;
//	double digital = 0;
//	string_to_chinese(digital, str1, str2);
//	cout << "uppercase:" << str1 << endl;
//	cout << "uppercase:" << str2 << endl;
//	return 0;
//}



string ws2s(const wstring& ws)
{
	string curlocale = setlocale(LC_ALL, NULL); // curLocale = "C";  

	setlocale(LC_ALL, "chs");

	const wchar_t* _source = ws.c_str();
	size_t _dsize = 2 * ws.size() + 1;
	char *_dest = new char[_dsize];
	memset(_dest, 0, _dsize);
	wcstombs(_dest, _source, _dsize);
	string result = _dest;
	delete[]_dest;

	setlocale(LC_ALL, curlocale.c_str());

	return result;
}

wstring s2ws(const string& s)
{
	setlocale(LC_ALL, "chs");

	const char* _source = s.c_str();
	size_t _dsize = s.size() + 1;
	wchar_t *_dest = new wchar_t[_dsize];
	wmemset(_dest, 0, _dsize);
	mbstowcs(_dest, _source, _dsize);
	wstring result = _dest;
	delete[]_dest;

	setlocale(LC_ALL, "C");

	return result;
}

double analyze_small(wstring strsmall, map<wstring, int>& mapupperlow)
{
	double small = 0.0;
	int jiaopos = strsmall.find_last_of(L"角");
	if (jiaopos != -1)
	{
		if (jiaopos - 1 >= 0)
		{
			wstring character = strsmall.substr(jiaopos - 1, 1);
			small = small + mapupperlow[character] * 0.1;
		}
	}

	int fenpos = strsmall.find_last_of(L"分");
	if (fenpos != -1)
	{
		if (fenpos - 1 >= 0)
		{
			wstring character = strsmall.substr(fenpos - 1, 1);
			small = small + mapupperlow[character] * 0.01;
		}
	}

	return small;
}

int analyze_yuan(wstring stryuan, map<wstring, int>& mapupperlow)
{
	int yuan = 0;

	int qianpos = stryuan.find_first_of(L"仟");
	if (qianpos != -1)
	{
		if (qianpos - 1 >= 0)
		{
			wstring character = stryuan.substr(qianpos - 1, 1);
			yuan = yuan + mapupperlow[character] * 1000;
		}
	}

	int baipos = stryuan.find_first_of(L"佰");
	if (baipos != -1)
	{
		if (baipos - 1 >= 0)
		{
			wstring character = stryuan.substr(baipos - 1, 1);
			yuan = yuan + mapupperlow[character] * 100;
		}
	}

	int shipos = stryuan.find_first_of(L"拾");
	if (shipos != -1)
	{
		if (shipos - 1 >= 0)
		{
			wstring character = stryuan.substr(shipos - 1, 1);
			yuan = yuan + mapupperlow[character] * 10;
		}
	}

	wstring character = stryuan.substr(stryuan.length() - 1, 1);
	yuan = yuan + mapupperlow[character];

	return yuan;
}

int analyze_wan(wstring strwan, map<wstring, int>& mapupperlow)
{
	int wan = 0;
	wan = analyze_yuan(strwan, mapupperlow);
	wan = 10000 * wan;

	return wan;
}

//处理亿后面的字段
double process_yi_after(wstring stryiafter, map<wstring, int>& mapupperlow)
{
	//找出圆或元的位置
	int yuanpos = stryiafter.find_last_of(L"元");
	if (yuanpos == -1)
	{
		yuanpos = stryiafter.find_last_of(L"圆");
	}

	//小数部分，角分字段
	wstring strsmall = L"";
	//元字段
	wstring stryuan = L"";
	//万字段
	wstring strwan = L"";

	double small = 0.0;
	if (yuanpos != -1)
	{
		//小数部分，角分字段
		strsmall = stryiafter.substr(yuanpos + 1);
		if (!strsmall.empty())
		{
			//解析
			small = analyze_small(strsmall, mapupperlow);
		}
	}


	//找出万
	int wanpos = stryiafter.find_last_of(L"万");

	//元字段
	if (wanpos == -1)
	{
		stryuan = stryiafter.substr(0, yuanpos);
	}
	else
	{
		stryuan = stryiafter.substr(wanpos + 1, yuanpos - wanpos - 1);
	}
	int yuan = 0;
	if (!stryuan.empty())
	{
		//解析
		yuan = analyze_yuan(stryuan, mapupperlow);
	}

	if (wanpos != -1)
	{
		strwan = stryiafter.substr(0, wanpos);
	}
	int wan = 0;
	if (!strwan.empty())
	{
		wan = analyze_wan(strwan, mapupperlow);
	}

	return wan + yuan + small;
}

//处理亿前面的字段
int process_yi_before(wstring stryiafter, map<wstring, int>& mapupperlow)
{
	int yi = 0;
	yi = process_yi_after(stryiafter, mapupperlow);
	yi = yi * 100000000;
	return yi;
}

double chinese_upperdigital_to_digital(const std::string & chinese)
{
	if (chinese.empty() || chinese.length() > 30)
	{
		return -1.0;
	}

	//string 2 wstring
	wstring strchinese = s2ws(chinese);

	//找出圆或元的位置
	int yuanpos = strchinese.find_last_of(L"元");
	if (yuanpos == -1)
	{
		yuanpos = strchinese.find_last_of(L"圆");
	}

	if (yuanpos == -1)
	{
		return -1.0;
	}

	map<wstring, int> mapupperlow;
	mapupperlow.insert(std::pair<wstring, int>(L"零", 0));
	mapupperlow.insert(std::pair<wstring, int>(L"壹", 1));
	mapupperlow.insert(std::pair<wstring, int>(L"贰", 2));
	mapupperlow.insert(std::pair<wstring, int>(L"叁", 3));
	mapupperlow.insert(std::pair<wstring, int>(L"肆", 4));
	mapupperlow.insert(std::pair<wstring, int>(L"伍", 5));
	mapupperlow.insert(std::pair<wstring, int>(L"陆", 6));
	mapupperlow.insert(std::pair<wstring, int>(L"柒", 7));
	mapupperlow.insert(std::pair<wstring, int>(L"捌", 8));
	mapupperlow.insert(std::pair<wstring, int>(L"玖", 9));

	int yibefore = 0;
	double yiafter = 0;

	//找出亿
	int yipos = strchinese.find_last_of(L"亿");

	//亿后面的字段
	wstring stryiafter = L"";

	//亿前面的字段
	wstring stryibefore = L"";

	//亿所在的位置
	if (yipos == -1)
	{
		stryiafter = strchinese;
	}
	else
	{
		stryibefore = strchinese.substr(0, yipos);
		stryiafter = strchinese.substr(yipos + 1, strchinese.length() - yipos);
	}

	//亿前面的字段
	if (stryibefore.length() > 0)
	{
		yibefore = process_yi_before(stryibefore, mapupperlow);
	}

	//亿后面的字段
	if (stryiafter.length() > 0)
	{
		yiafter = process_yi_after(stryiafter, mapupperlow);
	}

	return yibefore + yiafter;
}

//int main()
//{
//	string strchinese = "贰亿壹万零叁佰肆拾伍圆零玖分";
//	double digital = chinese_upperdigital_to_digital(strchinese);
//	cout << "digital:" << std::fixed << digital << endl;
//	return 0;
//}