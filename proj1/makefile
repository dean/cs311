default: o

all: pdf o

dvi: recreate.tex 
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
#	pygmentize -f latex -o __recreate.tex recreate.tex
#	run latex twice to get references correct
	latex recreate.tex
	latex assignment1.tex
#	you can also have a bibtex line here
#	bibtex recreate
	latex recreate.tex
	latex assignment1.tex
#	remove the pygmentized output to avoid cluttering up the directory
#	rm __recreate.tex

ps: dvi
	dvips -R -Poutline -t letter recreate.dvi -o recreate.ps
	dvips -R -Poutline -t letter assignment1.dvi -o assignment1.ps

pdf: ps
	ps2pdf recreate.ps
	ps2pdf assignment1.ps

o: seve.c
	icc seve.c  

