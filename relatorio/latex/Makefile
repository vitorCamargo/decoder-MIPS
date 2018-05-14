all: relatorio

relatorio:
	pdflatex relatorio.tex
	bibtex relatorio
	pdflatex relatorio.tex
	pdflatex relatorio.tex
	rm -f *~ *.aux *.bbl *.blg *.log *.dvi *.backup *.kate-swp
	
	evince relatorio.pdf &

clean:
	rm *~ *.aux *.dvi *.bbl *.blg *.log *.ps *.backup *.kate-swp

