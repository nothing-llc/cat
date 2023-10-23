sign.pdf: sign.tex
	latexmk -auxdir=aux/ -outdir=out/ $<
	cp out/sign.pdf .

.PHONY: clean
clean:
	rm -rf sign.pdf aux/ out/
