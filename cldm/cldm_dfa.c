#include "cldm_dfa.h"
#include "cldm_log.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_NFA_STATE_DELTA = 64 };
enum { CLDM_NFA_EDGE_DELTA = 128 };
enum { CLDM_NFA_EDGE_NONE = -1 };

enum cldm_dfa_op {
    cldm_dfa_op_accept,
    cldm_dfa_op_valid
};

struct cldm_dfa_state {
    enum cldm_dfa_op op;
    int edges;
};

struct cldm_dfa_edge {
    int ch;
    int next;
    int end;
};

void cldm_dfa_free(struct cldm_dfa *dfa);

static bool cldm_dfa_ensure_state_capacity(struct cldm_dfa *dfa, unsigned nrequested) {
    struct cldm_dfa_state *states;
    if(dfa->state_idx  + nrequested < dfa->nstates) {
        return true;
    }
    states = realloc(dfa->states, (dfa->nstates + CLDM_NFA_STATE_DELTA) * sizeof(*states));
    if(!states) {
        cldm_err("Could not increase total number of states");
        return false;
    }
    dfa->states = states;
    dfa->nstates += CLDM_NFA_STATE_DELTA;
    return true;
}

static bool cldm_dfa_ensure_edge_capacity(struct cldm_dfa *dfa, unsigned nrequested) {
    struct cldm_dfa_edge *edges;
    if(dfa->edge_idx + nrequested < dfa->nedges) {
        return true;
    }
    edges = realloc(dfa->edges, (dfa->nedges + CLDM_NFA_EDGE_DELTA) * sizeof(*edges));
    if(!edges) {
        cldm_err("Could not increase total number of edges");
        return false;
    }
    dfa->edges = edges;
    dfa->nedges += CLDM_NFA_EDGE_DELTA;
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

static struct cldm_dfa_state *cldm_dfa_append(struct cldm_dfa *restrict dfa, struct cldm_dfa_state *restrict current, int ch, enum cldm_dfa_op op) {
    int edge;

    edge = cldm_dfa_edge_pop(dfa);
    if(edge == -1) {
        return 0;
    }
    dfa->edges[edge] = (struct cldm_dfa_edge) {
        .ch = ch,
        .next = current->edges,
        .end = cldm_dfa_state_pop(dfa)
    };

    if(dfa->edges[edge].end == -1) {
        return 0;
    }

    current->edges = edge;

    current = &dfa->states[dfa->edges[edge].end];
    *current = (struct cldm_dfa_state) {
        .op = op,
        .edges = CLDM_NFA_EDGE_NONE
    };
    return current;
}

bool cldm_dfa_add_argument(struct cldm_dfa *restrict dfa, char const *restrict arg) {
    struct cldm_dfa_state *state = &dfa->states[dfa->start];
    int edge;
    for(; *arg; ++arg) {
        /* Check for existing edge */
        for(edge = state->edges; edge != CLDM_NFA_EDGE_NONE; edge = dfa->edges[edge].next) {
            if(dfa->edges[edge].ch == *arg) {
                state = &dfa->states[dfa->edges[edge].end];
                break;
            }
        }

        /* No edge matched, add one */
        if(edge == CLDM_NFA_EDGE_NONE) {
            state = cldm_dfa_append(dfa, state, *arg, cldm_dfa_op_valid);
            if(!state) {
                return false;
            }
        }
    }

    return true;
}

char const *cldm_dfa_simulate(struct cldm_dfa const *restrict dfa, char const *restrict input) {
    struct cldm_dfa_state *state = &dfa->states[dfa->start];
    int edge;

    /* Simulate as far as possible */
    for(; *input; ++input) {
        for(edge = state->edges; edge != CLDM_NFA_EDGE_NONE; edge = dfa->edges[edge].next) {
            if(dfa->edges[edge].ch == *input) {
                state = &dfa->states[dfa->edges[edge].end];
                break;
            }
        }

        if(edge == CLDM_NFA_EDGE_NONE) {
            break;
        }
    }

    /* Return pointer to first character not matching an edge */
    return input;
}

bool cldm_dfa_init(struct cldm_dfa *dfa) {
    dfa->states = malloc(CLDM_NFA_STATE_DELTA * sizeof(*dfa->states));
    if(!dfa->states) {
        cldm_err("Could not allocate dfa nodes for argument parsing");
        return false;
    }

    dfa->edges = malloc(CLDM_NFA_EDGE_DELTA * (sizeof(*dfa->edges)));
    if(!dfa->edges) {
        cldm_err("Could not allocate dfa edges for arguments parsing");
        free(dfa->states);
        return false;
    }

    dfa->nstates = CLDM_NFA_STATE_DELTA;
    dfa->state_idx = 1;
    dfa->nedges = CLDM_NFA_EDGE_DELTA;
    dfa->edge_idx = 0;
    dfa->start = 0;

    dfa->states[0] = (struct cldm_dfa_state) {
        .op = cldm_dfa_op_valid,
        .edges = CLDM_NFA_EDGE_NONE
    };

    return true;
}

