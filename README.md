> Under active development. iON is being built in a layered strategy, where I am building it in functional layers and then adding features in each layer. Each layer will be released as a seperate package.

# iON
iON is a standalone global register allocator targeting AArch64. iON takes in an intermediate representation (IR) program with virtual registers (VRs) and produces an allocated IR -- the same representation it recieved but with every virtual register replaced with a physical register. 
