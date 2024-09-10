//
// THIS FILE IS FROM THE ORIGINAL PROJECT, USE THIS AS A REFERENCE
//


#include <stdio.h>
#include <stdlib.h>
/* #include <process.h>  ver 4.0 */
#define nmax 64
#define p_max 256
#define l_max 64        /* l_max is the maximum value of loop_register(v.3)  */

void falu(short alu,short A,short B,short *p_C,short *p_ov,short *p_z,short *p_m);

/****************************************************************************/
int main(void)
{
   struct mip{
	short Acl,Aco,Al,Ao,MAi,MAl,MAo,ADeo,ABo,A1o,A0o; /* A-bus control */
	short Bl,Bo,Cl,Co,MDi,BMDl,BMDo,Beo,B1o,B0o;      /* B-bus control */
	short Byo,AI,Lpl,Lpc;                         /* pipe line control */
	short r_w,MMDo,Memo;                             /* memory control */
	short alu;                                /* ALU operation control */
	short ncld,ovf,zf,mf,stp ;                       /* branch control */
	short emit;                                          /* emit field */
       };


   short mem[nmax]={1,12,30};
   short ac_reg,a_reg,ma_reg,b_reg,c_reg,md_reg,A,B,C,ov,z,m,i,jump=0,aluc,emfd;
   short pre_ov,pre_z,pre_m;   /* ver 2.1 */
   short loop_reg;
	  /*  A is the left side input of ALU                      */
	  /*  B is the right side input of ALU                     */
	  /*  C is the output of ALU                               */

/*****<<<  Here describe structure  >>>*****/
   struct mip *p_mip,*p_mip0,s[p_max]={ {0,1,0,1,0,1,0,1,1,1,1,
					0,1,0,1,0,1,1,1,1,1,
					1,0,0,0,
					1,1,0,
		/*  0 */                0,
					0,0,0,0,0,
					0},     /*  memory read */

				       {0,1,0,0,0,0,1,1,1,1,1,
					0,1,0,1,0,0,0,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  1 */                2,
					0,0,0,0,0,
					0},     /* A_reg - MD_reg */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  2 */                0,
					0,0,0,1,0,
					12},     /* if minus,jump to 12 */

				       {0,1,0,1,0,0,0,1,1,1,1,
					0,1,0,1,0,0,1,1,0,1,
					1,0,0,0,
					1,1,1,
		/*  3 */                1,
					0,0,0,0,0,
					0},    /* MA_reg + 1 */

				       {0,1,0,1,1,1,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  4 */                0,
					0,0,0,0,0,
					0}, /* save ALU result to MA_reg  */

				       {0,1,0,1,0,0,0,1,1,1,1,
					0,1,0,0,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  5 */                2,
					0,0,0,0,0,
					0},  /* MA_reg - C_reg */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  6 */                0,
					0,0,1,0,0,
					8},  /* if zero,jump to 8 */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  7 */                0,
					1,0,0,0,0,
					0},  /* jump to 0 non-conditionally */

				       {0,1,0,0,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,0,
					1,0,0,0,
					1,1,1,
		/*  8 */                1,
					0,0,0,0,0,
					0},  /* A_reg + 0 */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,2,1,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/*  9 */                0,
					0,0,0,0,0,
					0}, /*  save ALU result to MD_reg  */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					0,0,1,
		/* 10 */                0,
					0,0,0,0,0,
					0},  /* write to memory */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 11 */                0,
					0,0,0,0,1,
					0}, /* stp bit on : stop */

				       {0,1,0,1,0,0,0,1,1,1,1,
					0,1,0,1,0,0,1,1,1,0,
					1,0,0,0,
					1,1,1,
	       /* 12 */                 1,
					0,0,0,0,0,
					0},    /* MA_reg + 0 */

				       {0,1,0,1,0,0,1,1,1,1,1,
					1,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 13 */                0,
					0,0,0,0,0,
					0},    /* save the content of MA_reg */
					       /* to B_reg                   */
				       {0,1,0,1,0,0,1,1,1,0,1,
					0,1,0,0,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 14 */                3,
					0,0,0,0,0,
					0}, /* C_reg - 1 */
				       {0,1,0,1,1,1,1,1,1,1,1,
					0,1,1,0,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 15 */                0,
					0,0,0,0,0,
					0}, /* save ALU result to MA_reg and */
					    /* C_reg                         */
				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,1,1,1,1,1,
					1,0,0,0,
					1,1,0,
		/* 16 */                0,
					0,0,0,0,0,
					0}, /* memory read */

				       {0,1,0,1,0,0,1,1,1,0,1,
					0,1,0,0,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 17 */                1,
					0,0,0,0,0,
					0}, /* C_reg + 1 */

				       {0,1,0,1,1,1,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 18 */                0,
					0,0,0,0,0,
					0}, /* save ALU result to MA_reg*/

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					0,0,1,
		/* 19 */                0,
					0,0,0,0,0,
					0}, /* write to memory */

				       {0,1,0,1,0,0,1,1,0,1,1,
					0,1,0,0,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 20 */                2,
					0,0,0,0,0,
					0}, /* B_reg - C_reg */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 21 */                0,
					0,0,1,0,0,
					23}, /* if zero,jump to 23 */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 22 */                0,
					1,0,0,0,0,
					14}, /* jump to 14 non-conditionally */

				       {0,1,0,1,0,0,1,1,1,1,0,
					0,0,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 23 */                1,
					0,0,0,0,0,
					0}, /* B_reg + 0*/

				       {0,1,0,1,1,1,1,1,1,1,1,
					0,1,0,1,0,0,1,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 24 */                0,
					0,0,0,0,0,
					0}, /* save ALU result to MA_reg */

				       {0,1,0,1,0,0,1,1,1,1,1,
					0,1,0,1,0,0,0,1,1,1,
					1,0,0,0,
					1,1,1,
		/* 25 */                0,
					1,0,0,0,0,
					8} }; /*jump to 8 non-conditionally*/



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
/*    for(i=0;i<nmax;i++){mem[i]=0;}     ver 3.1 not intitialinze memory     */

/*****<<<  Here describe Register initial value  >>>*****/
     ac_reg=0; a_reg=13; ma_reg=0; b_reg=0; c_reg=3; md_reg=0;loop_reg=0;
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
	     +(p_mip->B1o)+(p_mip->B0o)) < 5 ){
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

    /*  check branch condition  */
    /*                          */
     if( ((ov==1)&&((p_mip->ovf)==1)) || ((z==1)&&((p_mip->zf)==1))
	 || ((m==1)&&((p_mip->mf)==1)) ) { jump=1;}
       else { jump=0;}
     pre_ov=ov; pre_z=z; pre_m=m;  /* save previous flags  ver 2.1 */

    /*  ALU  */
    /*       */
     aluc=(p_mip->alu);
     falu(aluc,A,B,p_C,p_ov,p_z,p_m);

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
	     printf("r_w =%1d MMDo=%1d Memo=%1d alu=%1d\n",
		     p_mip->r_w,p_mip->MMDo,p_mip->Memo,p_mip->alu);
	     printf("ncld=%1d ovf=%1d zf =%1d mf  =%1d stp =%1d emit=%7d\n",
		     p_mip->ncld,p_mip->ovf,p_mip->zf,p_mip->mf,p_mip->stp,
		     p_mip->emit);

    /* registers and memory contents */
	    printf("intermediate results of registers and memory contents:\n");
	    printf("ac_reg=%6d a_reg =%6d ma_reg=%6d\n",ac_reg,a_reg,ma_reg);
	    printf("b_reg =%6d c_reg =%6d md_reg=%6d,loop_reg=%6d\n",b_reg,c_reg,md_reg,loop_reg);
	    printf("ov=%1d z =%1d m =%1d\n",pre_ov,pre_z,pre_m);  /* ver 2.1 */

	    printf("memory contents:\n");
	    for(i=0;i<nmax;i++){
		     if((i%8)==0){printf("\n");}
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
               }  /* ver 4.0 add */
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
               }  /* ver 4.0 add */
             /* set ovf flag ; if 1bit left shift A then carry MSB */
               temp0 = A << 1;

	       if(temp0 < 0) { *p_m=1; }
	       if(temp0 == 0) { *p_z=1; }
	       *p_C=temp0;
	       break;
  }


}

