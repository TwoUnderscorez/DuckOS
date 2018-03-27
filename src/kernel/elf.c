#include "elf.h"
#include "../drivers/screen.h"
#include "../kernel/heap.h"
#include "../libs/string.h"

static inline Elf32_Shdr_t *elf_sheader(Elf32_Ehdr_t *hdr) {
	return (Elf32_Shdr_t *)((int)hdr + hdr->e_shoff);
}
 
static inline Elf32_Shdr_t *elf_section(Elf32_Ehdr_t *hdr, int idx) {
	return &elf_sheader(hdr)[idx];
}

static inline Elf32_Phdr_t *elf_pheader(Elf32_Ehdr_t *hdr) {
	return (Elf32_Phdr_t *)((int)hdr + hdr->e_phoff);
}
 
static inline Elf32_Phdr_t *elf_segment(Elf32_Ehdr_t *hdr, int idx) {
	return &elf_pheader(hdr)[idx];
}

int elf_check_file(Elf32_Ehdr_t *hdr) {
	if(!hdr) return 0;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		puts("ELF Header EI_MAG0 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		puts("ELF Header EI_MAG1 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		puts("ELF Header EI_MAG2 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		puts("ELF Header EI_MAG3 incorrect.\n");
		return 0;
	}
	return 1;
}

int elf_check_supported(Elf32_Ehdr_t *hdr) {
	if(!elf_check_file(hdr)) {
		puts("Invalid ELF File.\n");
		return 0;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		puts("Unsupported ELF File Class.\n");
		return 0;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		puts("Unsupported ELF File byte order.\n");
		return 0;
	}
	if(hdr->e_machine != EM_386) {
		puts("Unsupported ELF File target.\n");
		return 0;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		puts("Unsupported ELF File version.\n");
		return 0;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		puts("Unsupported ELF File type.\n");
		return 0;
	}
	return 1;
}

static int elf_get_symval(Elf32_Ehdr_t *hdr, int table, unsigned int idx) {
	if(table == SHN_UNDEF || idx == SHN_UNDEF) return 0;
	Elf32_Shdr_t *symtab = elf_section(hdr, table);
 
	unsigned int symtab_entries = symtab->sh_size / symtab->sh_entsize;
	if(idx >= symtab_entries) {
		puts("Symbol Index out of Range ");
		screen_print_int(table, 10);
		putc(':');
		screen_print_int(idx, 10);
		puts("\n");
		return 0;
	}
 
	int symaddr = (int)hdr + symtab->sh_offset;
	Elf32_Sym_t *symbol = &((Elf32_Sym_t *)symaddr)[idx];
    if(symbol->st_shndx == SHN_UNDEF) {
		// External symbol, lookup value
		Elf32_Shdr_t *strtab = elf_section(hdr, symtab->sh_link);
		const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;
 
		void *target = elf_lookup_symbol(name);
 
		if(target == 0) {
			// Extern symbol not found
			if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
				// Weak symbol initialized as 0
				return 0;
			} else {
				puts("Undefined External Symbol : ");
				puts(name);
				puts("\n");
				return 0;
			}
		} else {
			return (int)target;
		}
        	} else if(symbol->st_shndx == SHN_ABS) {
		// Absolute symbol
		return symbol->st_value;
	} else {
		// Internally defined symbol
		Elf32_Shdr_t *target = elf_section(hdr, symbol->st_shndx);
		return (int)hdr + symbol->st_value + target->sh_offset;
	}
}

static int elf_do_reloc(Elf32_Ehdr_t *hdr, Elf32_Rel_t *rel, Elf32_Shdr_t *reltab) {
	/* retrieve the section header for the section wherein the 
	 * symbol exists and computes a reference to the symbol.
	 */
	Elf32_Shdr_t *target = elf_section(hdr, reltab->sh_info);
 
	int addr = (int)hdr + target->sh_offset;
	int *ref = (int *)(addr + rel->r_offset);

	// Symbol value
	int symval = 0;
	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
		symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(rel->r_info));
		if(symval == ELF_RELOC_ERR) return ELF_RELOC_ERR;
	}

	// Relocate based on type
	switch(ELF32_R_TYPE(rel->r_info)) {
		case R_386_NONE:
			// No relocation
			break;
		case R_386_32:
			// Symbol + Offset
			*ref = DO_386_32(symval, *ref);
			break;
		case R_386_PC32:
			// Symbol + Offset - Section Offset
			*ref = DO_386_PC32(symval, *ref, (int)ref);
			break;
		default:
			// Relocation type not supported, display puts and return
			puts("Unsupported Relocation Type ");
			screen_print_int(ELF32_R_TYPE(rel->r_info), 10);
			puts("\n");
			return ELF_RELOC_ERR;
	}
	return symval;
}

static int elf_map_pdpt(Elf32_Ehdr_t *hdr) {
	Elf32_Phdr_t *phdr = elf_pheader(hdr);
	unsigned int i;
	// Iterate over section headers
	for(i = 0; i < hdr->e_phnum; i++) {
		Elf32_Phdr_t *segment = &phdr[i];
		// Skip if it the section is empty
		if(!segment->p_memsz) return -1;
		// add mapping in pdpt based on section->sh_addr
		// and section->sh_entsize
		puts("vaddr: ");
		screen_print_int(segment->p_vaddr, 16);
		puts(" Size: "); 
		screen_print_int(segment->p_memsz, 10);
		puts("\n");
	}
	return 0;
}

static int elf_load_stage1(Elf32_Ehdr_t *hdr) {
	Elf32_Shdr_t *shdr = elf_sheader(hdr);
 
	unsigned int i;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr_t *section = &shdr[i];
		// If the section isn't present in the file
		if(section->sh_type == SHT_NOBITS) {
			// Skip if it the section is empty
			if(!section->sh_size) continue;
			// If the section should appear in memory
			if(section->sh_flags & SHF_ALLOC) {
				// Allocate and zero some memory
                // TODO: Adapt to paging 
				void *mem = malloc(section->sh_size);
				memset(mem, 0, section->sh_size);
 
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)hdr;
				puts("Allocated memory for a section "); 
				screen_print_int(section->sh_size, 10);
				puts("\n");
			}
		}
	}
	return 0;
}

static int elf_load_stage2(Elf32_Ehdr_t *hdr) {
	Elf32_Shdr_t *shdr = elf_sheader(hdr);
 
	unsigned int i, idx;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr_t *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel_t *reltab = &((Elf32_Rel_t *)((int)hdr + section->sh_offset))[idx];
				int result = elf_do_reloc(hdr, reltab, section);
				// On puts, display a message and return
				if(result == ELF_RELOC_ERR) {
					puts("Failed to relocate symbol.\n");
					return ELF_RELOC_ERR;
				}
			}
		}
	}
	return 0;
}

// static inline void *elf_load_rel(Elf32_Ehdr_t *hdr) {
// 	int result;
// 	result = elf_load_stage1(hdr);
// 	if(result == ELF_RELOC_ERR) {
// 		puts("Unable to load ELF file.\n");
// 		return 0;
// 	}
// 	result = elf_load_stage2(hdr);
// 	if(result == ELF_RELOC_ERR) {
// 		puts("Unable to load ELF file.\n");
// 		return 0;
// 	}
// 	// TODO : Parse the program header (if present)
// 	return (void *)hdr->e_entry;
// }
 
void *elf_load_file(void *file) {
	Elf32_Ehdr_t *hdr = (Elf32_Ehdr_t *)file;
	if(!elf_check_supported(hdr)) {
		puts("ELF File cannot be loaded.\n");
		return 0;
	}
	switch(hdr->e_type) {
		case ET_EXEC:
			puts("ET_EXEC\n");
			elf_map_pdpt(hdr);
			return 0;
		case ET_REL:
            puts("ET_REL\n");
			return 0;
	}
	return 0;
}

void *elf_lookup_symbol(const char *name) {
	return 0;
}

