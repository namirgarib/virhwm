//
// THIS FILE IS FROM THE ORIGINAL PROJECT, USE THIS AS A REFERENCE
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define nmax 64
#define p_max 256
#define l_max 64        /* l_max is the maximum value of loop_register(v.3)  */

void falu(short alu,short A,short B,short *p_C,short *p_ov,short *p_z,short *p_m);
short multi(short m1,short m2,short mode); /* ver 4*/

int main(void)
{
   struct mip{
	short Acl,Aco,Al,Ao,MAi,MAl,MAo,ADeo,ABo,A1o,A0o; /* A-bus control */
	short Bl,Bo,Cl,Co,MDi,BMDl,BMDo,Beo,B1o,B0o;      /* B-bus control */
	short Byo,AI,Lpl,Lpc;                         /* pipe line control */
	short M1i,M1l,M2i,M2l,MPY,Mo;	/* multiplication control (ver 4)*/
	short r_w,MMDo,Memo;                             /* memory control */
	short alu;                                /* ALU operation control */
	short ncld,ovf,zf,mf,stp ;                       /* branch control */
	short emit;                                          /* emit field */
       };

   /*  -------- required bits of each instruction field  ------------------- */
   /* Acl,Aco,Al,Ao,MAi,MAl,MAo,ADeo,ABo,A1o and A0o occupy 1bit respectively*/
   /* Bl,Bo,Cl,Co,BMDl,BMDo,Beo,B1o and B0o occupy 1bit respectively         */
   /* MDi field occupies 2 bits                                              */
   /* Byo,AI,Lpl and Lpc occupy 1bit respectively                            */
   /* M1i,Ml1,M2l,MPY,Mo occupy 1bit respectively (ver 4)		     */
   /* M2i occupies 2 bits    (ver 4)					     */
   /* r_w,MMDo and Memo occupy 1bit respectively                             */
   /* alu field occupies 4 bis                                               */
   /* ncld,ovf,zf,mf and stp occupy 1 bit respectively                       */
   /* emit field occupies 16 bits                                            */
   /*                                                                        */
   /* each instruction has 61-bits length                                    */
   /* ---------------------------------------------------------------------- */

   short mem[nmax];
   short ac_reg,a_reg,ma_reg,b_reg,c_reg,md_reg,A,B,C,ov,z,m,i,jump=0,aluc,emfd;
   short pre_ov,pre_z,pre_m;   /* ver 2.1 */
   short m1_reg,m2_reg,mode,m_out;  /* ver 4 */
   short loop_reg;
	  /*  A is the left side input of ALU                      */
	  /*  B is the right side input of ALU                     */
	  /*  C is the output of ALU                               */
   struct mip *p_mip,*p_mip0,s[p_max];

   short *p_C,*p_ov,*p_z,*p_m;
   int one_step,relative_pc=0,cont_term;
     /*variable "one_step" controls whether your program is executed one step*/
     /*  by one step ,or not.                                                */
     /* relative_pc is program counter in the case that initial program      */
     /* address is assumed to be zero.                                       */

   printf("Do you want to execute your program one step by one step?\n");
   printf("      yes:1  no:0      \n");
   scanf("%d",&one_step);

   /* +++++++++++++++++++++ */
   /* initialize parameters */
   /* +++++++++++++++++++++ */
/*   for(i=0;i<nmax;i++){mem[i]=0;}      ver 4.2 not intitialize memory     */
     ac_reg=0; a_reg=0; ma_reg=0; b_reg=0; c_reg=0; md_reg=0;loop_reg=0;
     m1_reg=0; m2_reg=0; m_out=0;
						     /* initialize registers */
     C=0; ov=0; z=0; m=0; /* initialize an output of ALU and alu-flags  */
     p_C=&C; p_ov=&ov; p_z=&z;  p_m=&m;
			     /* set alu-flags' address to pointer variables  */
     p_mip=&s[0];  /* initialize program counter to zero  */
     p_mip0=&s[0]; /* p_mip0 has initial instruction address  */

   /*  +++++++++++++++++++++   */
   /*  execute instructions    */
   /*  +++++++++++++++++++++   */

   /*  in order to terminate your program  ,you should write  */
   /*     an instruction whose stp field is 1.                */

   while(!(   (p_mip->stp)==1)   ) {

     /* memory control */
     /*                */

      /* memory write   */
      if((p_mip->r_w)==0){ if( ((p_mip->Memo)+(p_mip->MMDo)) < 1 ){
				      printf("error:memory data bus conflict");
				      exit(0);
				     }
			    if(((p_mip->Memo)==1)&&((p_mip->MMDo)==0)){
				      if(ma_reg < nmax){
					   /* check memory address renge */
					   mem[ma_reg]=md_reg;
					  }
				      else {
					     printf("out of memory range");
					     exit(0);
					   }
			   }
	 }
      /* memory read   */
      if((p_mip->r_w)==1){
	       if( ((p_mip->Memo)+(p_mip->MMDo)) < 1 ){
				      printf("error:memory data bus conflict");
				      exit(0);
				     }
	       if(((p_mip->Memo)==0)&&((p_mip->MDi)==0)&&((p_mip->BMDl)==1)){
				      if(ma_reg < nmax){
					   /* check memory address range   */
					   md_reg=mem[ma_reg];
					  }
				      else {
					    printf("out of memery range");
					    exit(0);
					   }
	       }
	}
     /* memory address control  */
      if( ((p_mip->MAl)==1)&&((p_mip->MAi)==0)&&((p_mip->ADeo)==0) )
				     { ma_reg=(p_mip->emit);}
			/* value of emit-field is set to ma_reg */
      if( ((p_mip->MAl)==1)&&((p_mip->MAi)==1) ) { ma_reg=C;}
			/* output of ALU is set to ma_reg */
      if( (p_mip->AI)==1) {ma_reg=ma_reg+1;}
			/* ma_reg is automatically incremented by one   */

    /* A-bus control */
    /*               */

     if( ( (p_mip->Aco)+(p_mip->Ao)+(p_mip->MAo)
	    +(p_mip->ABo)+(p_mip->A1o)+(p_mip->A0o)+(p_mip->Byo) ) < 6 ) {
				      printf("error:A-bus conflict");
				      exit(0);
				     }
     if( (p_mip->Aco)==0){A=ac_reg;}  /* A-side input of ALU is ac_reg */
     if( (p_mip->Ao)==0) {A=a_reg;}   /* A-side input of ALU is a_reg  */
     if( (p_mip->MAo)==0){A=ma_reg;}  /* A-side input of ALU is ma_reg */
     if( (p_mip->ABo)==0){A=b_reg;}   /* A-side input of ALU is b_reg  */
     if( (p_mip->A1o)==0){A=1;}       /* A-side input of ALU is 0-01   */
     if( (p_mip->A0o)==0){A=0;}       /* A-side input of ALU is 0--0   */
     if( (p_mip->Byo)==0){A=C;}
			   /* A-side input of ALU is the output of ALU */

    /* B-bus control */
    /*               */

     if( ((p_mip->Bo)+(p_mip->Co)+(p_mip->BMDo)+(p_mip->Beo)
	     +(p_mip->B1o)+(p_mip->B0o)+(p_mip->Mo)) < 6 ){   /* ver 4 */
				      printf("error:B-bus conflict");
				      exit(0);
				     }
     if( (p_mip->Bo)==0){B=b_reg;}     /* B-side input of ALU is b_reg   */
     if( (p_mip->Co)==0){B=c_reg;}     /* B-side input of ALU is c_reg   */
     if( (p_mip->BMDo)==0) {B=md_reg;} /* B-side input of ALU is md_reg  */
     if( (p_mip->Beo)==0){B=(p_mip->emit);}
				    /* B-side input of ALU is emit field */
     if( (p_mip->B1o)==0){B=1;}        /* B-side input of ALU is 0-01    */
     if( (p_mip->B0o)==0){B=0;}        /* B-side input of ALU is 0--0    */
     if( (p_mip->Mo) ==0){B=m_out;}
			 /* B-side input of ALU is multiplication result */

    /* latch control of registers */
    /*                            */

     if( (p_mip->Acl)==1 ) {ac_reg=C;}   /* ac_reg latch control */
     if( (p_mip->Al)==1 ) {a_reg=C;}     /* a_reg latch control */
     if( ( (p_mip->MAl)==1 )&&( (p_mip->MAi)==1) ) {ma_reg=C;}
					  /* ma_reg latch control */
     if( (p_mip->Bl)==1 ) {b_reg=C;}    /* b_reg latch control */
     if( (p_mip->Cl)==1 ) {c_reg=C;}    /* c_reg latch control */
     if( ( (p_mip->BMDl)==1 )&&( (p_mip->MDi)==1)&&((p_mip->ADeo)==0) )
				    {md_reg=(p_mip->emit);}
				     /* value of emit field is set to md_reg */
     if( ( (p_mip->Lpl)==1)  &&( (p_mip->ADeo)==0) ) {loop_reg=(p_mip->emit);}
				   /* value of emit field is set to loop_reg */
     if( ( (p_mip->BMDl)==1 )&&( (p_mip->MDi)==2) ) {md_reg=C;}
				     /* output of ALU is set to md_reg */
     if( ( (p_mip->M1i)==0) && ( (p_mip->M1l)==1) ) {m1_reg=C;} /* ver 4 */
				     /* output of ALU is set to m1_reg */
     if( ( (p_mip->M1i)==1) && ( (p_mip->M1l)==1) ) {m1_reg=m_out;}/* ver 4*/
			   /* output of Multiply unit is set to m1_reg */
     if( ( (p_mip->M2i)==0) && ( (p_mip->M2l)==1) ) {m2_reg=C;} /* ver 4*/
				/* output of ALU is set to m2_reg */
     if( ( (p_mip->M2i)==1) && ( (p_mip->M2l)==1) ) {m2_reg=(p_mip->emit);} /* ver 4*/
				/* value of emit field is set to m2_reg */
     if( ( (p_mip->M2i)==2) && ( (p_mip->M2l)==1) ) {m2_reg=md_reg;}/* ver 4*/
					/* correct bug:m1_reg-->m2_reg  *//* ver 4.1 */
				/* value of md_reg is set to m2_reg */

    /*  check branch condition  */
    /*                          */
     if( ((ov==1)&&((p_mip->ovf)==1)) || ((z==1)&&((p_mip->zf)==1))
	 || ((m==1)&&((p_mip->mf)==1)) ) { jump=1;}
       else { jump=0;}
     pre_ov=ov; pre_z=z; pre_m=m;  /* save previous flags  ver 2.1 */

    /*  ALU  and Multiplication */
    /*       */
     aluc=(p_mip->alu);
     falu(aluc,A,B,p_C,p_ov,p_z,p_m);
     mode=(p_mip->MPY);              /* ver 4*/
     m_out=multi(m1_reg,m2_reg,mode);/* ver 4*/

    /* loop register control    */
     if( (p_mip->Lpc)==1 ){ loop_reg=loop_reg-1;}

    /* control one step execution   */
    /* instruction  */
     if(one_step == 1){
	     printf("%d th instruction\n",relative_pc);
	     printf("Acl =%1d Aco =%1d Al  =%1d Ao  =%1d MAi =%1d MAl=%1d MAo=%1d\n",
		     p_mip->Acl,p_mip->Aco,p_mip->Al,p_mip->Ao,
		     p_mip->MAi,p_mip->MAl,p_mip->MAo);
	     printf("ADeo=%1d ABo =%1d A1o =%1d A0o =%1d Bl  =%1d Bo =%1d\n",
		     p_mip->ADeo,p_mip->ABo,p_mip->A1o,p_mip->A0o,
		     p_mip->Bl,p_mip->Bo);
	     printf("Cl  =%1d Co  =%1d MDi =%1d BMDl=%1d BMDo=%1d Beo=%1d\n",
		     p_mip->Cl,p_mip->Co,p_mip->MDi,p_mip->BMDl,p_mip->BMDo,
		     p_mip->Beo);
	     printf("B1o =%1d B0o =%1d Byo =%1d AI  =%1d Lpl =%1d Lpc=%1d\n",
		     p_mip->B1o,p_mip->B0o,p_mip->Byo,p_mip->AI,p_mip->Lpl,p_mip->Lpc);
	     printf("M1i =%1d M1l =%1d M2i =%1d M2l =%1d MPY =%1d Mo =%1d\n",
		     p_mip->M1i,p_mip->M1l,p_mip->M2i,p_mip->M2l,p_mip->MPY,p_mip->Mo); /* ver 4*/
	     printf("r_w =%1d MMDo=%1d Memo=%1d alu=%1d\n",
		     p_mip->r_w,p_mip->MMDo,p_mip->Memo,p_mip->alu);
	     printf("ncld=%1d ovf=%1d zf =%1d mf  =%1d stp =%1d emit=%7d\n",
		     p_mip->ncld,p_mip->ovf,p_mip->zf,p_mip->mf,p_mip->stp,
		     p_mip->emit);

    /* registers and memory contents */
	    printf("intermediate results of registers and memory contents:\n");
	    printf("ac_reg=%6d a_reg =%6d ma_reg=%6d\n",ac_reg,a_reg,ma_reg);
	    printf("b_reg =%6d c_reg =%6d md_reg=%6d,loop_reg=%6d\n",b_reg,c_reg,md_reg,loop_reg);
	    printf("m1_reg=%6x m2_reg=%6x multiplication result=%6x\n",
			m1_reg,m2_reg,m_out);/* ver4*/
	    printf("ov=%1d z =%1d m =%1d\n",pre_ov,pre_z,pre_m);  /* ver 2.1 */

	    printf("memory contents:\n");
	    for(i=0;i<nmax;i++){
		     if( (i!=0) && ((i%8)==0) ){printf("\n");}
		     printf("%6d",mem[i]);
		    }
	    printf("\n");
      }

    /* check the range of pipe line control register    */
     if( ((p_mip->Lpc)==1) && (loop_reg >=l_max) ){
			printf(" out of range of loop register \n");
			exit(0);
		       }

    /* branch control  */
     emfd=(p_mip->emit);
     if( (emfd>=p_max) && ( (jump==1) || ((p_mip->ncld)==1) ) ){
		    printf("program stalled");
		       /* out of range of program memory  */
		      exit(0);
		    }
     if((jump==1)||((p_mip->ncld)==1)) {  /* branch */
			     p_mip=p_mip0+emfd;
			     relative_pc=emfd;
			   }
	    else { if( ((p_mip->Lpc)==1) && (loop_reg!=0)){
			     p_mip=p_mip; relative_pc=relative_pc;
			     /* execute the same instruction    */
			   }
			else {  /* program counter counts up by 1 */
			     p_mip=p_mip+1;
			     relative_pc=relative_pc+1;
			  }
	       }

    /* termination control      */
     if( one_step == 1 ) {
	  printf(" Do you want to continue or terminate your program?\n");
	  printf(" continue:1  terminate:0\n");
	  scanf("%d",&cont_term);
	  if(cont_term == 0){exit(0);}
	}

  } /* end of main procedure  */

  /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
  /*   print out final results of registers and memory contents   */
  /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

  printf("end of program execution\n");
  printf("final results of registers and memory contents:\n");
  printf("ac_reg=%6d a_reg =%6d ma_reg=%6d\n",ac_reg,a_reg,ma_reg);
  printf("b_reg =%6d c_reg =%6d md_reg=%6d\n",b_reg,c_reg,md_reg);

  printf("memory contents:\n");
  for(i=0;i<nmax;i++){
	 if((i%8)==0){printf("\n");}
	 printf("%6d",mem[i]);
	}
  printf("\n");
}

void falu(short alu,short A,short B,short *p_C,short *p_ov,short *p_z,short *p_m)

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* ALU (16 bits-width operation)                                 */
/*                                                               */
/* A and B are input variables                                   */
/* C is an operation result                                      */
/* ov,z and m are status flags of operation result               */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
{
   short temp0;

   /*  ALU operation   */
   switch (alu) {
       case 0: /* no operation  */
	       break;

       case 1: /*  A+B  */
	       temp0=A+B;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */
	       /* check if the result is in the appropriate range  */
	       if( (A >= 0) && ( B >= 0)) {
			      if(temp0 < 0 ) {
				      *p_ov=1;
				      printf(" overflow occurred ");
				}
			  }
	       if( (A < 0) && ( B < 0)) {
			      if(temp0 > 0 ) {
				      *p_ov=1;
				      printf(" overflow occurred ");
				}
			  }
	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 2: /*  A-B  */
	       temp0 = A - B;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */
	       /* check if the result is in the appropriate range  */
	       if( (A > 0) && (B < 0)) {
			      if(temp0 < 0 ) {
				      *p_ov=1;
				      printf(" overflow occurred ");
				}
			  }
	       if( (A < 0) && ( B > 0)) {
			      if(temp0 > 0 ) {
				      *p_ov=1;
				      printf(" overflow occured ");
				}
			  }
	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 3: /*  B-A  */
	       temp0 = B - A ;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */
	       /* check if the result is in the appropriate range  */
	       if( (A < 0) && (B > 0)) {
			      if(temp0 < 0 ) {
				      *p_ov=1;
				      printf(" overflow occurred ");
				}
			  }
	       if( (A > 0) && ( B < 0)) {
			      if(temp0 > 0 ) {
				      *p_ov=1;
				      printf(" overflow occurred ");
				}
			  }
	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 4: /*  A or B  */
	       temp0 = A | B ;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */

	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 5: /*  A and B  */
	       temp0 = A & B;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */

	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 6: /*  A exclusive-or B  */
	       temp0 = A ^ B;
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */

	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;


       case 7: /*  1bit right arithmetic shift A  */
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */
               if( (A & 0x0001)==0x0001 ) { *p_ov=1;
				            printf(" shift out occurred ");
               }  /* ver 4.4 add */
            /* set ovf flag ; if 1bit right arithmetic shift A then carry LSB */

	       temp0 = A >> 1;
	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;

       case 8: /*  1bit left shift A  */
	       *p_ov=0; *p_m=0; *p_z=0;  /* initialize status flags */
               if( (A & 0x8000)==0x8000 ) { *p_ov=1;
				            printf(" carry occurred ");
               }  /* ver 4.4 add */
             /* set ovf flag ; if 1bit left shift A then carry MSB */
               temp0 = A << 1;

	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;
  }


}
short multi(short m1,short m2,short mode)
/*------------------------------------------------------------------*/
/* multiplication for 2's complement numbers.			    */
/* a decimal point is located between MSB and the second significant*/
/* bit and this routine accepts also negative numbers.		    */
/* m1,m2--inputs  			 			    */
/* mode=0---neglect 4 least significant bits			    */
/*      1---use all 16 bits 					    */
/*------------------------------------------------------------------*/
{
	short t_m1,t_m2,t_out_16=1;
	int t_out;

	t_m1=m1;	t_m2=m2;
	if(mode ==0){  /* neglect 4 least significant bits */
		/* convert positive numbers */
		t_m1=t_m1>>4;	t_m2=t_m2>>4;
		if((t_m1 < 0)&&(t_m1!=0xfffff800)){ t_m1= (t_m1^0xffffffff)+1;}
		if(t_m1==0xfffff800){ t_m1=0x07ff;}
		if((t_m2 < 0)&&(t_m2!=0xfffff800)){ t_m2= (t_m2^0xffffffff)+1;}
		if(t_m2==0xfffff800){ t_m2=0x07ff;}
		t_out=(int)t_m1*t_m2;

		/* check if overflow occurred */
		if( ( (m1&0xfffffff0)==0xffff8000 )&&( (m2&0xfffffff0)==0xffff8000 ) ){
			printf("overflow occured:result is approximated\n");
			t_out=0x3fffff;
			}
		/* convert 16 bits width */
		t_out_16=t_out>>7;
		if( ( (m1>0)&&(m2<0) ) || ( (m1<0)&&(m2>0) ) ){
			t_out_16=-t_out_16;
			}   /* convert a negative number */
		}

	if(mode ==1){  /* use all 16 bits */
		/* convert positive numbers */
		if((t_m1 < 0)&&(t_m1!=0xffff8000)){ t_m1=( t_m1^0xffffffff)+1;}
		if(t_m1==0xffff8000){ t_m1=0x7fff;}
		if((t_m2 < 0)&&(t_m2!=0xffff8000)){ t_m2=( t_m2^0xffffffff)+1;}
		if(t_m2==0xffff8000){ t_m2=0x7fff;}
		t_out=(int)t_m1*t_m2;

		/* check if overflow occurred */
		if( (m1==0xffff8000)&&(m2==0xffff8000) ){
			printf("overflow occured:result is approximated\n");
			t_out=0x3fffffff;
			}
		/* convert 16 bits width */
		t_out_16=t_out>>15;
		if( ( (m1>0)&&(m2<0) ) || ( (m1<0)&&(m2>0) ) ){
			t_out_16=-t_out_16;
			}   /* convert a negative number */
		}
	return(t_out_16);
}

