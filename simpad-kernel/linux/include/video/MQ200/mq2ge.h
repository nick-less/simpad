#ifndef _VIDEO_MQ200_MQ2GE_H
#define _VIDEO_MQ200_MQ2GE_H


/* Misc. GE Function Macro */
#undef CHECK_SRCFIFO 
#define RGB_RASTER_CHECK
#define INCLUDE_GENERIC_CODE

#ifdef CHECK_SRCFIFO
#define geWAITSRCFIFO(cnt)			geWaitSrcFIFO(cnt) 
#define	geWAITCMDFIFO(cnt)			geWaitCmdFIFO(cnt)
#define geWAITNOTBUSY()				geWaitNotBusy()
#else
#define geWAITSRCFIFO(cnt) 
#define geWAITCMDFIFO(cnt) 
#define geWAITNOTBUSY()
#endif

/* Additional UGL Raster Ops */
#define UGL_RASTER_OP_NOP			0x00000000L

#define UGL_RASTER_OP_BLACKNESS			0x00000001L
#define UGL_RASTER_OP_BSRC_BLACK		0x00000002L
#define UGL_RASTER_OP_BSRC_OCOPY		0x00000003L
#define UGL_RASTER_OP_BSRC_XCOPY		0x00000004L

#define UGL_RASTER_OP_WHITENESS			0x00010001L
#define UGL_RASTER_OP_WSRC_COPY			0x00010002L
#define UGL_RASTER_OP_WSRC_WHITE		0x00010003L
#define UGL_RASTER_OP_WSRC_INVERT		0x00010004L

#define UGL_RASTER_OP_SRC_COPY			0x00020001L
#define UGL_RASTER_OP_SRCDEST_AND		0x00020002L
#define UGL_RASTER_OP_SRCDEST_OR		0x00020003L
#define UGL_RASTER_OP_SRCDEST_XOR		0x00020004L

#define UGL_RASTER_OP_DEST_COPY			0x00030001L
#define UGL_RASTER_OP_DESTSRC_AND		0x00030002L
#define UGL_RASTER_OP_DESTSRC_OR		0x00030003L
#define UGL_RASTER_OP_DESTSRC_XOR		0x00030004L

/* MediaQ Raster Ops */
#define MQ200_SOURCE_ROP			0x01
#define MQ200_PATTERN_ROP			0x02
#define MQ200_GE_NOP				0x000000AAL
#define MQ200_GE_BLACKNESS			0x00000000L
#define MQ200_GE_WHITENESS			0x000000FFL
#define MQ200_GE_SRC_INVERT			0x00000033L
#define MQ200_GE_SRC_COPY			0x000000CCL
#define MQ200_GE_SRCDEST_XOR		0x00000066L
#define MQ200_GE_SRCDEST_AND		0x00000088L
#define MQ200_GE_SRCDEST_OR			0x000000EEL
#define MQ200_GE_PATTERN_INVERT		0x0000000FL
#define MQ200_GE_PATTERN_COPY		0x000000F0L
#define MQ200_GE_PATDEST_XOR		0x0000005AL
#define MQ200_GE_PATDEST_AND		0x000000A0L
#define MQ200_GE_PATDEST_OR			0x000000FAL
/* MediaQ Raster Ops mapping table */
#define UGL_NR_OPERAND					4
#define UGL_NR_OPERATION				5

#define geREG_2( idx1, val1, idx2, val2 ) \
        geREG( idx2, val2 ); \
        geREG( idx1, val1 )
#define geREG_3( idx1, val1, idx2, val2, idx3, val3 ) \
        geREG_2( idx2, val2, idx3, val3 ); \
        geREG( idx1, val1 )
#define geREG_4( idx1, val1, idx2, val2, idx3, val3, idx4, val4 ) \
        geREG_3( idx2, val2, idx3, val3, idx4, val4 ); \
        geREG( idx1, val1 )
#define geREG_5( idx1, val1, idx2, val2, idx3, val3, idx4, val4, idx5, val5 ) \
        geREG_4( idx2, val2, idx3, val3, idx4, val4, idx5, val5 ); \
        geREG( idx1, val1 )

/* Declare MQ200 GE Utility Functions */
void geWaitNotBusy(void);
void geWaitCmdFIFO(u32 cnt);
void geWaitSrcFIFO(u32 cnt);

#endif /* _VIDEO_MQ200_MQ2GE_H */
