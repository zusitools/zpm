/*************************
Copyright 2007 Jinbo Huang

This file is part of Tinisat.

Tinisat is free software; you can redistribute it 
and/or modify it under the terms of the GNU General 
Public License as published by the Free Software 
Foundation; either version 2 of the License, or
(at your option) any later version.

Tinisat is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE. See the GNU General Public 
License for more details.

You should have received a copy of the GNU General
Public License along with Tinisat; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, 
Fifth Floor, Boston, MA  02110-1301  USA
*************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Cnf.h"

Cnf::Cnf(unsigned vc, unsigned cc, int **clauses, unsigned lc, unsigned *cl)
{
    this->vc = vc;
    this->cc = cc;
    this->clauses = clauses;
    this->lc = lc;
    this->cl = cl;
}

Cnf::~Cnf(){
	if(clauses){ 
		for(unsigned i = 0; i < cc; i++) free(clauses[i]);
		free(clauses);
	}
    free(cl);
}

int Cnf::addClauseWithExistingVars(int *clause, unsigned length)
{
    this->cc++;
    this->lc += length;
    this->cl = (unsigned*) realloc(cl, cc * sizeof(unsigned));
    this->cl[cc - 1] = length;
    this->clauses = clauses = (int **) realloc(clauses, cc * sizeof(int *));
    this->clauses[cc - 1] = clause;

    return cc - 1;
}
