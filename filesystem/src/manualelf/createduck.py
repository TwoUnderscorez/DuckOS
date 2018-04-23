# This script will create an ELF file

### SECTION .TEXT
# mov ebx, 0           ; prints hello
# mov eax, HWADDR        
# mov ecx, 0   
# mov edx, 0    
# int 0x83          

# mov eax, 0            ; exits
# mov ebx, 0      
# int 0x82          

### SECTION .DATA
# HWADDR db "Hello ELF!", 0x0A  

out = ''

### ELF HEADER
# e_ident(16):
out += '\x7FELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x10'
# e_type(2) - set it to 0x02 0x00 - ELF file:
out += '\x02\x00'
# e_machine(2) - set it to 0x03 0x00 - i386:
out += '\x03\x00'
# e_version(4):
out += '\x01\x00\x00\x00'
# e_entry(4) entry point:
out += '\x80\x80\x04\x08'
# e_phoff(4) - offset from file to program header table.
out += '\x34\x00\x00\x00'
# e_shoff(4) - offset from file to section header table.
out += '\x00\x00\x00\x00'
# e_flags(4) - we don't need flags:
out += '\x00\x00\x00\x00'
# e_ehsize(2) size of the ELF header:
out += '\x34\x00'
# e_phentsize(2) - size of a program header.
out += '\x20\x00'
# e_phnum(2) - number of program headers:
out += '\x02\x00'
# e_shentsize(2), e_shnum(2), e_shstrndx(2): irrelevant:
out += '\x00\x00\x00\x00\x00\x00'

### PROGRAM HEADER 
# .text segment header
# p_type(4) type of segment:
out += '\x01\x00\x00\x00'
# p_offset(4) offset from the beginning of the file:
out += '\x80\x00\x00\x00'
# p_vaddr(4) - what virtual address to assign to segment:
out += '\x80\x80\x04\x08'
# p_paddr(4) - physical addressing is irrelevant:
out += '\x00\x00\x00\x00'
# p_filesz(4) - number of bytes in file image of segment
out += '\x24\x00\x00\x00'
# p_memsz(4) - number of bytes in memory image of segment:
out += '\x24\x00\x00\x00'
# p_flags(4):
out += '\x05\x00\x00\x00'
# p_align(4) - handles alignment to memory pages:
out += '\x00\x10\x00\x00'

# .data segment header
out += '\x01\x00\x00\x00\xA4\x00\x00\x00\xA4\x80\x04\x08\x00\x00\x00\x00'
out += '\x20\x00\x00\x00\x20\x00\x00\x00\x07\x00\x00\x00\x00\x10\x00\x00'
# padding
out += '\x00' * 12
# .text segment
out += '\xBB\x00\x00\x00\x00\xB8\xA4\x80\x04\x08\xB9\x00\x00\x00\x00\xBA'
out += '\x00\x00\x00\x00\xCD\x83\xB8\x00\x00\x00\x00\xBB\x00\x00\x00\x00'
out += '\xCD\x82'
# padding
out += '\x00\x00'
# .data segment
out += 'Hello ELF!\x0A'

f = file('elffile', 'r+wb')
f.seek(0)
f.truncate()
f.writelines([out])
f.close()