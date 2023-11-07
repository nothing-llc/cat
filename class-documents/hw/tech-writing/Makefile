modulation.pdf: modulation.tex references.bib $(wildcard images/*)
	latexmk -auxdir=aux/ -outdir=out/ $<
	cp out/modulation.pdf .

.PHONY: clean
clean:
	rm -rf modulation.pdf aux/ out/
