default: pdf

all: pdf

dvi: writeup.tex
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
#	pygmentize -f latex -o __recreate.tex recreate.tex
#	run latex twice to get references correct
	latex writeup.tex
#	you can also have a bibtex line here
#	bibtex recreate
	latex writeup.tex
#	remove the pygmentized output to avoid cluttering up the directory
#	rm __recreate.tex

ps: dvi
	dvips -R -Poutline -t letter writeup.dvi -o writeup.ps

pdf: ps
	ps2pdf writeup.ps
