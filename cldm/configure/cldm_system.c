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

enum machine {
    mach_none,
    mach_m32,
    mach_sparc,
    mach_386,
    mach_68k,
    mach_88k,
    mach_860,
    mach_mips,
    mach_parisc,
    mach_sparc32plus,
    mach_ppc,
    mach_ppc64,
    mach_s390,
    mach_arm,
    mach_sh,
    mach_sparcv9,
    mach_ia_64,
    mach_x86_64,
    mach_vax
};

struct makecfg {
    char has_generic;
    char has_noreturn;
    enum arch arch;
    enum abi abi;
    enum machine machine;
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

static char const *mach_strings[] = {
    "NONE",
    "M32",
    "SPARC",
    "386",
    "68K",
    "88K",
    "860",
    "MIPS",
    "PARISC",
    "SPARC32PLUS",
    "PPC",
    "PPC64",
    "S390",
    "ARM",
    "SH",
    "SPARCV9",
    "IA_64",
    "x86_64",
    "VAX"
};

static struct makecfg makecfg;

static void usage(char const *argv0);
static bool write_config(char const *out);
static void detect_generic(void);
static void detect_noreturn(void);
static bool read_elf(char const *argv0);

int main(int argc, char **argv)  {
    if(argc < 2) {
        fputs("No outfile specified\n", stderr);
        usage(argv[0]);
        return 1;
    }

    detect_generic();
    detect_noreturn();
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

    fprintf(fp, "has_generic  := %c\n", makecfg.has_generic);
    fprintf(fp, "has_noreturn := %c\n",makecfg.has_noreturn);
    fprintf(fp, "arch         := %s\n", arch_strings[makecfg.arch]);
    fprintf(fp, "abi          := %s\n", abi_strings[makecfg.abi]);
    fprintf(fp, "machine      := %s\n", mach_strings[makecfg.machine]);
    fprintf(fp, "pagesize     := %ld\n", sysconf(_SC_PAGESIZE));
    fprintf(fp, "l1_dcache    := %ld\n", sysconf(_SC_LEVEL1_DCACHE_LINESIZE));

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

static void detect_noreturn(void) {
    makecfg.has_noreturn = 'n';
#ifdef __STDC__
#if __STDC_VERSION__ >= 201112L
    makecfg.has_noreturn = 'y';
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

    switch(ehdr.e_machine) {
        default:
        case EM_NONE:
            makecfg.machine = mach_none;
            break;
        case EM_M32:
            makecfg.machine = mach_m32;
            break;
        case EM_SPARC:
            makecfg.machine = mach_sparc;
            break;
        case EM_386:
            makecfg.machine = mach_386;
            break;
        case EM_68K:
            makecfg.machine = mach_68k;
            break;
        case EM_88K:
            makecfg.machine = mach_88k;
            break;
        case EM_860:
            makecfg.machine = mach_860;
            break;
        case EM_MIPS:
            makecfg.machine = mach_mips;
            break;
        case EM_PARISC:
            makecfg.machine = mach_parisc;
            break;
        case EM_SPARC32PLUS:
            makecfg.machine = mach_sparc32plus;
            break;
        case EM_PPC:
            makecfg.machine = mach_ppc;
            break;
        case EM_PPC64:
            makecfg.machine = mach_ppc64;
            break;
        case EM_S390:
            makecfg.machine = mach_s390;
            break;
        case EM_ARM:
            makecfg.machine = mach_arm;
            break;
        case EM_SH:
            makecfg.machine = mach_sh;
            break;
        case EM_SPARCV9:
            makecfg.machine = mach_sparcv9;
            break;
        case EM_IA_64:
            makecfg.machine = mach_ia_64;
            break;
        case EM_X86_64:
            makecfg.machine = mach_x86_64;
            break;
        case EM_VAX:
            makecfg.machine = mach_vax;
            break;
    }

    success = true;
epilogue:
    close(fd);

    return success;
}
