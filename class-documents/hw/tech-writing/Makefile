modulation.pdf: modulation.tex references.bib
	latexmk -auxdir=aux/ -outdir=out/ $<
	cp out/modulation.pdf .

.PHONY: clean
clean:
	rm -rf modulation.pdf aux/ out/
