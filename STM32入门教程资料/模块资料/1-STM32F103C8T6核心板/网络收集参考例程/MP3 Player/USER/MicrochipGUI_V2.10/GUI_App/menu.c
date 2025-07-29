/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               menu.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-27
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include <menu.h>


/* Private variables ---------------------------------------------------------*/
SCREEN_STATES  ScreenState = DISPLAY_MP3; /* current state of play */
extern uint8_t MusicFileCount;


void StartMenu(void)
{  	
	LISTBOX*       pLb;
	LISTITEM*      pItem; 
	PICTURE*       pPicture;  		
   	GOL_SCHEME*    alt1Scheme;				
	GOL_SCHEME*    alt2Scheme;	
	GOL_SCHEME*    alt3Scheme;
				
	GOLFree();	
	
	alt1Scheme = GOLCreateScheme(); 	/* create alternative style scheme */
	alt2Scheme = GOLCreateScheme(); 	/* create alternative style scheme */
	alt3Scheme = GOLCreateScheme(); 	/* create alternative style scheme */		 
	
	/* 声音进度条 */
	{
	  alt1Scheme->Color0 = RGB565CONVERT(155, 235, 117);
 	  alt1Scheme->EmbossDkColor = RGB565CONVERT(111, 190, 183);
	  alt1Scheme->EmbossLtColor = RGB565CONVERT(82, 146, 174);
 	}
	/* 列表框 */
	{
	  alt2Scheme->Color0 = WHITE;
      alt2Scheme->Color1 = RGB565CONVERT(49, 106, 197);
	  alt2Scheme->EmbossDkColor = WHITE;
	  alt2Scheme->EmbossLtColor = RGB565CONVERT(0xA9, 0xDB, 0xEF);
      alt2Scheme->TextColor1 = BLACK;
      alt2Scheme->TextColor0 = BLACK;
	  alt2Scheme->pFont = (void*)&externalfont;	
	}
	/* 列表框滑动条 */
	{
	  alt3Scheme->Color0 = RGB565CONVERT(198, 213, 253);
 	  alt3Scheme->EmbossDkColor = RGB565CONVERT(183, 202, 245);
	  alt3Scheme->EmbossLtColor = WHITE;
	  alt3Scheme->pFont = (void*)&externalfont;
 	}
	  
	BtnCreate(ID_BUTTON1, 				/* button ID             */
              7,97,7+16,97+24,          /* dimensio	             */
              0,					   	/* set radius            */
              BTN_DRAW,  			   	/* draw a beveled button */
              (void*)&previous_on,      /* bitmap				 */
              NULL, 				  	/* text					 */
              _pDefaultGolScheme);      /* use alternate scheme	 */

	BtnCreate(ID_BUTTON2, 				/* button ID             */
              24,97,24+17,97+24,        /* dimensio	             */
              0,					   	/* set radius            */
              BTN_DRAW,  			   	/* draw a beveled button */
              (void*)&play_on,          /* bitmap				 */
              NULL, 				    /* text					 */
              _pDefaultGolScheme);      /* use alternate scheme	 */

	BtnCreate(ID_BUTTON3, 				/* button ID             */
              42,97,42+17,97+24,        /* dimensio	             */
              0,					    /* set radius            */
              BTN_DRAW,  			    /* draw a beveled button */
              (void*)&stop_on,          /* bitmap				 */
              NULL, 				    /* text					 */
              _pDefaultGolScheme);      /* use alternate scheme	 */

	BtnCreate(ID_BUTTON4, 				/* button ID             */
              60,97,60+17,97+24,        /* dimensio	             */
              0,					   	/* set radius            */
              BTN_DRAW,  			   	/* draw a beveled button */
              (void*)&next_on,          /* bitmap				 */
              NULL, 			        /* text					 */
              _pDefaultGolScheme);      /* use alternate scheme	 */

	BtnCreate(ID_BUTTON5, 				/* button ID 	         */
              91,101,91+16,101+16,      /* dimension	         */
              0,						/* set radius            */
              BTN_DRAW, 			    /* draw a beveled button */
			  (void*)&UNMUTE_HOVER,     /* bitmap				 */
              NULL, 			        /* text					 */
              _pDefaultGolScheme);	    /* use alternate scheme	 */
	 
	BtnCreate(ID_BUTTON6,             	/* button ID             */
              220,124,240,124+20,       /* dimensio	             */
              0,					   	/* set radius            */
              BTN_DRAW,  		        /* draw a beveled button */
              (void*)&press_up,         /* bitmap				 */
              NULL, 				    /* text					 */
              _pDefaultGolScheme);      /* use alternate scheme	 */
	
    BtnCreate(ID_BUTTON7, 				/* button ID             */
              220,320-20,240,320,       /* dimensio	             */
              0,					    /* set radius            */
              BTN_DRAW, 			    /* draw a beveled button */
              (void*)&rpress_up,        /* bitmap				 */
              NULL, 				    /* text					 */
              _pDefaultGolScheme);	    /* use alternate scheme	 */
   
    LbCreate(ID_LISTBOX1,        	    /* button ID             */
             0,124,240-20,320,          /* dimensio	             */
             LB_DRAW|LB_SINGLE_SEL,     /* will be dislayed after creation */
             NULL,
             alt2Scheme);               /* use alternate scheme	 */

	pLb = (LISTBOX *)GOLFindObject(ID_LISTBOX1);
	/* 设置字体高度 */
	pLb->textHeight = 23;
			 
    while( (MusicFileCount--) && (PlayFile != NULL) )
	{
	  pItem = NULL;               
	  pItem = LbAddItem(pLb, pItem,PlayFile->filename, NULL, 0 ,0);
	  LbSetBitmap(pItem, (void*)&musicoff);	 
	  PlayFile = PlayFile->next;
	}

    PictCreate(ID_PICTURE1,	                     /* ID                                                     */
               0,0,240,124,                      /* dimension                                              */
               PICT_FRAME | PICT_DISABLED ,    	 /* will no be dislayed, has frame                         */
               1,                       	     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
               (void*)&Smart_MP3,                /* bitmap                                                 */
               _pDefaultGolScheme);              /* default GOL scheme                                     */	
 	
	pPicture = (PICTURE *)GOLFindObject(ID_PICTURE1);
	PictDraw(pPicture);

	PictCreate(ID_PICTURE2, 				     /* ID 	                                                   */
               217,100,217+20,100+20,            /* dimension	                                           */
               PICT_DRAW,                        /* will  be dislayed                                      */
               1,								 /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
			   (void*)&sound_add,                /* bitmap                                                 */
               _pDefaultGolScheme);	             /* default GOL scheme                                     */

	PictCreate(ID_PICTURE3, 				     /* ID 	                                                   */
              114,101,114+20,101+20,             /* dimension	                                           */
              PICT_DRAW,						 /* will  be dislayed                                      */
              1, 			                     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
              (void*)&sound_delete,              /* bitmap                                                 */
              _pDefaultGolScheme);	             /* default GOL scheme                                     */

    PictCreate(ID_PICTURE4,	                     /* ID 	                                                   */
               70,30,70+16*10,30+16,             /* dimension	                                           */
               PICT_DISABLED ,    	             /* will no be dislayed                                    */
               1,                       	     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
               (void*)&Smart_MP3_Part1,          /* bitmap                                                 */
               _pDefaultGolScheme);              /* default GOL scheme                                     */	

    PictCreate(ID_PICTURE5,	                     /* ID 	                                                   */
               70,60,70+8*5 ,60+16 ,             /* dimension	                                           */
               PICT_DISABLED ,    	             /* will no be dislayed                                    */
               1,                       	     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
               (void*)&Smart_MP3_Part2,          /* bitmap                                                 */
               _pDefaultGolScheme);              /* default GOL scheme                                     */	

    PictCreate(ID_PICTURE6,	                     /* ID 	                                                   */
               120,60,120+16*3,60+16,            /* dimension	                                           */
               PICT_DISABLED ,    	             /* will no be dislayed                                    */
               1,                       	     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
               (void*)&Smart_MP3_Part3,          /* bitmap                                                 */
               _pDefaultGolScheme);              /* default GOL scheme                                     */	

    PictCreate(ID_PICTURE7,	                     /* ID 	                                                   */
               178,60,178+8*3,60+16,             /* dimension	                                           */
               PICT_DISABLED ,    	             /* will no be dislayed                                    */
               1,                       	     /* 改写低层为1时,图片中黑色部分放弃操作 参考PutImage16BPP */
               (void*)&Smart_MP3_Part4,          /* bitmap                                                 */
               _pDefaultGolScheme);              /* default GOL scheme                                     */	

	/* 声音进度条 */		
    SldCreate(ID_SLIDER1,                        /* ID                                                     */
              135,105,215,105+8,                 /* dimension                                              */
              SLD_DRAW |SLD_SCROLLBAR ,          /* will be dislayed after creation                        */
              80,                                /* range                                                  */
              1,                                 /* page                                                   */
              75, 	                             /* pos                                                    */
              alt1Scheme);                       /* alternative GOL scheme                                 */

	/* 播放进度条 */
    SldCreate(ID_SLIDER2,                        /* ID                                                     */
              8,83,230,83+10,                    /* dimension                                              */
              SLD_DRAW|SLD_SCROLLBAR ,           /* will be dislayed after creation                        */
              200,   							 /* range                                                  */
              1,                                 /* page                                                   */
              0, 	 							 /* pos                                                    */
              _pDefaultGolScheme);               /* alternative GOL scheme                                 */

	/* 列表框滑动条 */
	SldCreate(ID_SLIDER3,                    	 /* ID                                                     */
              220,124+20,240,320-20,  			 /* dimension                                              */
              SLD_DRAW |SLD_SCROLLBAR|SLD_VERTICAL ,  /* will be dislayed after creation                        */
              LbGetCount(pLb)-1,           		 /* range                                                  */  
              1,                                 /* page                                                   */ 
              LbGetCount(pLb)-1, 	             /* pos                                                    */
              alt3Scheme);              		 /* alternative GOL scheme                                 */
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

