#ifndef CLDM_DFA_H
#define CLDM_DFA_H

#include <stdbool.h>
#include <stdlib.h>

struct cldm_nfa {
    struct cldm_nfa_state *states;
    struct cldm_nfa_edge *edges;
    unsigned nstates;
    unsigned state_idx;
    unsigned nedges;
    unsigned edge_idx;
    unsigned start;
};

bool cldm_nfa_add_argument(struct cldm_nfa *restrict nfa, char const *restrict arg);
int cldm_nfa_simulate(struct cldm_nfa const *restrict nfa, char const *restrict input);

bool cldm_nfa_init(struct cldm_nfa *nfa);
inline void cldm_nfa_free(struct cldm_nfa *nfa) {
    free(nfa->states);
    free(nfa->edges);
}

#endif /* CLDM_DFA_H */
