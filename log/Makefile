entries=$(wildcard entries/october/*.tex entries/november/*.tex entries/december/*.tex)

# work out the macro to pass to latex so it knows what to do
space=$(eval) $(eval)
comma=,
logentries=$(subst $(space),$(comma),$(entries))

out/log.pdf: log.tex references.bib $(entries) $(wildcard figures/**)
	latexmk -g \
		-auxdir=aux/ -outdir=out/ \
		-usepretex='\def\logentries{$(logentries)}' \
		$<

.PHONY: clean
clean:
	rm -rf aux/ out/
