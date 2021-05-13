#include "cldm_nfa.h"
#include "cldm_log.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_NFA_STATE_DELTA = 64 };
enum { CLDM_NFA_EDGE_DELTA = 128 };
enum { CLDM_NFA_EDGE_NONE = -1 };

enum cldm_nfa_op {
    cldm_nfa_op_accept,
    cldm_nfa_op_valid
};

enum cldm_nfa_metachar {
    cldm_nfa_epsilon = CHAR_MAX + 1
};

struct cldm_nfa_state {
    enum cldm_nfa_op op;
    int edges;
};

struct cldm_nfa_edge {
    int ch;
    int next;
    int end;
};

void cldm_nfa_free(struct cldm_nfa *nfa);

static bool cldm_nfa_ensure_state_capacity(struct cldm_nfa *nfa, unsigned nrequested) {
    struct cldm_nfa_state *states;
    if(nfa->state_idx + nrequested < nfa->nstates) {
        return true;
    }
    states = realloc(nfa->states, (nfa->nstates + CLDM_NFA_STATE_DELTA) * sizeof(*states));
    if(!states) {
        cldm_err("Could not increase total number of states");
        return false;
    }
    nfa->states = states;
    nfa->nstates += CLDM_NFA_STATE_DELTA;
    return true;
}

static bool cldm_nfa_ensure_edge_capacity(struct cldm_nfa *nfa, unsigned nrequested) {
    struct cldm_nfa_edge *edges;
    if(nfa->edge_idx + nrequested < nfa->nedges) {
        return true;
    }
    edges = realloc(nfa->edges, (nfa->nedges + CLDM_NFA_EDGE_DELTA) * sizeof(*edges));
    if(!edges) {
        cldm_err("Could not increase total number of edges");
        return false;
    }
    nfa->edges = edges;
    nfa->nedges += CLDM_NFA_EDGE_DELTA;
    return true;
}

static inline int cldm_nfa_edge_pop(struct cldm_nfa *nfa) {
    if(!cldm_nfa_ensure_edge_capacity(nfa, 1)) {
        return -1;
    }

    return nfa->edge_idx++;
}

static inline int cldm_nfa_state_pop(struct cldm_nfa *nfa) {
    if(!cldm_nfa_ensure_state_capacity(nfa, 1)) {
        return -1;
    }

    return nfa->state_idx++;
}

static int cldm_nfa_append(struct cldm_nfa *nfa, int state, int ch, enum cldm_nfa_op op) {
    int edge;

    edge = cldm_nfa_edge_pop(nfa);
    if(edge == -1) {
        return -1;
    }
    nfa->edges[edge] = (struct cldm_nfa_edge) {
        .ch = ch,
        .next = nfa->states[state].edges,
        .end = cldm_nfa_state_pop(nfa)
    };

    if(nfa->edges[edge].end == -1) {
        return -1;
    }

    nfa->states[state].edges = edge;

    state = nfa->edges[edge].end;
    nfa->states[nfa->edges[edge].end] = (struct cldm_nfa_state) {
        .op = op,
        .edges = CLDM_NFA_EDGE_NONE
    };
    return state;
}

bool cldm_nfa_add_argument(struct cldm_nfa *restrict nfa, char const *restrict arg) {
    int state = nfa->start;
    int edge;
    for(; *arg; ++arg) {
        /* Check for existing edge */
        for(edge = nfa->states[state].edges; edge != CLDM_NFA_EDGE_NONE; edge = nfa->edges[edge].next) {
            if(nfa->edges[edge].ch == *arg) {
                state = nfa->edges[edge].end;
                break;
            }
        }

        /* No edge matched, add one */
        if(edge == CLDM_NFA_EDGE_NONE) {
            state = cldm_nfa_append(nfa, state, *arg, cldm_nfa_op_valid);
            if(state == -1) {
                return false;
            }
        }
    }

    return cldm_nfa_append(nfa, state, cldm_nfa_epsilon, cldm_nfa_op_accept) != -1;
}

int cldm_nfa_simulate(struct cldm_nfa const *restrict nfa, char const *restrict input) {
    struct cldm_nfa_state *state = &nfa->states[nfa->start];
    int edge;
    char const *pos = input;

    /* Simulate */
    for(; *pos; ++pos) {
        for(edge = state->edges; edge != CLDM_NFA_EDGE_NONE; edge = nfa->edges[edge].next) {
            if(nfa->edges[edge].ch == *pos) {
                state = &nfa->states[nfa->edges[edge].end];
                break;
            }
        }

        if(edge == CLDM_NFA_EDGE_NONE) {
            break;
        }
    }

    /* Check whether state is an accept state */
    if(state->op == cldm_nfa_op_accept) {
        return pos - input;
    }

    /* Check for accept states reachable via epsilon transition */
    for(edge = state->edges; edge != CLDM_NFA_EDGE_NONE; edge = nfa->edges[edge].next) {
        if(nfa->edges[edge].ch == cldm_nfa_epsilon && nfa->states[nfa->edges[edge].end].op == cldm_nfa_op_accept) {
            return pos - input;
        }
    }

    return -1;
}

bool cldm_nfa_init(struct cldm_nfa *nfa) {
    nfa->states = malloc(CLDM_NFA_STATE_DELTA * sizeof(*nfa->states));
    if(!nfa->states) {
        cldm_err("Could not allocate nfa nodes for argument parsing");
        return false;
    }

    nfa->edges = malloc(CLDM_NFA_EDGE_DELTA * (sizeof(*nfa->edges)));
    if(!nfa->edges) {
        cldm_err("Could not allocate nfa edges for arguments parsing");
        free(nfa->states);
        return false;
    }

    nfa->nstates = CLDM_NFA_STATE_DELTA;
    nfa->state_idx = 1;
    nfa->nedges = CLDM_NFA_EDGE_DELTA;
    nfa->edge_idx = 0;
    nfa->start = 0;

    nfa->states[0] = (struct cldm_nfa_state) {
        .op = cldm_nfa_op_valid,
        .edges = CLDM_NFA_EDGE_NONE
    };

    return true;
}

