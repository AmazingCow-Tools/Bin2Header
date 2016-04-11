##----------------------------------------------------------------------------##
##               █      █                                                     ##
##               ████████                                                     ##
##             ██        ██                                                   ##
##            ███  █  █  ███        Makefile                                  ##
##            █ █        █ █        Bin2Header                                ##
##             ████████████                                                   ##
##           █              █       Copyright (c) 2016                        ##
##          █     █    █     █      AmazingCow - www.AmazingCow.com           ##
##          █     █    █     █                                                ##
##           █              █       N2OMatt - n2omatt@amazingcow.com          ##
##             ████████████         www.amazingcow.com/n2omatt                ##
##                                                                            ##
##                  This software is licensed as GPLv3                        ##
##                 CHECK THE COPYING FILE TO MORE DETAILS                     ##
##                                                                            ##
##    Permission is granted to anyone to use this software for any purpose,   ##
##   including commercial applications, and to alter it and redistribute it   ##
##               freely, subject to the following restrictions:               ##
##                                                                            ##
##     0. You **CANNOT** change the type of the license.                      ##
##     1. The origin of this software must not be misrepresented;             ##
##        you must not claim that you wrote the original software.            ##
##     2. If you use this software in a product, an acknowledgment in the     ##
##        product IS HIGHLY APPRECIATED, both in source and binary forms.     ##
##        (See opensource.AmazingCow.com/acknowledgment.html for details).    ##
##        If you will not acknowledge, just send us a email. We'll be         ##
##        *VERY* happy to see our work being used by other people. :)         ##
##        The email is: acknowledgment_opensource@AmazingCow.com              ##
##     3. Altered source versions must be plainly marked as such,             ##
##        and must notbe misrepresented as being the original software.       ##
##     4. This notice may not be removed or altered from any source           ##
##        distribution.                                                       ##
##     5. Most important, you must have fun. ;)                               ##
##                                                                            ##
##      Visit opensource.amazingcow.com for more open-source projects.        ##
##                                                                            ##
##                                  Enjoy :)                                  ##
##----------------------------------------------------------------------------##


all: bin

clean:
	@ rm -rf ./bin

stdcow:
	@ cd ./lib/libstdcow && make clean && make obj

stdcow-debug:
	@cd ./lib/libstdcow && make clean && make NDEBUG=-DDEBUG obj


bin-debug: stdcow-debug
	@ mkdir -p ./bin

	@ gcc -g                           \
	      -DDEBUG                      \
	      -o ./bin/bin2header-debug    \
	      -I ./lib/libstdcow/include/  \
	         ./src/bin2header.c        \
	         ./lib/libstdcow/obj/*.o

bin: stdcow
	@ mkdir -p ./bin

	@ gcc -D NDEBUG                    \
	      -o ./bin/bin2header          \
	      -I ./lib/libstdcow/include/  \
	         ./src/bin2header.c        \
	         ./lib/libstdcow/obj/*.o


install: bin
	@ sudo mv ./bin/bin2header /usr/local/bin/bin2header

uninstall:
	@ sudo rm -f /usr/local/bin/bin2header

