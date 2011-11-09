void reset(void);
extern void main(void);
extern void _stack_top;

__attribute__ ((section(".vector_table")))
void (* const vectors[128])(void) =
{
(void *)&_stack_top,
reset
};

void reset(void)
{
/* copy the .data section from flash to RAM */

/* zero the .bss section */

main();

while(1) ;
}
