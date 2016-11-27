@echo off

call "bin/release/Otimizacao.exe" < X/X-n101-k25.vrp 10 100000 -n 10 > "X/gen_solutions/X-n101-k25.txt"
call "bin/release/Otimizacao.exe" < X/X-n106-k14.vrp 10 100000 -n 10 > "X/gen_solutions/X-n106-k14.txt"
call "bin/release/Otimizacao.exe" < X/X-n110-k13.vrp 10 100000 -n 10 > "X/gen_solutions/X-n110-k13.txt"
call "bin/release/Otimizacao.exe" < X/X-n115-k10.vrp 10 100000 -n 10 > "X/gen_solutions/X-n115-k10.txt"
call "bin/release/Otimizacao.exe" < X/X-n120-k6.vrp 10 100000 -n 10 > "X/gen_solutions/X-n120-k6.txt"