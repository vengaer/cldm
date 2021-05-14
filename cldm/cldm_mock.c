#define CLDM_GENERATE_SYMBOLS
#include "cldm_macro.h"
#include "cldm_mock.h"

cldm_aligned_mockinfo mockinfos[CLDM_MAX_THREADS];
cldm_aligned_bool cldm_mock_force_disable[CLDM_MAX_THREADS];

bool cldm_mock_disabled(void);
bool cldm_mock_enabled(void);

void cldm_mock_enable_all(void) {
    for(unsigned i = 0; i < cldm_arrsize(cldm_mock_force_disable); i++) {
        cldm_mock_force_disable[i].data = false;
    }
}

void cldm_mock_disable_all(void) {
    for(unsigned i = 0; i < cldm_arrsize(cldm_mock_force_disable); i++) {
        cldm_mock_force_disable[i].data = true;
    }
}
