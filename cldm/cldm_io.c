#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"

#include <errno.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CLDM_CAPTURE_STDOUT "/tmp/cldm_out"
#define CLDM_CAPTURE_STDERR "/tmp/cldm_err"

FILE *cldm_stdout;
FILE *cldm_stderr;
static int cldm_stdoutfd;
static int cldm_stderrfd;

static int cldm_io_capture_standard(FILE *restrict orig, char const *restrict path, FILE **restrict replacement, int *fd) {
    int status = -1;
    *replacement = fopen(path, "w");
    if(!*replacement) {
        cldm_err("Could not open %s for writing: %s", path, strerror(errno));
        return -1;
    }

    *fd = cldm_io_redirect(orig, path, *replacement);
    if(*fd == -1) {
        cldm_err("Redirection of %d failed", *fd);
        goto epilogue;
    }

    status = 0;
epilogue:
    if(status) {
        fclose(*replacement);
    }

    return status;
}

static int cldm_io_dump_capture(FILE *restrict redirected, char const *restrict capture, FILE *restrict dumpstream, char const *restrict name) {
    char buffer[CLDM_PAGE_SIZE];
    FILE *fp;
    size_t linelen;
    int fd;
    struct stat sb;

    fflush(redirected);

    fp = fopen(capture, "r");
    if(!fp) {
        cldm_err("Could not open %s for reading: %s", capture, strerror(errno));
        return -1;
    }

    fd = fileno(fp);
    if(fstat(fd, &sb) == -1) {
        cldm_err("Could not stat captured %s: %s", name, strerror(errno));
        goto epilogue;
    }

    if(!sb.st_size) {
        goto epilogue;
    }

    cldm_log_stream(dumpstream, "\nCaptured %s:", name);
    while(fgets(buffer, sizeof(buffer), fp)) {
        linelen = cldm_ntbslen(buffer);
        buffer[linelen - 1] = '\0';
        cldm_log_stream(dumpstream, "%s", buffer);
    }

epilogue:
    fclose(fp);
    return 0;
}

int cldm_io_redirect2(FILE *restrict fp, char const *restrict path) {
    int prevfd;
    int fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    if(fd == -1) {
        cldm_err("Could not open %s for redirection: %s", path, strerror(errno));
        return -1;
    }

    fflush(fp);

    prevfd = dup(fileno(fp));

    if(dup2(fd, fileno(fp)) == -1) {
        cldm_err("Could not redirect file descriptor %d: %s", prevfd, strerror(errno));
        goto epilogue;
    }

epilogue:
    if(prevfd == fileno(fp)) {
        close(fd);
        return -1;
    }

    return prevfd;
}

int cldm_io_redirect3(FILE *restrict fp, char const *restrict path, FILE *restrict replacement) {
    int status = -1;
    int prevfd = cldm_io_redirect2(fp, path);
    if(prevfd == -1) {
        return prevfd;
    }

    if(dup2(prevfd, fileno(replacement)) == -1) {
        cldm_err("Could not replace stream associated with file descriptor %d: %s", prevfd, strerror(errno));
        goto epilogue;
    }

    status = 0;

epilogue:
    if(status) {
        cldm_io_restore(fp, prevfd);
        prevfd = -1;
    }

    return prevfd;
}

int cldm_io_restore(FILE *restrict fp, int fd) {
    int prevfd;
    int status = -1;

    fflush(fp);
    prevfd = fileno(fp);
    close(prevfd);
    if(dup2(fd, fileno(fp)) == -1) {
        cldm_err("Could not restore file descriptor %d: %s", fd, strerror(errno));
        goto epilogue;
    }

    status = 0;

epilogue:
    return status;
}

int cldm_io_capture_stdout(void) {
    return cldm_io_capture_standard(stdout, CLDM_CAPTURE_STDOUT, &cldm_stdout, &cldm_stdoutfd);
}

int cldm_io_capture_stderr(void) {
    return cldm_io_capture_standard(stderr, CLDM_CAPTURE_STDERR, &cldm_stderr, &cldm_stderrfd);
}

int cldm_io_dump_captured_stdout(void) {
    return cldm_io_dump_capture(stdout, CLDM_CAPTURE_STDOUT, cldm_stdout, "stdout");
}

int cldm_io_dump_captured_stderr(void) {
    return cldm_io_dump_capture(stderr, CLDM_CAPTURE_STDERR, cldm_stderr, "stderr");
}

int cldm_io_restore_stdout(void) {
    int status = cldm_io_restore(cldm_stdout, cldm_stdoutfd);
    fclose(cldm_stdout);
    cldm_stdout = 0;
    return status;
}

int cldm_io_restore_stderr(void) {
    int status = cldm_io_restore(cldm_stderr, cldm_stderrfd);
    fclose(cldm_stderr);
    cldm_stderr = 0;
    return status;
}

int cldm_io_remove_captured_stdout(void) {
    int res = remove(CLDM_CAPTURE_STDOUT);
    if(res) {
        cldm_err("Could not remove %s: %s", CLDM_CAPTURE_STDOUT, strerror(errno));
    }
    return res;
}

int cldm_io_remove_captured_stderr(void) {
    int res = remove(CLDM_CAPTURE_STDERR);
    if(res) {
        cldm_err("Could not remove %s: %s", CLDM_CAPTURE_STDERR, strerror(errno));
    }
    return res;
}
