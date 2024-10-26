
#ifndef  BKTREE_API_H
#define BKTREE_API_H
/*
https://blog.csdn.net/key_xiaodj/article/details/78403996
#/home/lix/poc/boost_1_64_0/tools/build/src/tools  -fPIC      gcc.jam +400   rule setup-fpic ()

tar -zxvf boost_1_64_0.tar.gz
cd boost_1_64_0
#generate cfg file
./bootstrap.sh --with-libraries=all --with-toolset=gcc   or  only part serialization compile
./bootstrap.sh --with-libraries=serialization --with-toolset=gcc   //use this

#compile build
./b2 install --prefix=/usr/local


- atomic                   : not building
- chrono                   : not building
- container                : not building
- context                  : not building
- coroutine                : not building
- coroutine2               : not building
- date_time                : not building
- exception                : not building
- fiber                    : not building
- filesystem               : not building
- graph                    : not building
- graph_parallel           : not building
- iostreams                : not building
- locale                   : not building
- log                      : not building
- math                     : not building
- metaparse                : not building
- mpi                      : not building
- program_options          : not building
- python                   : not building
- random                   : not building
- regex                    : not building
- serialization            : building
- signals                  : not building
- system                   : not building
- test                     : not building
- thread                   : not building
- timer                    : not building
- type_erasure             : not building
- wave                     : not building

*/
#include <string>
#include <set>
#include <iostream>
#include <fstream>

class BKT {
public:
  struct bkresult_info {
    std::string val;
    int dist;
    bool operator <(const bkresult_info &oth)const {
      return dist < oth.dist || (dist == oth.dist  && val.size() > oth.val.size());
    }
  };

  //offline
  int create_bktree(const std::string &src, const std::string &dst);

  //job
  int bktree_search(const std::string &bk_dict, const std::string &word, std::set<bkresult_info> &result_set, const int &max_dist=6);

  int bkt_test(){
#if (defined __GNUC__ && __GNUC__ >= 4)
     create_bktree("./hosp_lib.txt", "./hosp_lib.dict");  //linux下  ./hosp_lib.txt必须是unix -LF 行尾, //windows下 LF or CRLF均可
     std::ofstream outtxt("./bktlog");
#elif (defined _WIN32 || defined WINCE || defined __CYGWIN__) 
   // create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/h_name", "E:/lc_lite_v3/x64/Release/model/bkt/h_name.dict2");//windows下 LF or CRLF均可
	  //create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/h_name", "E:/lc_lite_v3/x64/Release/model/bkt/h_name.dict");
	  //create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/h_type", "E:/lc_lite_v3/x64/Release/model/bkt/h_type.dict");
	  //create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/p_name", "E:/lc_lite_v3/x64/Release/model/bkt/p_name.dict");
	  //create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/pd_name", "E:/lc_lite_v3/x64/Release/model/bkt/pd_name.dict");
	  //create_bktree("E:/lc_lite_v3/x64/Release/model/bkt/u_type", "E:/lc_lite_v3/x64/Release/model/bkt/u_type.dict");
	  std::ofstream outtxt("E:/lc_lite_v3/x64/Release/model/bktlog_load_each");
#endif
    
	/// if use utf-8 no bom ;  c/c++  command line  /utf-8 
    ///
    std::set<bkresult_info> result_set;
    std::string hosp[8] = {"上海交通大学医学院附新医院"
		"海洋县人民医院", 
		"宜兴市人民","上海市第六人民医院", "上海市东方医院",
		"上海交通大学医学院附新华医院", "中福会妇幼国院","上海市第大人民医院"
	};
    for (int i = 0; i < 8; ++i) {
      result_set.clear();
#if (defined __GNUC__ && __GNUC__ >= 4)
      bktree_search("./hosp_lib-2.dict", hosp[i], result_set);
#elif (defined _WIN32 || defined WINCE || defined __CYGWIN__)

    bktree_search("E:/lc_lite_v3/x64/Release/model/bkt/h_name.dict", hosp[i], result_set);
	  bktree_search("E:/lc_lite_v3/x64/Release/model/bkt/h_type.dict", hosp[i], result_set);
	  bktree_search("E:/lc_lite_v3/x64/Release/model/bkt/p_name.dict", hosp[i], result_set);
	  bktree_search("E:/lc_lite_v3/x64/Release/model/bkt/pd_name.dict", hosp[i], result_set);
	  bktree_search("E:/lc_lite_v3/x64/Release/model/bkt/u_type.dict", hosp[i], result_set);
#endif

      for (auto &it : result_set) {
        std::cout << it.val << "\t" << it.dist << std::endl;
        outtxt << it.val << "\t" << it.dist << std::endl;
      }
      std::cout << "\n";
      outtxt << "\n";
    }

    outtxt.close();
    return 0;
  }
  
};

#endif


