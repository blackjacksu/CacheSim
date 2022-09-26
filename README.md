# CacheSim
This is a cache simulator authored by Willy the man (willytwsu@tamu.edu).


# Compilation: 
g++ -g3 -std=c++11 -o cache cache.cpp
# Execution: 
gzip -dc 429.mcf-184B.trace.txt.gz | ./cache 2048 64 64 l 
55752 5.575200% 55703 5.610155% 49 0.689752%