/*
    load_and_search.cpp
    By Stephen Holiday 2011
    http://stephenholiday.com
    (Exception, Distance Algorithm by Anders Sewerin Johansen)

    The code is under the [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) license.

    This is a utility for loading a BKTree from a serialized file.
    
    ./bkSearch [serialized tree] [word] [max edxit distance]
    ./bkSearch bkTree.dat word 3

    This is a utility for creating a BKTree from a text file and saving to disk.
    The input files should have one entry per line.

    ./bkLoad [from] [to]
    ./bkLoad /usr/share/dict/words wordTree.dat

    //////////////////////////////////////////////////////////////
    CC =g++
    CFLAGS=-g -Wall -std=c++11 -O2

    INC = -I/usr/local/include/boost
    LIB=-L/usr/local/lib/
    BOOSTLIBS=/usr/local/lib/libboost_serialization.a
    #

    tools:
      $(CC) $(CFLAGS) $(INC) BKTree_api.cpp -o bkt $(LIB) $(BOOSTLIBS)

*/

#include "lc_bktree.h"
#include "lc_bktree_api.h"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//txt -> dict   offline interface
int BKT::create_bktree(const std::string &src, const std::string &dst) {
  BKTree<std::string> tree = BKTree<std::string>();
  std::ifstream words(src);
  std::ofstream ofs(dst);
  if (!words.is_open()) {
    std::cout << "error, The file could not be opened, file: " << src << std::endl;
    return 1;
  }

  std::string str;
  while (std::getline(words, str)) {
    tree.insert(str);
  }
  std::cout << "Loaded " << tree.size() << " entries" << std::endl;
  boost::archive::text_oarchive oa(ofs);
  oa << tree;
  return 0;
}

int BKT::bktree_search(const std::string &bk_dict, const std::string &word,std::set<bkresult_info> &result_set, const int &max_dist) {
  if (word.empty()) {
    return 1;
  }
  BKTree<std::string> in_tree;
  std::ifstream iff(bk_dict);
  boost::archive::text_iarchive iar(iff);
  iar >> in_tree;
  if (0 == in_tree.size()) {
    std::cerr << "in_tree is empty " << bk_dict << std::endl;
    return 1;
  }

  std::vector<std::string> results;
  results = in_tree.find(word, max_dist);
  if (results.empty()) {
    //std::cout << "results not find" << std::endl;
    return 1;
  }

  for (const auto &it : results) {
    auto pos = it.find("=");
    if (pos == std::string::npos) {
      std::cout << "error, Should not happen ! " << std::endl;
      continue;
    }
    bkresult_info bki;
    bki.val = it.substr(0, pos);
    bki.dist = atoi(it.substr(pos + 1).c_str());
    result_set.insert(bki);
  }

  //std::vector<std::string>::iterator it;
  //for ( it=results.begin() ; it < results.end(); ++it )
  //    std::cout << *it << std::endl;
  return 0;
}

/*
# compiler to use
CC =g++ 
CFLAGS=-g -Wall -std=c++11 -O2

INC = -I/usr/local/include/boost
LIB=-L/usr/local/lib/
BOOSTLIBS=/usr/local/lib/libboost_serialization.a 
or BOOSTLIBS=-lboost_serialization
#libboost_wserialization.a  is not ok


#vim /etc/ld.so.conf.d/cv.conf   ldconfig
#note tab key space
tools:
  $(CC) $(CFLAGS) $(INC) lc_bktree_api.cpp -o bkt $(LIB) $(BOOSTLIBS)
*/
#if (defined __GNUC__ && __GNUC__ >= 4)
int main_t(int argc, char** argv) {
  BKT bkt;
  bkt.bkt_test();
  return 0;
}
#endif



