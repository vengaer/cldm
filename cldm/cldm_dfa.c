#include "cldm_dfa.h"
#include "cldm_log.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_DFA_STATE_DELTA = 64 };
enum { CLDM_DFA_EDGE_DELTA = 128 };
enum { CLDM_DFA_EDGE_NONE = -1 };

struct cldm_dfa_state {
    int edges;
    int shortop;
};

struct cldm_dfa_edge {
    int ch;
    int next;
    int end;
};

void cldm_dfa_free(struct cldm_dfa *dfa);

static bool cldm_dfa_ensure_state_capacity(struct cldm_dfa *dfa, unsigned nrequested) {
    struct cldm_dfa_state *states;
    if(dfa->state_idx + nrequested < dfa->nstates) {
        return true;
    }
    states = realloc(dfa->states, (dfa->nstates + CLDM_DFA_STATE_DELTA) * sizeof(*states));
    if(!states) {
        cldm_err("Could not increase total number of states");
        return false;
    }
    dfa->states = states;
    dfa->nstates += CLDM_DFA_STATE_DELTA;
    return true;
}

static bool cldm_dfa_ensure_edge_capacity(struct cldm_dfa *dfa, unsigned nrequested) {
    struct cldm_dfa_edge *edges;
    if(dfa->edge_idx + nrequested < dfa->nedges) {
        return true;
    }
    edges = realloc(dfa->edges, (dfa->nedges + CLDM_DFA_EDGE_DELTA) * sizeof(*edges));
    if(!edges) {
        cldm_err("Could not increase total number of edges");
        return false;
    }
    dfa->edges = edges;
    dfa->nedges += CLDM_DFA_EDGE_DELTA;
    return true;
}

static inline int cldm_dfa_edge_pop(struct cldm_dfa *dfa) {
    if(!cldm_dfa_ensure_edge_capacity(dfa, 1)) {
        return -1;
    }

    return dfa->edge_idx++;
}

static inline int cldm_dfa_state_pop(struct cldm_dfa *dfa) {
    if(!cldm_dfa_ensure_state_capacity(dfa, 1)) {
        return -1;
    }

    return dfa->state_idx++;
}

static int cldm_dfa_append(struct cldm_dfa *dfa, int state, int ch, int shortop) {
    int edge;

    edge = cldm_dfa_edge_pop(dfa);
    if(edge == -1) {
        return -1;
    }
    dfa->edges[edge] = (struct cldm_dfa_edge) {
        .ch = ch,
        .next = dfa->states[state].edges,
        .end = cldm_dfa_state_pop(dfa)
    };

    if(dfa->edges[edge].end == -1) {
        return -1;
    }

    dfa->states[state].edges = edge;

    state = dfa->edges[edge].end;
    dfa->states[dfa->edges[edge].end] = (struct cldm_dfa_state) {
        .edges = CLDM_DFA_EDGE_NONE,
        .shortop = shortop
    };
    return state;
}

bool cldm_dfa_add_argument(struct cldm_dfa *restrict dfa, char const *restrict arg, int shortop) {
    int state = dfa->start;
    int edge;
    for(; *arg; ++arg) {
        /* Check for existing edge */
        for(edge = dfa->states[state].edges; edge != CLDM_DFA_EDGE_NONE; edge = dfa->edges[edge].next) {
            if(dfa->edges[edge].ch == *arg) {
                state = dfa->edges[edge].end;
                break;
            }
        }

        /* No edge matched, add one */
        if(edge == CLDM_DFA_EDGE_NONE) {
            state = cldm_dfa_append(dfa, state, *arg, 0);
            if(state == -1) {
                return false;
            }
        }
    }
    dfa->states[state].shortop = shortop;
    return true;
}

int cldm_dfa_simulate(struct cldm_dfa const *restrict dfa, char const *restrict input, int *restrict shortop) {
    struct cldm_dfa_state *state = &dfa->states[dfa->start];
    int edge;
    char const *pos = input;

    /* Simulate */
    for(; *pos; ++pos) {
        for(edge = state->edges; edge != CLDM_DFA_EDGE_NONE; edge = dfa->edges[edge].next) {
            if(dfa->edges[edge].ch == *pos) {
                state = &dfa->states[dfa->edges[edge].end];
                break;
            }
        }

        if(edge == CLDM_DFA_EDGE_NONE) {
            break;
        }
    }

    /* Check whether state is an accept state */
    if(state->shortop) {
        *shortop = state->shortop;
        return pos - input;
    }

    *shortop = 0;
    return -1;
}

bool cldm_dfa_init(struct cldm_dfa *dfa) {
    dfa->states = malloc(CLDM_DFA_STATE_DELTA * sizeof(*dfa->states));
    if(!dfa->states) {
        cldm_err("Could not allocate dfa nodes for argument parsing");
        return false;
    }

    dfa->edges = malloc(CLDM_DFA_EDGE_DELTA * (sizeof(*dfa->edges)));
    if(!dfa->edges) {
        cldm_err("Could not allocate dfa edges for arguments parsing");
        free(dfa->states);
        return false;
    }

    dfa->nstates = CLDM_DFA_STATE_DELTA;
    dfa->state_idx = 1;
    dfa->nedges = CLDM_DFA_EDGE_DELTA;
    dfa->edge_idx = 0;
    dfa->start = 0;

    dfa->states[0] = (struct cldm_dfa_state) {
        .edges = CLDM_DFA_EDGE_NONE,
        .shortop = 0
    };

    return true;
}

