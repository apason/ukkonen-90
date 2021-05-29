"""
Copyright (c) <2021> <Andreas Niskanen>
Modified by Arttu Kilpinen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import math, sys, itertools
from pysat.solvers import Cadical
from pysat.card import *

filename = sys.argv[1]
strings = open(filename).read().split("\n")
strings = list(filter(None, strings))
alphabet = sorted(list(set([c for s in strings for c in s])))

out_len = max([len(s) for s in strings])-1
while True:
	out_len += 1
	var_counter = itertools.count(1)
	output_char = { (j,c) : next(var_counter) for j in range(out_len) for c in alphabet }
	string_at_pos = { (i,j) : next(var_counter) for i,s in enumerate(strings) for j in range(out_len-len(s)+1) }

	solver = Cadical()

	#print("Encoding length =", out_len)
	for i, s in enumerate(strings):
		string_occurs = [ string_at_pos[(i,j)] for j in range(out_len-len(s)+1) ]
		solver.add_clause(string_occurs)
		for j in range(out_len-len(s)+1):
			for k, c in enumerate(s):
				solver.add_clause([ -string_at_pos[(i,j)], output_char[(j+k,c)] ])

	for j in range(out_len):
		cnf = CardEnc.equals(lits=[ output_char[(j,c)] for c in alphabet ], bound=1, top_id=solver.nof_vars())
		solver.append_formula(cnf.clauses)

	#print("Variables:", solver.nof_vars())
	#print("Clauses:", solver.nof_clauses())
	#print("Solving...")

	if solver.solve():
		print(out_len)
		model = solver.get_model()
		out_string = ""
		for j in range(out_len):
			for c in alphabet:
				if model[output_char[(j,c)]-1] > 0:
					out_string += c
		if len(sys.argv) < 3 or (len(sys.argv) == 3 and sys.argv[2] != "-n"):
			print(out_string)
		break
