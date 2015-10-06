// Note that we've skipped the silly <!-- part by now
typedef struct {
#if DMXB >= 4
	uint32_t
#else
	uint16_t
#endif
		nStrings;
} V(dmx_dict_header);

dmx* V(dmx_from_buffer) (char* buffer, unsigned int length);
