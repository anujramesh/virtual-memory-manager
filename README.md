# Virtual Memory Manager

This program translates logical to physical addresses for a virtual address space of size 2<sup>16</sup> = 65,536 bytes. It will read from a file containing logical addresses and, using a TLB and a page table, translates each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address. 

### Phase 1 
#### Address Translation

First, the page number is extracted from the logical address, and the TLB is consulted. In the case of a TLB hit, the frame
number is obtained from the TLB. In the case of a TLB miss, the page table must be consulted. In the latter case, either
the frame number is obtained from the page table, or a page fault occurs. A visual representation of the address-translation 
process is:
