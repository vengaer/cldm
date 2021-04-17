
/* Tell address sanitizer to accept preloaded libraries */
char const *__asan_default_options(void) {
    return "verify_asan_link_order=0";
}
