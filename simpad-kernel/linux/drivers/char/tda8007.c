/*
 *  linux/drivers/char/tda8007.c
 *
 *  Copyright (C) 2001 juergen.messerer@freesurf.ch, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 *  The TDA8007B driver provides basic services for handling IO,
 *  interrupts, and accessing registers.  
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>

#include <asm/dma.h>
#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/arch/simpad.h>
#include <asm/uaccess.h>

#include "tda8007b.h"

#define TDA8007_DIRNAME "driver/tda8007"
#define REG_DIRNAME "registers"

extern void clear_cs3_bit(int value);

static struct proc_dir_entry *regdir;
static struct proc_dir_entry *tda8007dir;

static ssize_t proc_tda8007_read(struct file * file, char * buf,
                size_t nbytes, loff_t *ppos);
static ssize_t proc_tda8007_write(struct file * file, const char * buffer,
                size_t count, loff_t *ppos);

static struct file_operations proc_reg_operations = {
        read:   proc_tda8007_read,
        write:  proc_tda8007_write
};

static int __init tda8007_init();

/* ------------------------------------------------------------------------- */
void tda8007_reg_write(int reg, int val)
{
    printk("Address:%x \n", CS4_BASE+reg);
    printk("Value:%x \n", val);
    TDA_REG_WRITE(reg,val);
}
/* ------------------------------------------------------------------------- */
int tda8007_reg_read(int reg)
{
    printk("Address:%x \n", CS4_BASE+reg);
    return(TDA_REG_READ(reg)&0xff);
}
/* ------------------------------------------------------------------------- */
int tdaregs[16];
/* ------------------------------------------------------------------------- */
static void tda8007_irq(int irqnr, void *devid, struct pt_regs *regs)
{
    printk("\n****tda8007_irq****\n");
}
/* ------------------------------------------------------------------------- */
static int tda_card_present( uint cardport )
{
    int val=0;

    switch( cardport )
    {
	case CARD_PORT1:
	    if( tda8007_reg_read(TDA_MSR) & TDA_MSR_PR1 )
		val = 1;
	    break;
	case CARD_PORT2:
	    if( tda8007_reg_read(TDA_MSR) & TDA_MSR_PR2 )
		val = 1;
	    break;
	default:
	    val =0;
	    break;
    }
    
    return val;
}
/* ------------------------------------------------------------------------- */
void tda_inituart(void)
{
    int hsr_reg, fcr_reg;

    printk("Init TDA8007 Uart\n");
    hsr_reg = tda8007_reg_read(TDA_HSR);       
    tda8007_reg_write(TDA_PCR, 0x00);	
   
    tda8007_reg_write(TDA_CSR, 0x00);	
    tda8007_reg_write(TDA_CSR, TDA_CSR_SC1); 		/* select Card 1 */
    
    tda8007_reg_write(TDA_CSR, TDA_CSR_nRIU|TDA_CSR_SC1);	
    tda8007_reg_write(TDA_PCR, 0x00);	

    tda8007_reg_write(TDA_PDR, TDA_PDR_VAL); 	/* Rat v. jandu 8.9.2000 */
    tda8007_reg_write(TDA_UCR2, TDA_UCR2_DIV);
    
    tda8007_reg_write(TDA_CCR, 0x40|TDA_CCR_AC1);  /*1=XTAL/2 2=XTAL/4 3=XTAL/8  */
    tda8007_reg_write(TDA_GTR, TDA_GTR_GT1);	

    fcr_reg = tda8007_reg_read(TDA_FCR);
    tda8007_reg_write(TDA_FCR, (fcr_reg & 0xf0) | TDA_FCR_FL1);	
    
    tda8007_reg_write(TDA_FCR,  TDA_FCR_FL2|TDA_FCR_FL1|TDA_FCR_FL0);	
    tda8007_reg_write(TDA_UCR1, TDA_UCR_SS|TDA_UCR_CONV);
    tda8007_reg_write(TDA_PCR,  0x00);		
    
    while( tda8007_reg_read(TDA_USR) & TDA_USR_TBE_RBF )
    {
        hsr_reg = tda8007_reg_read(TDA_URR);
	udelay(5);
    }
}
/* ------------------------------------------------------------------------- */
void start_tda8007_sync(int volt)
{
    int i=0,j=0;	
    if( tda_card_present( CARD_PORT1 ) )
    {
        printk("Card Present ");
	tda8007_reg_write(TDA_TOR1, TDA_TOR1_TOL2|TDA_TOR1_TOL3);
	tda8007_reg_write(TDA_TOR2, TDA_TOR2_TOL16|TDA_TOR2_TOL15|
			            TDA_TOR2_TOL13|TDA_TOR2_TOL12|
			            TDA_TOR2_TOL11);
	tda8007_reg_write(TDA_TOR3, 0x00);
	tda8007_reg_write(TDA_TOC,  TDA_TOC_MODE2);
	tda_inituart();		
	tda8007_reg_write(TDA_UCR2, TDA_UCR_DISAUX|TDA_UCR2_DIV); // DIS_AUX ASYNC MODE
	
	if( volt == 3 ) 
	    volt = TDA_PCR_3V_5V;
	else            
	    volt = 0x00;
		
	tda8007_reg_write(TDA_PCR, 0x00|volt); // Set /Reset,3V
	udelay(1000);		
	tda8007_reg_write(TDA_PCR, TDA_PCR_START|volt); // /Reset,3V,Start
	udelay(2000);		
	tda8007_reg_write(TDA_PCR, TDA_PCR_RSTIN|TDA_PCR_START|volt); // Set Reset High
	i=0;		
	while( 1 )// !serstat()
	{
	    if( ((msr[i]=tda8007_reg_read(TDA_MSR)) &  TDA_MSR_FE) == 0 )
	    {
	        hsr[i]=tda8007_reg_read(TDA_HSR);
		usr[i]=tda8007_reg_read(TDA_USR);
		csr[i]=tda8007_reg_read(TDA_CSR);
		urr[i]=tda8007_reg_read(TDA_URR);				
		i++;				
	    }
	    if( i == 1 )
	    {	
	        /* Reset SS */
	        tda8007_reg_write(TDA_UCR1, 
				  tda8007_reg_read(TDA_UCR1) & ~TDA_UCR_SS); 
		/* Set Autoconv high */
		tda8007_reg_write(TDA_UCR2, 
				  tda8007_reg_read(TDA_UCR2) | TDA_UCR_nAUTOCONV); 
	    }
	    
	    if( i >= BUFFSIZE )
	    {
	        printk("Buffer Overflow");				
		break;			
	    }			
	    // tda8007_reg_write(TDA_FCR, TDA_FCR_PEC0|TDA_FCR_FL0);	
	}
	hsr[i]=tda8007_reg_read(TDA_HSR);
	msr[i]=tda8007_reg_read(TDA_MSR);
	csr[i]=tda8007_reg_read(TDA_CSR);
	urr[i]=tda8007_reg_read(TDA_URR);				
	i++;			
	//serin();
	if( i==1 )
	    printk("No Characters received\n");
	else
	  for(j=0;j<i-1; j++)
	    printk("Buffer[%3d]=USR(0x%02x) HSR(0x%02x) MSR(0x%02x) CSR(0x%02x) URR(0x%02x=%c)\n",
		   j,usr[j],hsr[j],msr[j],csr[j],urr[j],pascii(urr[j]));
	
	printk("Now  USR(0x%02x) HSR(0x%02x) MSR(0x%02x) CSR(0x%02x) URR(0x%02x=%c)\n",
	    usr[j],hsr[j],msr[j],csr[j],urr[j],pascii(urr[j]));
	
	tda8007_reg_write(TDA_PCR, TDA_PCR_RSTIN|volt); // remove start
	udelay(2000);		
	tda8007_reg_write(TDA_PCR, 0x00|volt); // remove Reset
		
    } 
    else 
    {		
        tda8007_reg_write(TDA_PCR, TDA_PCR_3V_5V);
    }
	
}
/* -------------------------------------------------------------------------*/ 
int test_tda8007(void)
{
    int c, i,j, reg,end=0;

    printk("\nTDA8007 TEST:");

    for( i=0; i < 0x10; i++ )
        printk("\nTDA8007 Reg %2d = 0x%02x ", i, tda8007_reg_read(i)&0xff);

    for( i=0 ;i < 0x10; i++ )
    {
        tdaregs[i]=tda8007_reg_read(i) & 0xff;
    }
    do
    {
        printk("\nTDA8007 IRQ=%s  Command:",
	       (GPLR&(1<<10) ? "HIGH":"LOW"));

	//c=serin();
	//serout(c);
	printk("\n");
	
	switch (c )
	{
	    
	    case 'c':
		printk("\nReg?:");  
		//reg=gethex(serin,serout);
		printk("\nVal?:");  
		//i=gethex(serin,serout);			
		tda8007_reg_write(reg,i);
		j=tda8007_reg_read(reg);
		printk("Reg 0x%02x (0x%02x) now 0x%02x\n", reg, i, j);	
		break;
		
	    case 'i':
		printk("\nInit\n");
		tda8007_init();			
		
	    case 'p':
		for( i=0; i < 0x10; i++ )
		    printk("\nTDA8007 Reg %2d = 0x%02x ", i, 
			   tda8007_reg_read(i)&0xff);
		break;
	    case 'e':
		end=1;
		break;
		
	    case 'r':
		while(  1 ) // serstat() == 0
		{
		    for( i=0 ;i < 0x10; i++ )
		    {
			tdaregs[i]=tda8007_reg_read(i) & 0xff;
		    }
		}
		//serin();
		break;
			
	    case 'S':
		start_tda8007_sync(5);			
		break;
	    case 's':
		start_tda8007_sync(3);			
		break;
	    case 'w':
		while( 1 )//serstat() == 0
		{
		    for( i=0 ;i < 0x10; i++ )
		    {
			tda8007_reg_write(i,0x10-i);		
		    }
		}
		//serin();
		break;
	    
	    default :
		//serout(0x07);
		break;		
	}
	
    }while( end == 0 );
    return(0);	
}
/*-------------------------------------------------------------------------*/
static int tda8007_ioctl(struct inode *ino, struct file *filp, 
			       uint cmd, ulong arg)
{
    unsigned int val, gain;
    int ret = 0;

    if (_IOC_TYPE(cmd) != 'M')
        return -EINVAL;

    switch (_IOC_NR(cmd)) 
    {
      case TDA_INFO:
	  break;

      case TDA_INIT:
	  break;

      case TDA_SET:
	  break;

      case TDA_CARD_PRESENT:
	  break;

      case TDA_CARD_VOLT:
	  break;

      default:
	  val = 0;
	  ret = -EINVAL;
	  break;
    }
    return ret;
}
/* ------------------------------------------------------------------------- */
static ssize_t proc_tda8007_read( struct file *file, char *buf, size_t nbytes, 
			      loff_t *ppos)
{
    char outputbuf[80];
    int count = 0;
    int i     = 0;
    int i_ino = (file->f_dentry->d_inode)->i_ino;
    tda8007_reg_entry_t* current_reg = NULL;

    if ((*ppos) > 0) /* Assume reading completed in previous read*/
	return 0;

    for (i=0; i<NUM_OF_TDA8007_REG_ENTRY; i++) 
    {
	if (tda8007_regs[i].low_ino==i_ino) 
	{
	    if( tda8007_regs[i].mode == 2 ) /* write only */
	    {
		printk("%s\n", tda8007_regs[i].description);
		printk("Read operation is on this register not possible!\n");
		return -EINVAL;
	    }
	    current_reg = &tda8007_regs[i];
	    
	    break;
	}
    }
    
    if (current_reg==NULL)
	return -EINVAL;

    printk("%s\n", current_reg->description);
    count += sprintf(outputbuf, "%s: 0x%x\n",  current_reg->name,
    tda8007_reg_read( current_reg->addr ));
    /*  count = sprintf(outputbuf, "value: 0x%x\n",  
	tda8007_reg_read( current_reg->addr ));*/
    
    *ppos+=count;

    if (count>nbytes)  /* Assume output can be read at one time */
	return -EINVAL;
  
    if (copy_to_user(buf, outputbuf, count))
	return -EFAULT;

    return count;
}
/* ------------------------------------------------------------------------- */
static ssize_t proc_tda8007_write(struct file * file, const char * buffer,
                size_t count, loff_t *ppos)
{
    int i;
    unsigned long newRegValue;
    char *endp;
    int i_ino = (file->f_dentry->d_inode)->i_ino;
    tda8007_reg_entry_t* current_reg=NULL;

    for (i=0; i<NUM_OF_TDA8007_REG_ENTRY; i++) 
    {
	if (tda8007_regs[i].low_ino==i_ino) 
	{
	    if( tda8007_regs[i].mode == 1 ) /* read only */
	    {
		printk("%s\n", tda8007_regs[i].description);
		printk("Write operation is on this register not possible!\n");
		return -EINVAL;
	    }
	    current_reg = &tda8007_regs[i];
	    break;
	}
    }
    if (current_reg==NULL)
	return -EINVAL;

    newRegValue = simple_strtoul(buffer,&endp,0);
    tda8007_reg_write( current_reg->addr, newRegValue);
    return (count+endp-buffer);
}
/* ------------------------------------------------------------------------- */
static int __init tda8007_init()
{
      int i, hsr_reg, res;
      int ret = -ENODEV;
      struct proc_dir_entry *entry;
      int tda8007_major = 60;
      
      res = register_chrdev( tda8007_major, "tda8007", NULL );
      
      if(res < 0){
	  printk(KERN_WARNING "tda8007: can't get major%d\n", tda8007_major);
	  return res;
      }
      
      if( tda8007_major == 0 )
	  tda8007_major = res;
      
      set_GPIO_IRQ_edge(GPIO_SMART_CARD, GPIO_RISING_EDGE);

      ret = request_irq( IRQ_GPIO_SMART_CARD, tda8007_irq, 
			       SA_INTERRUPT, "SMARTCARD_CD", NULL );
      if (ret) {
	  printk(KERN_ERR "tda8007: unable to grab irq%d: %d\n",
		 IRQ_GPIO_SMART_CARD, ret);
	  return ret;
      }

      printk("\nInit TDA8007 IRQ=%s\n", 
	     (GPLR&(1<<10) ? "HIGH":"LOW"));

//      clear_cs3_bit(RESET_SIMCARD);


#ifdef CONFIG_PROC_FS
      /* Create two dir entries for the TDA8007 */
      tda8007dir = proc_mkdir("tda8007"/*TDA8007_DIRNAME*/, NULL);
      if (tda8007dir == NULL) {
	  printk(KERN_ERR "tda80007: can't create /proc/" TDA8007_DIRNAME "\n");
	  return(-ENOMEM);
      }
      
      regdir = proc_mkdir(REG_DIRNAME, tda8007dir);
      if (regdir == NULL) {
	  printk(KERN_ERR "tda8007: can't create /proc/" TDA8007_DIRNAME "/" REG_DIRNAME "\n");
	  return(-ENOMEM);
      }

      for(i=0;i<NUM_OF_TDA8007_REG_ENTRY;i++) {
	  entry = create_proc_entry(tda8007_regs[i].name,
				    S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH,
				    regdir);
	  if(entry) {
	      tda8007_regs[i].low_ino = entry->low_ino;
	      entry->proc_fops = &proc_reg_operations;
	  }
	  else {
	      printk( KERN_ERR
		      "tda8007: can't create /proc/" REG_DIRNAME
		      "/%s\n", tda8007_regs[i].name);
	      return(-ENOMEM);
	  }
      }

#endif // CONFIG_PROC_FS


      tda8007_reg_write(TDA_CSR, 0);
      tda8007_reg_write(TDA_CSR, TDA_CSR_nRIU);
      for( i=0; i < 16; i++ )
	  tda8007_reg_write(i,0);

      tda8007_reg_write(TDA_CSR, TDA_CSR_nRIU|TDA_CSR_SC2);
      tda8007_reg_write(TDA_PCR, 0);    /* START=0 */
      tda8007_reg_write(TDA_CSR, TDA_CSR_nRIU|TDA_CSR_SC1);
      tda8007_reg_write(TDA_PCR, 0);    /* START=0 */
      tda8007_reg_write(TDA_TOC, 0);
      tda8007_reg_write(TDA_FCR, TDA_FCR_FL2|TDA_FCR_FL1|TDA_FCR_FL0);
      
      tda8007_reg_write(TDA_UCR2, TDA_UCR_DISAUX|TDA_UCR2_DIV); // DIS_AUX DIS_CLK
      tda8007_reg_write(TDA_UCR2, TDA_UCR_DISAUX|TDA_UCR2_DIV); // DIS_AUX CLK SYNC-MODE
      hsr_reg = tda8007_reg_read(TDA_HSR);
      
      tda8007_reg_write(TDA_CCR, TDA_CCR_AC1|TDA_CCR_AC0); /* XTAL/8 */

      return 0;
}
/* ------------------------------------------------------------------------- */
static void __exit tda8007_exit(void)
{
  int i;

  free_irq(IRQ_GPIO_SMART_CARD, NULL);
     /*   kfree(my_ucb);*/

  if (regdir)
  {
       for(i=0;i<NUM_OF_TDA8007_REG_ENTRY;i++) {
	  remove_proc_entry( tda8007_regs[i].name, regdir);
       }
  }
}
/* ------------------------------------------------------------------------- */
module_init(tda8007_init);
module_exit(tda8007_exit);

MODULE_AUTHOR("Juergen Messerer <juergen.messerer@freesurf.ch>");
MODULE_DESCRIPTION("TDA8007 driver");
MODULE_LICENSE("GPL");
