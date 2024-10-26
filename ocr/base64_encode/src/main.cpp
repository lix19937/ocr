
#include "lc_string_util.h"
#include <iostream>

int main(int argc, char *argv[]) {    
  if (argc < 2) {
    std::cout << "bad input cmds, argc:"  << argc << std::endl;
    return 1;
  }

  std::string input;
  int pattern = 0;
  if (argc == 2) {      // bin  cfg
    input.assign(argv[1]);
  }
  else if (argc == 3) {// bin  -d  b64.cfg 
    std::string pstr(argv[1]);
    if (pstr == "-d") {
      pattern = 1;
      input.assign(argv[2]);
    }
    else {
      std::cout << "bad input cmds, argc:" << argc <<  std::endl;
      std::cout << argv[0] << " " << argv[1] << " " << argv[2] << std::endl;
      return 1;
    }
  }

  if (pattern == 0) {
    auto cfg_b64 = input + ".pro";
    ///  cfg -> cfg.pro
    lc_string_util::ansiitext2base64(input, cfg_b64);

    /// cfg.pro -> cfg.tmp
    auto confpb = input + ".tmp";
    lc_string_util::base642ansciitext(cfg_b64, confpb);
  }
  else {
    auto cfg_b64 = input;

    /// cfg.pro -> cfg.tmp
    auto confpb = input + ".tmp";
    lc_string_util::base642ansciitext(cfg_b64, confpb);
  }

  return 0;
}
  
 

