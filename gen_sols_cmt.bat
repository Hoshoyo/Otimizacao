@echo off

call "bin/release/Otimizacao.exe" < CMT/CMT1.vrp 10 100000 -n 10 > "CMT/gen_solutions/CMT1.txt"
call "bin/release/Otimizacao.exe" < CMT/CMT2.vrp 10 100000 -n 10 > "CMT/gen_solutions/CMT2.txt"
call "bin/release/Otimizacao.exe" < CMT/CMT3.vrp 10 100000 -n 10 > "CMT/gen_solutions/CMT3.txt"
call "bin/release/Otimizacao.exe" < CMT/CMT4.vrp 10 100000 -n 10 > "CMT/gen_solutions/CMT4.txt"
call "bin/release/Otimizacao.exe" < CMT/CMT5.vrp 10 100000 -n 10 > "CMT/gen_solutions/CMT5.txt"