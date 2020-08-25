<CsoundSynthesizer>
<CsOptions>
-odac
--opcode-lib=/home/bargepole/bpcheckout/csound-dev/sdltest/sdllib.so
</CsOptions>
<CsInstruments>
sr = 44100
kr = 4410
nchnls = 2
0dbfs = 1
seed 0

gisdl init -1
gicol init -1

instr 1
	gisdl, ktrig, kx, ky sdlinit "A SDL Window", 1000, 1000, 60
	kvibe oscil 2, 0.1
	kvibe2 oscil 0.2, 0.5
	kcol oscil 250, 0.3
	gicol sdlcolour abs(kcol), 50, 50, 0

	schedkwhen ktrig, 0.05, 0, 2, 0, 1, kx, ky


	kl1 line 1, p3, 0
	sdlrect gisdl, gicol, kl1, 0.5, 0.2, abs(kvibe)*0.2
	ao oscil 1, 440*abs(kvibe)
	outs ao*0.01, ao*0.01
endin

instr 2
	krest line 0, p3, 255
	icol sdlcolour 255, krest, krest, 0
	kx init p4
	ky init p5
	sdlrect gisdl, icol, kx, ky, 0.1, 0.1
	
	iamp = 0.3 + (p5*0.7)
	kamp line iamp, p3, 0
	ipitch = 100 + (p4*400)
	a1 oscil kamp, ipitch
	outs a1*0.01, a1*0.01
endin

instr 3
	icol sdlcolour 0, 0, 0, 0
	kx line 0, p3, 1
	sdlline gisdl, icol, kx, 0, kx, 1
endin

</CsInstruments>
<CsScore>
i1 0 60
i3 0 10
</CsScore>
</CsoundSynthesizer>
