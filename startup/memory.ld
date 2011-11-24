MEMORY
{
  flash(rx): ORIGIN = 0x00000000, LENGTH = 32k
  sram(rwx): ORIGIN = 0x10000000+384, LENGTH = 8k-384
}
INCLUDE startup/linkscript.ld

