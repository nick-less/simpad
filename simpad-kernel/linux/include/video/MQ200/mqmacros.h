#ifndef _VIDEO_MQ200_MQMACROS_H
#define _VIDEO_MQ200_MQMACROS_H

#ifdef CHECK_SRCFIFO

#define PUMP_PACKED_SRCFIFO(pSrcData,srcStride,nDwords,height,extras) \
	{ \
		u32 *pData; \
		u32	i,j; \
		while( height-- ) \
		{ \
			pData = (u32 *)((u32)(pSrcData + 3UL) & ~0x03UL); \
			j = nDwords; \
			while ( j >= SRC_FIFO_MAX_DWORDS ) \
			{ \
				geWAITSRCFIFO( SRC_FIFO_MAX_DWORDS ); \
				for ( i = 0; i < SRC_FIFO_MAX_DWORDS; i++ ) \
					geREG(SRC_IMAGE_DATA, *pData++); \
				j -= SRC_FIFO_MAX_DWORDS; \
			} \
			geWAITSRCFIFO( j ); \
			while( j-- ) \
				geREG(SRC_IMAGE_DATA, *pData++);  \
			pSrcData += srcStride; \
		} \
		geWAITSRCFIFO( extras ); \
		while( extras-- ) \
			geREG(SRC_IMAGE_DATA, 0UL); \
	} 


#define PUMP_REAL_PACKED_SRCFIFO(pSrcData,nDwords,extras) \
	{ \
		u32 *pData =(u32 *)pSrcData; \
		u32 i; \
		while(nDwords) \
		{ \
		     if (nDwords >= SRC_FIFO_MAX_DWORDS) \
		     { \
			geWAITSRCFIFO( SRC_FIFO_MAX_DWORDS ); \
			for (i = SRC_FIFO_MAX_DWORDS; i > 0; i--) \
				geREG(SRC_IMAGE_DATA, *pData++); \
                        nDwords -= SRC_FIFO_MAX_DWORDS; \
		     } \
		     else \
		     { \
			geWAITSRCFIFO( nDwords ); \
			for (i = nDwords; i > 0; i--) \
				geREG(SRC_IMAGE_DATA, *pData++); \
                        nDwords -= nDwords; \
		     } \
		} \
                geWAITSRCFIFO(extras); \
		while( extras-- ) \
		     geREG(SRC_IMAGE_DATA, 0UL); \
	} 

#else /* CHECK_SRCFIFO */

#define PUMP_PACKED_SRCFIFO(pSrcData,srcStride,nDwords,height,extras) \
	{ \
		u32 *pData; \
		u32	i; \
		while( height-- ) \
		{ \
			pData = (u32 *)((u32)(pSrcData + 3UL) & ~0x03UL); \
			for ( i = 0; i < nDwords; i++ ) \
				geREG(SRC_IMAGE_DATA, *pData++); \
			pSrcData += srcStride; \
		} \
		while( extras-- ) \
			geREG(SRC_IMAGE_DATA, 0UL); \
	}

#define PUMP_REAL_PACKED_SRCFIFO(pSrcData,nDwords,extras) \
	{ \
		u32 *pData =(u32 *)pSrcData; \
		while(nDwords--) \
		     geREG(SRC_IMAGE_DATA, *pData++); \
		while( extras-- ) \
		     geREG(SRC_IMAGE_DATA, 0UL); \
	} 	

#endif

#endif /* _VIDEO_MQ200_MQMACROS_H */
