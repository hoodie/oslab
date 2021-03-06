#!/bin/bash
txtred='\e[0;31m'       # red
txtgrn='\e[0;32m'       # green
txtylw='\e[0;33m'       # yellow
txtblu='\e[0;34m'       # blue
txtpur='\e[0;35m'       # purple
txtcyn='\e[0;36m'       # cyan
txtwht='\e[0;37m'       # white
bldred='\e[1;31m'       # red    - Bold
bldgrn='\e[1;32m'       # green
bldylw='\e[1;33m'       # yellow
bldblu='\e[1;34m'       # blue
bldpur='\e[1;35m'       # purple
bldcyn='\e[1;36m'       # cyan
bldwht='\e[1;37m'       # white
txtund=$(tput sgr 0 1)  # Underline
txtbld=$(tput bold)     # Bold
txtrst='\e[0m'          # Text reset
function warn(){ echo -e ${bldred}! $*${txtrst} ; }
function info(){ echo -e ${bldblu}* $*${txtrst} ; }
function pass(){ echo -e ${bldwht}* $*${txtrst} ; }

cd src/l4/pkg/libc_backends/lib/l4re_mem/
echo '--------------------------------------------------------'
info $PWD
echo -e $bldpur
./make.sh
echo -e $txtrst'--------------------------------------------------------\n\n'
cd -

cd src/l4/pkg/video_server/
echo '--------------------------------------------------------'
info "Video Server($PWD)"
echo -e $bldblu
./make.sh
echo -e $txtrst'--------------------------------------------------------\n\n'
cd -

cd src/l4/pkg/hello_server/
echo '--------------------------------------------------------'
info $PWD
echo -e $bldcyn
./make.sh
echo -e $txtrst'--------------------------------------------------------\n\n'
cd -

cd src/l4/pkg/malloc_test
echo '--------------------------------------------------------'
info $PWD
echo -e $bldylw
./make.sh
echo -e $txtrst'--------------------------------------------------------\n\n'
cd -

cd obj/l4/x86/
echo '--------------------------------------------------------'
info $PWD
read -p "want to run? [Y|n] : " confirmation

  case "$confirmation" in
    Yes|yes|Y|y|"")
      make qemu E=video_hendrik
      ;;
    No|no|N|n)
      warn canceled
      ;;
    *)
      warn excuse me? ;;
  esac

echo -e '--------------------------------------------------------\n\n'
cd -
