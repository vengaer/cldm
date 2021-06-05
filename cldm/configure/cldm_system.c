#include <elf.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum arch {
    arch_32,
    arch_64
};

enum abi {
    abi_sysv,
    abi_hpux,
    abi_netbsd,
    abi_linux,
    abi_solaris,
    abi_irix,
    abi_freebsd,
    abi_tru64,
    abi_arm,
    abi_embedded
};

struct makecfg {
    char has_generic;
    enum arch arch;
    enum abi abi;
};

static char const *arch_strings[] = {
    "32-bit",
    "64-bit"
};

static char const *abi_strings[] = {
    "SYSV",
    "HPUX",
    "NETBSD",
    "LINUX",
    "SOLARIS",
    "IRIX",
    "FREEBSD",
    "TRU64",
    "ARM",
    "EMBEDDED"
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
    fprintf(fp, "arch        := %s\n", arch_strings[makecfg.arch]);
    fprintf(fp, "abi         := %s\n", abi_strings[makecfg.abi]);
    fprintf(fp, "pagesize    := %ld\n", sysconf(_SC_PAGESIZE));

    fclose(fp); return true;
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
            /* nop */
            break;
        default:
            fprintf(stderr, "Got %lld instead of %zu bytes\n", (long long)nbytes, sizeof(ehdr));
            goto epilogue;
    }

    if(ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
       ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3)
    {
        fprintf(stderr, "%s is not an ELF file\n", argv0);
        goto epilogue;
    }

    switch(ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            makecfg.arch = arch_32;
            break;
        case ELFCLASS64:
            makecfg.arch = arch_64;
            break;
        default:
            fputs("Invalid ELF class\n", stderr);
            goto epilogue;
    }

    switch(ehdr.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV:
            makecfg.abi = abi_sysv;
            break;
        case ELFOSABI_HPUX:
            makecfg.abi = abi_hpux;
            break;
        case ELFOSABI_NETBSD:
            makecfg.abi = abi_netbsd;
            break;
        case ELFOSABI_LINUX:
            makecfg.abi = abi_linux;
            break;
        case ELFOSABI_SOLARIS:
            makecfg.abi = abi_solaris;
            break;
        case ELFOSABI_IRIX:
            makecfg.abi = abi_irix;
            break;
        case ELFOSABI_FREEBSD:
            makecfg.abi = abi_freebsd;
            break;
        case ELFOSABI_TRU64:
            makecfg.abi = abi_tru64;
            break;
        case ELFOSABI_ARM:
            makecfg.abi = abi_arm;
            break;
        case ELFOSABI_STANDALONE:
            makecfg.abi = abi_embedded;
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
