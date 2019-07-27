#include "elf.h"
#include "memory.h"
#include "task.h"
#include "../drivers/screen.h"
#include "../kernel/heap.h"
#include "../libs/string.h"
#include "../asm/asmio.h"

#pragma region inline conversion
/**
 * @brief Get ELF section headers ptr from ELF header ptr  
 * 
 * @param hdr 
 * @return Elf32_Shdr_t* 
 */
static inline Elf32_Shdr_t *elf_sheader(Elf32_Ehdr_t *hdr)
{
	return (Elf32_Shdr_t *)((int)hdr + hdr->e_shoff);
}

/**
 * @brief Get specific ELF section header ptr from ELF header ptr  
 * 
 * @param hdr 
 * @param idx 
 * @return Elf32_Shdr_t* 
 */
static inline Elf32_Shdr_t *elf_section(Elf32_Ehdr_t *hdr, int idx)
{
	return &elf_sheader(hdr)[idx];
}

/**
 * @brief Get ELF program headers ptr form ELF header
 * 
 * @param hdr 
 * @return Elf32_Phdr_t* 
 */
static inline Elf32_Phdr_t *elf_pheader(Elf32_Ehdr_t *hdr)
{
	return (Elf32_Phdr_t *)((int)hdr + hdr->e_phoff);
}

/**
 * @brief Get specific ELF segment header ptr form ELF header
 * 
 * @param hdr 
 * @param idx 
 * @return Elf32_Phdr_t* 
 */
static inline Elf32_Phdr_t *elf_segment(Elf32_Ehdr_t *hdr, int idx)
{
	return &elf_pheader(hdr)[idx];
}
#pragma endregion

/**
 * @brief Check the ELF header of the file
 * 
 * @param hdr 
 * @return int 1 if magic is correct, otherwise 0
 */
int elf_check_file(Elf32_Ehdr_t *hdr)
{
	if (!hdr)
		return 0;
	if (hdr->e_ident[EI_MAG0] != ELFMAG0)
	{
		puts("ELF Header EI_MAG0 incorrect.\n");
		return 0;
	}
	if (hdr->e_ident[EI_MAG1] != ELFMAG1)
	{
		puts("ELF Header EI_MAG1 incorrect.\n");
		return 0;
	}
	if (hdr->e_ident[EI_MAG2] != ELFMAG2)
	{
		puts("ELF Header EI_MAG2 incorrect.\n");
		return 0;
	}
	if (hdr->e_ident[EI_MAG3] != ELFMAG3)
	{
		puts("ELF Header EI_MAG3 incorrect.\n");
		return 0;
	}
	return 1;
}

/**
 * @brief Check if the ELF format is supported
 * 
 * @param hdr 
 * @return int 1 if ok, otherwise 0
 */
int elf_check_supported(Elf32_Ehdr_t *hdr)
{
	if (!elf_check_file(hdr))
	{
		puts("Invalid ELF File.\n");
		return 0;
	}
	if (hdr->e_ident[EI_CLASS] != ELFCLASS32)
	{
		puts("Unsupported ELF File Class.\n");
		return 0;
	}
	if (hdr->e_ident[EI_DATA] != ELFDATA2LSB)
	{
		puts("Unsupported ELF File byte order.\n");
		return 0;
	}
	if (hdr->e_machine != EM_386)
	{
		puts("Unsupported ELF File target.\n");
		return 0;
	}
	if (hdr->e_ident[EI_VERSION] != EV_CURRENT)
	{
		puts("Unsupported ELF File version.\n");
		return 0;
	}
	if (hdr->e_type != ET_REL && hdr->e_type != ET_EXEC)
	{
		puts("Unsupported ELF File type.\n");
		return 0;
	}
	return 1;
}

static int elf_map_pdpt(Elf32_Ehdr_t *hdr, page_directory_pointer_table_entry_t *pdpt)
{
	Elf32_Phdr_t *phdr = elf_pheader(hdr);
	unsigned int i, pdpt_bk;
	page_table_entry_t *data = malloc(sizeof(page_table_entry_t));
	memset((void *)data, '\0', sizeof(page_table_entry_t));
	// Iterate over program headers
	for (i = 0; i < hdr->e_phnum; i++)
	{
		Elf32_Phdr_t *segment = &phdr[i];
		// Skip if it the segment is empty
		if (!segment->p_memsz)
			return -1;
		data->present = 1;
		data->ro_rw = 1;
		data->kernel_user = 1;
		map_vaddr_to_pdpt(pdpt, data, segment->p_vaddr, segment->p_vaddr + segment->p_memsz);
		pdpt_bk = swapPageDirectoryAsm((unsigned int *)pdpt);
		memcpy((void *)(segment->p_vaddr),
			   (void *)((unsigned int)hdr + (unsigned int)segment->p_offset),
			   segment->p_filesz);
		swapPageDirectoryAsm((unsigned int *)pdpt_bk);
	}
	free(data);
	return 0;
}

static void elf_init_exec(Elf32_Ehdr_t *hdr, int argc, char **argv)
{
	// Create pdpt
	page_directory_pointer_table_entry_t *pdpt;
	pdpt = (page_directory_pointer_table_entry_t *)create_pdpt();
	// Map program headers to pdpt
	elf_map_pdpt(hdr, pdpt);
	// Setup the stack and heap
	unsigned int usr_esp = 0x6FFFFF, isr_esp = 0x601FFF, heap_start = 0x700000, argv_addr = 0x600000, pdpt_bk, i, len;
	page_table_entry_t *data = malloc(sizeof(page_table_entry_t));
	memset((void *)data, '\0', sizeof(page_table_entry_t));
	data->present = 1;
	data->ro_rw = 1;
	data->kernel_user = 1;
	map_vaddr_to_pdpt(pdpt, data, usr_esp - 0xFFF, usr_esp);
	map_vaddr_to_pdpt(pdpt, data, heap_start, heap_start + 1);
	// map_vaddr_to_pdpt(pdpt, data, isr_esp-0xFFF, isr_esp);
	// Setup the task
	task_t *elf_task = malloc(sizeof(task_t));
	memset((void *)elf_task, '\0', sizeof(task_t));
	task_create(elf_task, (void *)hdr->e_entry, 0x0, (unsigned int)pdpt, usr_esp, isr_esp);
	strcpy(elf_task->name, argv[0]);
	task_add(elf_task);
	// Setup argc argv
	void *argv_mem_ptr = malloc(0x1000);
	char **argv_ptr = malloc(0x100);
	for (i = 0; i < argc; i++)
	{
		len = strlen(argv[i]) + 1;
		memcpy(argv_mem_ptr, argv[i], len);
		argv_ptr[i] = argv_mem_ptr;
		argv_mem_ptr += len + 1;
	}
	map_vaddr_to_pdpt(pdpt, data, argv_addr, argv_addr + 1);
	void *argv_mem_ptr2 = (void *)argv_addr + 0x100;
	char **argv_ptr2 = (char **)argv_addr;
	pdpt_bk = swapPageDirectoryAsm((unsigned int *)pdpt);
	for (i = 0; i < argc; i++)
	{
		len = strlen(argv_ptr[i]) + 1;
		memcpy(argv_mem_ptr2, argv_ptr[i], len);
		argv_ptr2[i] = argv_mem_ptr2;
		argv_mem_ptr2 += len + 1;
	}
	swapPageDirectoryAsm((unsigned int *)pdpt_bk);
	elf_task->regs.esi = argc;
	elf_task->regs.edi = (unsigned int)argv_ptr2;
	free(data);
	free((void *)argv_mem_ptr);
	free((void *)argv_ptr);
}

void *elf_load_file(void *file, int argc, char **argv)
{
	Elf32_Ehdr_t *hdr = (Elf32_Ehdr_t *)file;
	if (!elf_check_supported(hdr))
	{
		puts("ELF File cannot be loaded.\n");
		return 0;
	}
	switch (hdr->e_type)
	{
	case ET_EXEC:
		elf_init_exec(hdr, argc, argv);
		return 0;
	case ET_REL:
		puts("ET_REL - Unsupported\n");
		return 0;
	}
	return 0;
}

void *elf_lookup_symbol(const char *name)
{
	return 0;
}

#pragma region reloc will be implemented later
/**
 * @brief Will be implemented later
 * 
 */
// static int elf_get_symval(Elf32_Ehdr_t *hdr, int table, unsigned int idx)
// {
// 	if (table == SHN_UNDEF || idx == SHN_UNDEF)
// 		return 0;
// 	Elf32_Shdr_t *symtab = elf_section(hdr, table);

// 	unsigned int symtab_entries = symtab->sh_size / symtab->sh_entsize;
// 	if (idx >= symtab_entries)
// 	{
// 		puts("Symbol Index out of Range ");
// 		screen_print_int(table, 10);
// 		putc(':');
// 		screen_print_int(idx, 10);
// 		puts("\n");
// 		return 0;
// 	}

// 	int symaddr = (int)hdr + symtab->sh_offset;
// 	Elf32_Sym_t *symbol = &((Elf32_Sym_t *)symaddr)[idx];
// 	if (symbol->st_shndx == SHN_UNDEF)
// 	{
// 		// External symbol, lookup value
// 		Elf32_Shdr_t *strtab = elf_section(hdr, symtab->sh_link);
// 		const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;

// 		void *target = elf_lookup_symbol(name);

// 		if (target == 0)
// 		{
// 			// Extern symbol not found
// 			if (ELF32_ST_BIND(symbol->st_info) & STB_WEAK)
// 			{
// 				// Weak symbol initialized as 0
// 				return 0;
// 			}
// 			else
// 			{
// 				puts("Undefined External Symbol : ");
// 				puts(name);
// 				puts("\n");
// 				return 0;
// 			}
// 		}
// 		else
// 		{
// 			return (int)target;
// 		}
// 	}
// 	else if (symbol->st_shndx == SHN_ABS)
// 	{
// 		// Absolute symbol
// 		return symbol->st_value;
// 	}
// 	else
// 	{
// 		// Internally defined symbol
// 		Elf32_Shdr_t *target = elf_section(hdr, symbol->st_shndx);
// 		return (int)hdr + symbol->st_value + target->sh_offset;
// 	}
// }

/**
 * @brief Will be implemented later
 * 
 */
// static int elf_do_reloc(Elf32_Ehdr_t *hdr, Elf32_Rel_t *rel, Elf32_Shdr_t *reltab)
// {
// 	/* retrieve the section header for the section wherein the
// 	 * symbol exists and computes a reference to the symbol.
// 	 */
// 	Elf32_Shdr_t *target = elf_section(hdr, reltab->sh_info);

// 	int addr = (int)hdr + target->sh_offset;
// 	int *ref = (int *)(addr + rel->r_offset);

// 	// Symbol value
// 	int symval = 0;
// 	if (ELF32_R_SYM(rel->r_info) != SHN_UNDEF)
// 	{
// 		symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(rel->r_info));
// 		if (symval == ELF_RELOC_ERR)
// 			return ELF_RELOC_ERR;
// 	}

// 	// Relocate based on type
// 	switch (ELF32_R_TYPE(rel->r_info))
// 	{
// 	case R_386_NONE:
// 		// No relocation
// 		break;
// 	case R_386_32:
// 		// Symbol + Offset
// 		*ref = DO_386_32(symval, *ref);
// 		break;
// 	case R_386_PC32:
// 		// Symbol + Offset - Section Offset
// 		*ref = DO_386_PC32(symval, *ref, (int)ref);
// 		break;
// 	default:
// 		// Relocation type not supported, display puts and return
// 		puts("Unsupported Relocation Type ");
// 		screen_print_int(ELF32_R_TYPE(rel->r_info), 10);
// 		puts("\n");
// 		return ELF_RELOC_ERR;
// 	}
// 	return symval;
// }
/**
 * @brief Will be implemented later
 * 
 */
// static int elf_load_stage1(Elf32_Ehdr_t *hdr)
// {
// 	Elf32_Shdr_t *shdr = elf_sheader(hdr);

// 	unsigned int i;
// 	// Iterate over section headers
// 	for (i = 0; i < hdr->e_shnum; i++)
// 	{
// 		Elf32_Shdr_t *section = &shdr[i];
// 		// If the section isn't present in the file
// 		if (section->sh_type == SHT_NOBITS)
// 		{
// 			// Skip if it the section is empty
// 			if (!section->sh_size)
// 				continue;
// 			// If the section should appear in memory
// 			if (section->sh_flags & SHF_ALLOC)
// 			{
// 				// Allocate and zero some memory
// 				// TODO: Adapt to paging
// 				void *mem = malloc(section->sh_size);
// 				memset(mem, 0, section->sh_size);

// 				// Assign the memory offset to the section offset
// 				section->sh_offset = (int)mem - (int)hdr;
// 				puts("Allocated memory for a section ");
// 				screen_print_int(section->sh_size, 10);
// 				puts("\n");
// 				free(mem);
// 			}
// 		}
// 	}
// 	return 0;
// }

/**
 * @brief Will be implemented later
 * 
 */
// static int elf_load_stage2(Elf32_Ehdr_t *hdr)
// {
// 	Elf32_Shdr_t *shdr = elf_sheader(hdr);

// 	unsigned int i, idx;
// 	// Iterate over section headers
// 	for (i = 0; i < hdr->e_shnum; i++)
// 	{
// 		Elf32_Shdr_t *section = &shdr[i];

// 		// If this is a relocation section
// 		if (section->sh_type == SHT_REL)
// 		{
// 			// Process each entry in the table
// 			for (idx = 0; idx < section->sh_size / section->sh_entsize; idx++)
// 			{
// 				Elf32_Rel_t *reltab = &((Elf32_Rel_t *)((int)hdr + section->sh_offset))[idx];
// 				int result = elf_do_reloc(hdr, reltab, section);
// 				// On puts, display a message and return
// 				if (result == ELF_RELOC_ERR)
// 				{
// 					puts("Failed to relocate symbol.\n");
// 					return ELF_RELOC_ERR;
// 				}
// 			}
// 		}
// 	}
// 	return 0;
// }

/**
 * @brief Will be implemented later
 * 
 */
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
#pragma endregion