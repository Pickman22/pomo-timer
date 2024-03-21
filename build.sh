rm -rf build 2> /dev/null
mkdir build 2> /dev/null
gcc -std=c2x -Wall -Wextra -Wno-gnu -pedantic -g0 -ggdb -I/opt/homebrew/Cellar/raylib/5.0/include main.c -c
gcc -std=c2x -Wall -Wextra -Wno-gnu -pedantic -g0 -ggdb -I/opt/homebrew/Cellar/raylib/5.0/include ui_keys.c -c
gcc -std=c2x -Wall -Wextra -Wno-gnu -pedantic -g0 -ggdb -I/opt/homebrew/Cellar/raylib/5.0/include ui_pomo.c -c
gcc -std=c2x -Wall -Wextra -Wno-gnu -pedantic -g0 -ggdb pomo.c -c
mv *.o build
gcc -std=c2x -Wall -Wextra -pedantic -g0 -ggdb  ./build/ui_keys.o ./build/ui_pomo.o ./build/pomo.o ./build/main.o -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -o pomo
./pomo
