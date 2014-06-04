/* AVR-specific support for 32-bit ELF.
   Copyright 2006, 2007 Free Software Foundation, Inc.

   Written by Bjoern Haase <bjoern.m.haase@web.de>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, 
   Boston, MA 02110-1301, USA.  */

/* These four functions will be called from the ld back-end.  */

extern void
elf32_xpic_setup_params (struct bfd_link_info *, bfd *, asection *,
                        bfd_boolean, bfd_boolean, bfd_boolean,
                        bfd_vma, bfd_boolean);

extern int
elf32_xpic_setup_section_lists (bfd *, struct bfd_link_info *);

extern bfd_boolean
elf32_xpic_size_stubs (bfd *, struct bfd_link_info *, bfd_boolean);

extern bfd_boolean
elf32_xpic_build_stubs (struct bfd_link_info *);

int xpic_saturateY(bfd_signed_vma *srel, unsigned  int mas);
int xpic_saturateYv2(int srel, unsigned  int mas);

// Insert to string table a new string (name is part of name)
int InsertStringToStrtbl(bfd *abfd,unsigned int shindex, char *name);

// Get the value of the symbol referred to by the reloc.
bfd_boolean xpic_get_symbol_value(bfd *abfd,Elf_Internal_Rela *irel,bfd_vma *symval);

// Get the value of the internal symbol referred to by the reloc.
// Internal symbol must refer to .data section
bfd_boolean xpic_get_data_internal_symbol_value(bfd *abfd,asection *data,Elf_Internal_Rela *irel,bfd_vma *symval);

// Get '.data' section, with cached data
asection *xpic_get_data_section_cached(bfd *abfd,struct bfd_link_info *link_info);

// Get internal relocation, that links to 'dst_address_of_cur_irel' as current relocation (cur_irel)
// from current section (cur_sec) but through '.data' section
Elf_Internal_Rela *xpic_get_ireloc_through_data(bfd *abfd,asection *cur_sec, Elf_Internal_Rela *cur_irel,bfd_vma dst_address_of_cur_irel,asection *data);

// Insert new internal relocation at end of section data, that is links for a AddresS
bfd_boolean xpic_insert_ireloc(bfd *abfd, struct bfd_link_info *link_info, asection *data, bfd_vma r_info, bfd_vma r_addend);

// Insert to symbol table a new symbol, that associated with section (indx_assoc_section) and has value
int xpic_insert_symbol_to_symtab(bfd *abfd, unsigned int indx_assoc_section, bfd_vma value, char *name);

