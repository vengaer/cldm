#include <elf.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct makecfg {
    char has_generic;
    char const *arch;
    char const *abi;
};

static struct makecfg makecfg;

static void usage(char const *argv0);
static bool write_config(char const *out);
static void detect_generic(void);
static bool read_elf(char const *argv0);

int main(int argc, char **argv)  {
    if(argc < 2) {
        fputs("No outfile specified\n", stderr);
        usage(argv[0]);
        return 1;
    }

    detect_generic();
    if(!read_elf(argv[0])) {
        return 1;
    }

    return !write_config(argv[1]);
}

static void usage(char const *argv0) {
    char const *basename = strrchr(argv0, '/');
    basename = basename ? basename + 1 : argv0;
    printf("Usage: %s OUTFILE\n", basename);
}

static bool write_config(char const *out) {
    FILE *fp;
    fp = fopen(out, "w");
    if(!fp) {
        fprintf(stderr, "Could not open %s for writing: %s\n", out, strerror(errno));
        return false;
    }

    fprintf(fp, "has_generic := %c\n", makecfg.has_generic);
    fprintf(fp, "arch        := %s\n", makecfg.arch);
    fprintf(fp, "abi         := %s\n", makecfg.abi);

    fclose(fp);
    return true;
}

static void detect_generic(void) {
    makecfg.has_generic = 'n';
#ifdef __STDC__
#if __STDC_VERSION__ >= 201112L
    makecfg.has_generic = 'y';
#endif
#endif
}

static bool read_elf(char const *argv0) {
    Elf64_Ehdr ehdr;
    int fd;
    ssize_t nbytes;
    bool success = false;

    fd = open(argv0, O_RDONLY);
    if(fd == -1) {
        fprintf(stderr, "Could not open %s: %s\n", argv0, strerror(errno));
        return false;
    }

    nbytes = read(fd, &ehdr, sizeof(ehdr));
    switch(nbytes) {
        case -1:
            fprintf(stderr, "Could not read from %s: %s\n", argv0, strerror(errno));
            goto epilogue;
        case sizeof(ehdr):
            /* NOP */
            break;
        default:
            fprintf(stderr, "Got %lld instead of %zu bytes\n", (long long)nbytes, sizeof(ehdr));
            goto epilogue;
    }

    int i = memcmp(ehdr.e_ident, (unsigned char[]){ ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3 }, 4);
    if(i) {
        fprintf(stderr, "%s is not an ELF file\n", argv0);
        goto epilogue;
    }

    switch(ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            makecfg.arch = "32-bit";
            break;
        case ELFCLASS64:
            makecfg.arch = "64-bit";
            break;
        default:
            fputs("Invalid ELF class\n", stderr);
            goto epilogue;
    }

    switch(ehdr.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV:
            makecfg.abi = "SYSV";
            break;
        case ELFOSABI_HPUX:
            makecfg.abi = "HPUX";
            break;
        case ELFOSABI_NETBSD:
            makecfg.abi = "NETBSD";
            break;
        case ELFOSABI_LINUX:
            makecfg.abi = "LINUX";
            break;
        case ELFOSABI_SOLARIS:
            makecfg.abi = "SOLARIS";
            break;
        case ELFOSABI_IRIX:
            makecfg.abi = "IRIX";
            break;
        case ELFOSABI_FREEBSD:
            makecfg.abi = "FREEBSD";
            break;
        case ELFOSABI_TRU64:
            makecfg.abi = "TRU64";
            break;
        case ELFOSABI_ARM:
            makecfg.abi = "ARM";
            break;
        case ELFOSABI_STANDALONE:
            makecfg.abi = "EMBEDDED";
            break;
        default:
            fputs("Invalid ABI\n", stderr);
            goto epilogue;
    }

    success = true;
epilogue:
    close(fd);

    return success;
}
