#ifndef ELF_H
#define ELF_H

// Defenitions 
# define ELF_NIDENT	    16

# define ELFMAG0	    0x7F // e_ident[EI_MAG0]
# define ELFMAG1	    'E'  // e_ident[EI_MAG1]
# define ELFMAG2	    'L'  // e_ident[EI_MAG2]
# define ELFMAG3	    'F'  // e_ident[EI_MAG3]
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	    (1)  // 32-bit Architecture

# define EM_386		    (3)  // x86 Machine Type
# define EV_CURRENT	    (1)  // ELF Current Version

# define SHN_UNDEF	    (0x00) // Undefined/Not present
# define SHN_ABS        (0x01)

# define ELF_RELOC_ERR	-1
// Macros
# define ELF32_ST_BIND(INFO)	((INFO) >> 4)
# define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)

# define ELF32_R_SYM(INFO)		((INFO) >> 8)
# define ELF32_R_TYPE(INFO)		((unsigned char)(INFO))

# define DO_386_32(S, A)		((S) + (A))
# define DO_386_PC32(S, A, P)	((S) + (A) - (P))

// Type defenitions
typedef unsigned short  Elf32_Half_t;	// Unsigned half int
typedef unsigned int    Elf32_Off_t;	// Unsigned offset
typedef unsigned int    Elf32_Addr_t;	// Unsigned address
typedef unsigned int    Elf32_Word_t;	// Unsigned int
typedef int             Elf32_Sword_t;// Signed int

 // Struct defenitions
typedef struct {
	unsigned char e_ident[ELF_NIDENT];
	Elf32_Half_t  e_type;
	Elf32_Half_t  e_machine;
	Elf32_Word_t  e_version;
	Elf32_Addr_t  e_entry;
	Elf32_Off_t	  e_phoff;
	Elf32_Off_t	  e_shoff;
	Elf32_Word_t  e_flags;
	Elf32_Half_t  e_ehsize;
	Elf32_Half_t  e_phentsize;
	Elf32_Half_t  e_phnum;
	Elf32_Half_t  e_shentsize;
	Elf32_Half_t  e_shnum;
	Elf32_Half_t  e_shstrndx;
} Elf32_Ehdr_t;

enum Elf_Ident {
	EI_MAG0		    = 0, // 0x7F
	EI_MAG1		    = 1, // 'E'
	EI_MAG2		    = 2, // 'L'
	EI_MAG3		    = 3, // 'F'
	EI_CLASS	    = 4, // Architecture (32/64)
	EI_DATA		    = 5, // Byte Order
	EI_VERSION	    = 6, // ELF Version
	EI_OSABI	    = 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		    = 9  // Padding
};

enum Elf_Type {
	ET_NONE		    = 0, // Unkown Type
	ET_REL		    = 1, // Relocatable File
	ET_EXEC		    = 2  // Executable File
};

typedef struct {
	Elf32_Word_t	sh_name;
	Elf32_Word_t	sh_type;
	Elf32_Word_t	sh_flags;
	Elf32_Addr_t	sh_addr;
	Elf32_Off_t	    sh_offset;
	Elf32_Word_t	sh_size;
	Elf32_Word_t	sh_link;
	Elf32_Word_t	sh_info;
	Elf32_Word_t	sh_addralign;
	Elf32_Word_t	sh_entsize;
} Elf32_Shdr_t;
 
enum ShT_Types {
	SHT_NULL	    = 0,   // Null section
	SHT_PROGBITS	= 1,   // Program information
	SHT_SYMTAB	    = 2,   // Symbol table
	SHT_STRTAB	    = 3,   // String table
	SHT_RELA	    = 4,   // Relocation (w/ addend)
	SHT_NOBITS	    = 8,   // Not present in file
	SHT_REL		    = 9,   // Relocation (no addend)
};
 
enum ShT_Attributes {
	SHF_WRITE	= 0x01, // Writable section
	SHF_ALLOC	= 0x02  // Exists in memory
};

typedef struct {
	Elf32_Word_t		st_name;
	Elf32_Addr_t		st_value;
	Elf32_Word_t		st_size;
	unsigned char		st_info;
	unsigned char		st_other;
	Elf32_Half_t		st_shndx;
} Elf32_Sym_t;

enum StT_Bindings {
	STB_LOCAL		= 0, // Local scope
	STB_GLOBAL		= 1, // Global scope
	STB_WEAK		= 2  // Weak, (ie. __attribute__((weak)))
};
 
enum StT_Types {
	STT_NOTYPE		= 0, // No type
	STT_OBJECT		= 1, // Variables, arrays, etc.
	STT_FUNC		= 2  // Methods or functions
};

typedef struct {
	Elf32_Addr_t		r_offset;
	Elf32_Word_t		r_info;
} Elf32_Rel_t;
 
typedef struct {
	Elf32_Addr_t		r_offset;
	Elf32_Word_t		r_info;
	Elf32_Sword_t		r_addend;
} Elf32_Rela_t;

enum RtT_Types {
	R_386_NONE		= 0, // No relocation
	R_386_32		= 1, // Symbol + Offset
	R_386_PC32		= 2  // Symbol + Offset - Section Offset
};

typedef struct {
	Elf32_Word_t	p_type;
	Elf32_Off_t		p_offset;
	Elf32_Addr_t	p_vaddr;
	Elf32_Addr_t	p_paddr;
	Elf32_Word_t	p_filesz;
	Elf32_Word_t	p_memsz;
	Elf32_Word_t	p_flags;
	Elf32_Word_t	p_align;
} Elf32_Phdr_t;

// Methods
int elf_check_file(Elf32_Ehdr_t *hdr);
int elf_check_supported(Elf32_Ehdr_t *hdr);
void *elf_load_file(void *file);
void *elf_lookup_symbol(const char *name);
#endif