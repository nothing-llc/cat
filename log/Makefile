entries=$(wildcard entries/*/*.tex)

# work out the macro to pass to latex so it knows what to do
space=$(eval) $(eval)
comma=,
logentries=$(subst $(space),$(comma),$(entries))

log.pdf: log.tex references.bib $(entries)
	latexmk \
		-auxdir=aux/ -outdir=out/ \
		-usepretex='\def\logentries{$(logentries)}' \
		$<
	cp out/log.pdf .

.PHONY: clean
clean:
	rm -rf log.pdf aux/ out/
