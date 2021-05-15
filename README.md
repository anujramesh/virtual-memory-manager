# Virtual Memory Manager

This program translates logical to physical addresses for a virtual address space of size 2<sup>16</sup> = 65,536 bytes. It reads from a file containing logical addresses and, using a TLB and a page table, translates each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address.

<img src="./address.png" alt="alt text" width="500">

### Phase 1 
#### Address Translation

First, the page number is extracted from the logical address, and the TLB is consulted. In the case of a TLB hit, the frame
number is obtained from the TLB. In the case of a TLB miss, the page table must be consulted. In the latter case, either
the frame number is obtained from the page table, or a page fault occurs. A visual representation of the address-translation 
process is:

<img src=".//vmm.png" alt="alt text" width="800">

#### Handling Page Faults
The program uses demand paging. The backing store is represented by the file `BACKING_STORE.bin`, a binary file of size 65,536 bytes. When a page fault occurs, it 
reads in a 256-byte page from the file BACKING STORE and stores it in an available page frame in physical memory. For example, if a logical address with 
page number 15 resulted in a page fault, the program would read in page 15 from BACKING STORE (pages 
begin at 0 and are 256 bytes in size) and store it in a page frame in physical memory. Once this frame is stored (and 
the page table and TLB are updated), subsequent accesses to page 15 will be resolved by either the TLB or the page table.

### Phase 2 
#### Page Replacement
Thus far, this project has assumed that physical memory is the same size as the virtual address space.
In practice however, physical memory is typically much smaller than a virtual address space. This phase of the project now 
assumes using a smaller physical address space with 128 page frames rather than 256. So at this phase, we have at most
2<sup>7</sup> valid entries in the page table (i.e., 128 pages). The program therefore keeps track of page frames and uses LRU page-replacement to resolve page faults when there is no free memory. 

### How the Program Specifically Works
The program reads in the file `addresses.txt`, 
which contains 1,000 logical addresses ranging from 0 to 65535. 
It then translates each logical address to a physical address and determine the contents of the signed byte 
stored at the correct physical address.

The program outputs a comma-separated values (csv) file that has three columns:

* Column 1: the logical address being translated (the integer value being read from `addresses.txt`).
* Column 2: the corresponding physical address (what the program translates the logical address to).
* Column 3: the signed byte value stored in physical memory at the translated physical address.

### Statistics 
After completion, the program reports the following statistics for both phase 1 and 2 at the end of the `csv` files:
1. Page-fault rate: the percentage of address references that resulted in page faults.
2. TLB hit rate: the percentage of address references that were resolved in the TLB.

### Usage

./mmu [128/256] BACKING_STORE.bin addresses.txt
