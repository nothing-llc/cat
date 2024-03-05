months=october november december february march
entries=$(wildcard $(patsubst %,entries/%/*.tex,$(months)))
figures=$(patsubst %,{figures/%/},$(months))

# work out the macros to pass to latex so it knows what to do
space=$(eval) $(eval)
comma=,
logentries=$(subst $(space),$(comma),$(entries))
figuredirs=$(subst $(space),$(comma),$(figures))
pretex=\def\logentries{$(logentries)}\def\figuredirs{{$(figuredirs)}}

export pretexcode

out/log.pdf: log.tex references.bib $(entries) $(wildcard figures/**)
	latexmk -g \
		-auxdir=aux/ -outdir=out/ \
		-usepretex='$(pretex)' \
		$<

.PHONY: clean
clean:
	rm -rf aux/ out/
