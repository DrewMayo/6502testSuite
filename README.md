This is a simulation of a 6502 like processor (Ricoh 2A03)
The main difference with this cpu and a regular 6502 is the absence
of decimal mode. This cpu is instruction accurate meaning the state
before the cpu instruction is the same and after the cpu instruction is
exactly the same as in reality, however during the instruction the cycles
differ quite dramatically (In software I did it all at once). This cpu
tests all 256 opcodes and as long as the affects were deterministic
in the actual cpu in the enviroment where it was deployed (the Nintendo 
Entertainment System). Each opcode runs 10000 test cases designed by tom
harte running a total of 2.55 million tests and only failing on the ones
that are region specific (instructions like SBX)

Also note that that testing leaks memorly like a sieve but
the cpu itself does not (as far as I can tell).
