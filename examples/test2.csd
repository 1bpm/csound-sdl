<CsoundSynthesizer>
<CsOptions>
-odac
-m0
</CsOptions>
<CsInstruments>
sr = 44100
kr = 441
nchnls = 2
0dbfs = 1
seed 0
/*
    SDL Example 2
    Mouse triggered clusters of rectangles
*/

gisdl init -1
gicol init -1

instr 1
	gisdl, ktrig, kx, ky sdlinit "A SDL Window", 1000, 1000, 30
	schedkwhen ktrig, 0.05, 0, 2, 0, 1, kx, ky
        kc = abs(oscil(40, 0.2))
        ibg sdlcolour kc, kc, kc, 0
        sdlrect gisdl, ibg, 0, 0, 10, 10
endin


instr 2
    ix init p4
    iy init p5
    inum random 5, 20
    index = 0
    while (index < inum) do
        event_i "i", 3, random(0, 0.5), random(0.5, 1.5), random(ix-0.05, ix+0.05), random(iy-0.05, iy+0.05)
        index += 1
    od
endin

instr 3
	kc1 line 0, p3, random(100, 255)
	kc2 line 0, p3, random(100, 255)
	icol sdlcolour kc1, kc2, kc2, 255
	kx init p4
	ky init p5
        isize random 0.01, 0.05
	sdlrect gisdl, icol, kx, ky, isize, isize
	
	iamp = 0.3 + (p5*0.7)
	kamp line iamp, p3, 0
	ipitch = 100 + (p4*1000)
	a1 oscil kamp, ipitch
	outs a1*0.01, a1*0.01
endin


</CsInstruments>
<CsScore>
i1 0 6000
</CsScore>
</CsoundSynthesizer>
