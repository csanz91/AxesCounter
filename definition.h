typedef struct{
	const char *input_places;
	const char *output_places;
	void (*action) ();
	char (*condition) (unsigned char input[]);
} T_TRAN;
