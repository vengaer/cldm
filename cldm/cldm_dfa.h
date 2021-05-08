#ifndef CLDM_DFA_H
#define CLDM_DFA_H

#include <stdbool.h>
#include <stdlib.h>

struct cldm_dfa {
    struct cldm_dfa_state *states;
    struct cldm_dfa_edge  *edges;
    unsigned nstates;
    unsigned state_idx;
    unsigned nedges;
    unsigned edge_idx;
    unsigned start;
};

bool cldm_dfa_add_argument(struct cldm_dfa *restrict dfa, char const *restrict arg);
char const *cldm_dfa_simulate(struct cldm_dfa const *restrict dfa, char const *restrict input);

bool cldm_dfa_init(struct cldm_dfa *dfa);
inline void cldm_dfa_free(struct cldm_dfa *dfa) {
    free(dfa->states);
    free(dfa->edges);
}

#endif /* CLDM_DFA_H */
