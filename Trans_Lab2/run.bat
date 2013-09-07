@echo on
:lbl
cls
win_bison -dlvt language.y -o language.tab.cpp
win_flex -L -o lex.yy.cpp lexer.ll
win_bison.exe -dlvt -p tac -o language.tac.cpp language-3ac.y 
win_flex.exe -L -Ptac -o lex.tac.cpp lexer-3ac.l 
rem pause
rem goto lbl