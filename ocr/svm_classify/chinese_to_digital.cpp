
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
	int jiaopos = strsmall.find_last_of(L"��");
	if (jiaopos != -1)
	{
		if (jiaopos - 1 >= 0)
		{
			wstring character = strsmall.substr(jiaopos - 1, 1);
			small = small + mapupperlow[character] * 0.1;
		}		
	}

	int fenpos = strsmall.find_last_of(L"��");
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

	int qianpos = stryuan.find_first_of(L"Ǫ");
	if (qianpos != -1)
	{
		if (qianpos - 1 >= 0)
		{
			wstring character = stryuan.substr(qianpos - 1, 1);
			yuan = yuan + mapupperlow[character] * 1000;
		}
	}

	int baipos = stryuan.find_first_of(L"��");
	if (baipos != -1)
	{
		if (baipos - 1 >= 0)
		{
			wstring character = stryuan.substr(baipos - 1, 1);
			yuan = yuan + mapupperlow[character] * 100;
		}
	}

	int shipos = stryuan.find_first_of(L"ʰ");
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

//�����ں�����ֶ�
double process_yi_after(wstring stryiafter, map<wstring, int>& mapupperlow)
{
	//�ҳ�Բ��Ԫ��λ��
	int yuanpos = stryiafter.find_last_of(L"Ԫ");
	if (yuanpos == -1)
	{
		yuanpos = stryiafter.find_last_of(L"Բ");
	}

	//С�����֣��Ƿ��ֶ�
	wstring strsmall = L"";
	//Ԫ�ֶ�
	wstring stryuan = L"";
	//���ֶ�
	wstring strwan = L"";

	double small = 0.0;
	if (yuanpos != -1)
	{
		//С�����֣��Ƿ��ֶ�
		strsmall = stryiafter.substr(yuanpos + 1);
		if (!strsmall.empty())
		{
			//����
			small = analyze_small(strsmall, mapupperlow);
		}
	}


	//�ҳ���
	int wanpos = stryiafter.find_last_of(L"��");

	//Ԫ�ֶ�
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
		//����
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

//������ǰ����ֶ�
int process_yi_before(wstring stryiafter, map<wstring, int>& mapupperlow)
{
	int yi = 0;
	yi = process_yi_after(stryiafter, mapupperlow);
	yi = yi * 100000000;
	return yi;
}

double chinese_upperdigital_to_digital(string chinese)
{
	if (chinese.empty() || chinese.length() > 30)
	{
		return -1.0;
	}

	//string 2 wstring
	wstring strchinese = s2ws(chinese);

	//�ҳ�Բ��Ԫ��λ��
	int yuanpos = strchinese.find_last_of(L"Ԫ");
	if (yuanpos == -1)
	{
		yuanpos = strchinese.find_last_of(L"Բ");
	}

	if (yuanpos == -1)
	{
		return -1.0;
	}

	map<wstring, int> mapupperlow;
	mapupperlow.insert(std::pair<wstring, int>(L"��", 0));
	mapupperlow.insert(std::pair<wstring, int>(L"Ҽ", 1));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 2));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 3));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 4));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 5));
	mapupperlow.insert(std::pair<wstring, int>(L"½", 6));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 7));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 8));
	mapupperlow.insert(std::pair<wstring, int>(L"��", 9));

	int yibefore = 0;
	double yiafter = 0;

	//�ҳ���
	int yipos = strchinese.find_last_of(L"��");

	//�ں�����ֶ�
	wstring stryiafter = L"";

	//��ǰ����ֶ�
	wstring stryibefore = L"";

	//�����ڵ�λ��
	if (yipos == -1)
	{
		stryiafter = strchinese;
	}
	else
	{
		stryibefore = strchinese.substr(0, yipos);
		stryiafter = strchinese.substr(yipos + 1, strchinese.length() - yipos);
	}

	//��ǰ����ֶ�
	if (stryibefore.length() > 0)
	{
		yibefore = process_yi_before(stryibefore, mapupperlow);
	}

	//�ں�����ֶ�
	if (stryiafter.length() > 0)
	{
		yiafter = process_yi_after(stryiafter, mapupperlow);
	}

	return yibefore + yiafter;
}

int main()
{
	string strchinese = "����Ҽ����������ʰ��Բ�����";
	double digital = chinese_upperdigital_to_digital(strchinese);
	cout << "digital:" << std::fixed << digital << endl;
	return 0;
}